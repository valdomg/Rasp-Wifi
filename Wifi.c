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
    while (true){
        if (cyw43_arch_init()) {
            printf("Erro ao inicializar o Wi-Fi\n");
        }
    
        cyw43_arch_enable_sta_mode();
        printf("Conectando ao Wi-Fi...\n");
    
        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
            printf("Falha ao conectar ao Wi-Fi\n");
            
        } else {
            uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
            printf("Wi-Fi conectado! IP: %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
            break;
        }
        sleep_ms(1000);
    }
    
    

    //inicia os leds_gpio
    //setupLED();

    setupPwm();

    //inicia os ADC_GPIO
    setupADC();

    // Inicia o servidor HTTP
    start_http_server();

    // Loop principal
    while (true) {
        cyw43_arch_poll();
        sleep_ms(100);
       // printf("Interna: %.2f°F \n", read_internal_temp());
       // printf("Led R: %d , led G: %d, led B: %d \n", gpio_get(LED_R), gpio_get(LED_G),gpio_get(LED_G));

    }

    cyw43_arch_deinit();
    return 0;
}
