# Monitor de Temperatura com Raspberry Pi Pico W

##  Visão Geral
O projeto tem como base um exemplo já feito no repositório oficial BitDogLab em C, que utiliza o wifi para monitorar os botões e led da placa, este repositório adapta e cria novas funções.
Utilizando um **Raspberry Pi Pico W** para monitorar a temperatura do sensor interno, controle dos leds e exibi-los em uma página web para o usuário.

## Pinos Utilizados
1. **Leds RGB**
    - Pino 11 (GREEN) - para o led verde
    - Pino 12 (RED) - para o led vermelho
    - Pino 13 (BLUE) - para o led azul

2. **Sensor interno do RP2040**
   - ADC(4) - Mede a temperatura do próprio microcontrolador.

##  Funcionamento
- O código lê a temperaturs do sensor e exibe os valores em uma página HTML.
- Botão de ligar e desligar os leds
- Controle de luminosidade
- O **Raspberry Pi Pico W** se conecta à rede Wi-Fi e roda um **servidor HTTP** na porta 80.

##  O que mudar?
- Mude apenas as configurações de wifi nas variáveis **WIFI_SSID** (que será o nome de sua rede WIFI) e **WIFI_PASS** que será a senha do seu WIFI

###  Compilar e Executar
1. Compile o código para o **Raspberry Pi Pico W** utilizando a extensão do Visual Studio Code.
2. Carregue o binário .uf2 no dispositivo.
3. Abra o **monitor serial** e veja o **endereço IP** atribuído ao Pico W.
4. No navegador, acesse:  
   ```
   http://<IP_DO_PICO_W>
   ```
   
##  Código-Fonte em C dos arquivos principais
1. **Wifi.c**
```c
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include "Bibliotecas/Server.h"

#define WIFI_SSID ""  // Nome da sua rede Wi-Fi
#define WIFI_PASS ""   // Senha da sua rede Wi-Fi

int main() {
    stdio_init_all();
    sleep_ms(10000);
    printf("Iniciando servidor HTTP\n");

    // Inicializa Wi-Fi

    if (cyw43_arch_init()){
        printf("Erro ao inicializar o Wi-Fi\n");
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)){
        printf("Falha ao conectar ao Wi-Fi\n");
    }

    else{
        uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Wi-Fi conectado! IP: %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }
    
    //inicia os leds pwm
    setupPwm();

    //inicia os ADC_GPIO
    setupADC();

    // Inicia o servidor HTTP
    start_http_server();

    // Loop principal
    while (true) {
        cyw43_arch_poll();
        sleep_ms(100);

    }

    cyw43_arch_deinit();
    return 0;
}

```

