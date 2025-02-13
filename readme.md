# Monitor de Temperatura com Raspberry Pi Pico W

##  Visão Geral
O projeto tem como base um exemplo já feito no repositório oficial BitDogLab em C, que utiliza o wifi para monitorar os botões e led da placa, este repositório apenas adapta e cria duas novas funções.
Utilizando um **Raspberry Pi Pico W** para monitorar a temperatura de diferentes sensores e exibi-los em uma página web.

##  Sensores de Temperatura Utilizados
1. **Sensor interno do RP2040**
   - Mede a temperatura do próprio microcontrolador.
2. **Sensor TMP36 (ou outro sensor analógico)**
   - Conectado ao **GPIO26 (ADC0)**.

##  Funcionamento
- O código lê as temperaturas dos sensores e exibe os valores em uma página HTML.
- A página recarrega automaticamente a cada **5 segundos**.
- O **Raspberry Pi Pico W** se conecta à rede Wi-Fi e roda um **servidor HTTP** na porta 80.

##  O que mudar?
- Mude apenas as configurações de wifi nas variáveis **WIFI_SSID** (que será o nome de sua rede WIFI) e **WIFI_PASS** que será a senha do seu WIFI
##  Código-Fonte em C
```c
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include <stdio.h>
#include <string.h>
#include "hardware/adc.h"

#define WIFI_SSID ""  // Nome da sua rede Wi-Fi
#define WIFI_PASS ""   // Senha da sua rede Wi-Fi

// Buffer para resposta HTTP
char http_response[1024];

// Função para ler a temperatura ambiente do sensor TMP36 (conectado ao ADC0 - GPIO26)
// --- Função para ler a temperatura do sensor interno do RP2040 ---
float read_internal_temp() {
    adc_select_input(4); // ADC4 = Sensor interno do RP2040
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / 4096; // Converte para tensão (0-3.3V)
    return 27.0f - (voltage - 0.706f) / 0.001721f; // Fórmula do datasheet
}

// --- Função para ler a temperatura do TMP36 (ou outro sensor analógico) ---
float read_tmp36() {
    adc_select_input(0); // ADC0 = GPIO26
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / 4096;
    return (voltage - 0.5f) * 100.0f; // Fórmula do TMP36 para Celsius
}

// --- Função para criar a resposta HTML ---
void create_http_response() {
    float internal_temp = read_internal_temp();
    float external_temp = read_tmp36();
    
    snprintf(http_response, sizeof(http_response),
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "  <meta charset=\"UTF-8\">"
        "  <title>Monitor de Temperatura</title>"
        "  <meta http-equiv='refresh' content='5'>" // Atualiza a página a cada 5 segundos
        "  <style>"
        "    body { font-family: Arial, sans-serif; text-align: center; padding: 50px; }"
        "    h1 { color: #333; }"
        "    p { font-size: 24px; font-weight: bold; color: #007BFF; }"
        "  </style>"
        "</head>"
        "<body>"
        "  <h1>Temperatura Atual</h1>"
        "  <p>🌡️ Interna: %.2f°C</p>"
        "  <p>🌡️ Externa (TMP36): %.2f°C</p>"
        "</body>"
        "</html>\r\n",
        internal_temp, external_temp);
}

// Callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Atualiza o HTML com a temperatura atual
    create_http_response();
    
    // Envia a resposta HTTP
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);

    // Libera o buffer recebido
    pbuf_free(p);
    return ERR_OK;
}

// Callback de conexão: associa o http_callback à conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

// Função para iniciar o servidor HTTP
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }

    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);

    printf("Servidor HTTP rodando na porta 80...\n");
}

int main() {
    stdio_init_all();
    sleep_ms(10000);
    printf("Iniciando servidor HTTP\n");

    // Inicializa Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return 1;
    } else {
        uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Wi-Fi conectado! IP: %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    // Inicializa o ADC para leitura da temperatura
    adc_init();
    adc_gpio_init(26); // Configura GPIO26 (ADC0)

    // Inicia o servidor HTTP
    start_http_server();

    // Loop principal
    while (true) {
        cyw43_arch_poll();
        sleep_ms(100);
        sleep_ms(5000);
        printf("Interna: %.2f°C | Externa: %.2f°C\n", read_internal_temp(), read_tmp36());
    }

    cyw43_arch_deinit();
    return 0;
}

```

##  Como Usar
###  Conectar o Sensor TMP36
- **VCC** → **3.3V**
- **GND** → **GND**
- **OUT** → **GPIO26 (ADC0)**

###  Compilar e Executar
1. Compile o código para o **Raspberry Pi Pico W** utilizando a extensão do Visual Studio Code.
2. Carregue o binário .uf2 no dispositivo.
3. Abra o **monitor serial** e veja o **endereço IP** atribuído ao Pico W.
4. No navegador, acesse:  
   ```
   http://<IP_DO_PICO_W>
   ```

