#include "lwip/tcp.h"
#include <string.h>
#include "Temperatures.h"
#include "Led.h"

// Buffer para resposta HTTP
char http_response[1024];

// --- Função para criar a resposta HTML ---
void create_http_response() {

    float internal_temp = read_internal_temp();
    setupLED();
    
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
        "  <p>🌡️ Interna: %.2f°F</p>"
        "  <h1> Qual Led está ativo?</h1>"
        "  <p> LED_R: %s </p>"
        "</body>"
        "</html>\r\n",
        internal_temp, gpio_get(LED_R) ? "true" : "false");
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