2. **Server.h**
```c
#include "lwip/tcp.h"
#include <string.h>
#include "Temperatures.h"
#include "Led.h"

// Buffer para resposta HTTP
char http_response[1024];

// --- Função para criar a resposta HTML ---
void create_http_response() {

    float internal_temp = read_internal_temp();
    
    snprintf(http_response, sizeof(http_response),
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "  <meta charset=\"UTF-8\">"
        "  <title>Monitor de Temperatura</title>"
        "  <meta content='width=device-width, initial-scale=1.0'>"
        "  <style>"
        "    body { font-family: Arial, sans-serif; text-align: center; padding: 50px; color: white; background-color: black; }"
        "    p { font-size: 24px; font-weight: bold; color: #007BFF; }"
        "  </style>"
        "    <script> "
            "   function updateLuminosidade(value) {"
                    "fetch(`/led/luminosidade?value=` + value, { method: 'GET' })"
                        ".then(response => console.log(\"Brilho ajustado para: \" + value))"
                        ".catch(error => console.error(\"Erro ao ajustar brilho:\", error));}"
        "   </script>"
        "</head>"
        "<body>"
        "<h1>Temperatura Atual:  %.2f°C</h1>"
        "<p>Ajuste a luminosidade do LED usando o controle deslizante.</p>"
        "<input type='range' min='0' max='255' value='128' oninput='updateLuminosidade(this.value)'>"
        "<br>"
        "<a href=\"/led/on\"><button>Ligar Led</button></a>"
        "<a href=\"/led/off\"><button>Desligar Led</button></a>"
        "<a href=\"/led/animacao\"><button>Animação de Led</button></a>"
        "</body>"
        "</html>\r\n",
        internal_temp);
}

// Callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    /** \brief  para receber requisiões
    *           com isso é possível determinar qual botão o usuário clicou
    *           assim enviando de volta o comando escolhido por meio de condicionais
    */
    char *request = (char *)p->payload;
    
    if(strstr(request, "GET /led/on")){
        controleLedLigar();
    } 

    else if(strstr(request, "GET /led/off")){
        controleLedDesligar();
    } 

    else if (strstr(request, "GET /led/animacao")){
        animacaoLed();
    }
    /** \brief Neste endpoint ocorre o tratamento do número enviado pelo input range do html
    *
    */
    
    else if(strstr(request, "GET /led/luminosidade?value=")){
        char *ptr = strstr (request, "value=");
        if(ptr){
            int luminosidade = atoi(ptr + 6);
            if (luminosidade >= 0 && luminosidade <= 255) {
                sleep_ms(50);
                controleDeIluminacao(luminosidade);
            }
        }
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

```
3. **Leds.h**
```c
#include "hardware/pwm.h"
#include "pico/stdlib.h"

/** \brief Pinagem dos leds
 * --- ---
*/
#define LED_R 13
#define LED_G 11
#define LED_B 12

/** \brief Variável que servirão para configurar o PWM para os leds
 * --- ---
*/
const uint16_t PERIOD = 256;
const float DIVIDER_PWM = 16.0;
const uint16_t LED_STEP = 5;
uint16_t led_level = 0;
uint duty_cycle = 0;

_Bool ligado = 0;
_Bool loopAni = 1;


/** \brief Função para configurar os leds para sinal digital (0) - desligado (1) - ligado
 * --- ---
*/
void setupLED(){
    stdio_init_all();

    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_pull_down(LED_R);
    gpio_put(LED_R, ligado);

    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_pull_down(LED_G);
    gpio_put(LED_G, ligado);

    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);
    gpio_pull_down(LED_B);
    gpio_put(LED_B, ligado);
}

/** \brief Função para configurar os leds com pwm
 * --- ---
*/
void setupPwm(){
    gpio_set_function(LED_B, GPIO_FUNC_PWM);
    uint16_t slice_B = pwm_gpio_to_slice_num(LED_B);
    pwm_set_clkdiv(slice_B, DIVIDER_PWM);
    pwm_set_wrap(slice_B, PERIOD);
    pwm_set_gpio_level(LED_B, led_level);
    pwm_set_enabled(slice_B, true);

    gpio_set_function(LED_R, GPIO_FUNC_PWM);
    uint16_t slice_R = pwm_gpio_to_slice_num(LED_R);
    pwm_set_clkdiv(slice_R, DIVIDER_PWM);
    pwm_set_wrap(slice_R, PERIOD);
    pwm_set_gpio_level(LED_R, led_level);
    pwm_set_enabled(slice_R, true);

    
    gpio_set_function(LED_G, GPIO_FUNC_PWM);
    uint16_t slice_G = pwm_gpio_to_slice_num(LED_G);
    pwm_set_clkdiv(slice_G, DIVIDER_PWM);
    pwm_set_wrap(slice_G, PERIOD);
    pwm_set_gpio_level(LED_G, led_level);
    pwm_set_enabled(slice_G, true);
    
}

/** \brief Função para ligar o led
 * --- ---
*/
void controleLedLigar(){
    loopAni = 0;

    pwm_set_gpio_level(LED_R, PERIOD);
    pwm_set_gpio_level(LED_G, PERIOD);
    pwm_set_gpio_level(LED_B, PERIOD);
}

/** \brief Função para desligar o led
 * --- ---
*/
void controleLedDesligar(){
    loopAni = 0;

    pwm_set_gpio_level(LED_R, 0);
    pwm_set_gpio_level(LED_G, 0);
    pwm_set_gpio_level(LED_B, 0);
}

/** \brief Função para animação de fade in e fade out do led
 *  --- ---
*/
void animacaoLed(){
    uint up_down = 1;
    loopAni = 1;
    
    while (loopAni){

        pwm_set_gpio_level(LED_R, led_level);
        pwm_set_gpio_level(LED_G, led_level);
        pwm_set_gpio_level(LED_B, led_level);

        sleep_ms(100);

        if(up_down){
            led_level+=LED_STEP;
            if(led_level >= PERIOD){up_down = 0;}
        }

        else{
            led_level -= LED_STEP;
            if(led_level < LED_STEP){up_down = 1;}
        }

        if(loopAni == 0){break;}
        
    }
}

/** \brief Função para controle da luminosidade do led
 * 
 * \param led_level determina a quantidade de luminosidade que o led terá
*/
void controleDeIluminacao(int led_level){
    loopAni = 0;

    pwm_set_gpio_level(LED_R, led_level);
    pwm_set_gpio_level(LED_G, led_level);
    pwm_set_gpio_level(LED_B, led_level);


}

```
4. **Temperatures.h**
```c

#include "hardware/adc.h"

#define ADC_GPIO 26
#define ADC_INT 4 // ADC4 = Sensor interno do RP2040

/** \brief Função para ler a temperatura do sensor interno do RP2040
*   --- ---
*/ 
float read_internal_temp() {
    adc_select_input(ADC_INT); // ADC4 = Sensor interno do RP2040
    float raw = (float) adc_read();
    float voltage = raw * 3.3f / (1 << 12); // Converte para tensão (0-3.3V)

    float tempC = 27.0f - (voltage - 0.706f) / 0.001721f; // Fórmula do datasheet
    printf("Temperatura: %.2f", tempC);
    return tempC; 
}


/**
 * \brief Função para inicializar configurações do Analogic To Digital Converter do RP2040
 * --- ---
 */
void setupADC(){
    // Inicializa o ADC para leitura da temperatura
    adc_init();
    adc_set_temp_sensor_enabled(true);
    // Configura o ADC4
    adc_gpio_init(ADC_INT);
}
```

