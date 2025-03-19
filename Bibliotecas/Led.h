#include "hardware/pwm.h"
#include "pico/stdlib.h"

#define LED_R 11
#define LED_G 12
#define LED_B 13

/*SETUP LEDS GPIO*/
void setupLED(){
    stdio_init_all();

    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_pull_down(LED_R);
    gpio_put(LED_R, 1);

    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_pull_down(LED_G);
    gpio_put(LED_G, 1);

    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);
    gpio_pull_down(LED_B);
    gpio_put(LED_B, 1);
}

/* */

/*LÓGICA PARA CONTROLAR A LUMINOSIDADE DO LED VIA BOTÃO COM USO DO PWM*/
