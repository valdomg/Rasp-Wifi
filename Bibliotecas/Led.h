#include "hardware/pwm.h"
#include "pico/stdlib.h"

#define LED_R 13
#define LED_G 11
#define LED_B 12

/*CONFIGURAÇÕES DO PWW*/
const uint16_t PERIOD = 256;
const float DIVIDER_PWM = 16.0;
const uint16_t LED_STEP = 5;
uint16_t led_level = 0;
uint duty_cycle = 0;

_Bool ligado = 0;
_Bool loopAni = 1;


/*SETUP LEDS GPIO*/
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

/*
SETUP DE PWM
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

/*LIGAR LED */
void controleLedLigar(){
    loopAni = 0;

    pwm_set_gpio_level(LED_R, PERIOD);
    pwm_set_gpio_level(LED_G, PERIOD);
    pwm_set_gpio_level(LED_B, PERIOD);
}

/*DESLIGAR LED */
void controleLedDesligar(){
    loopAni = 0;

    pwm_set_gpio_level(LED_R, 0);
    pwm_set_gpio_level(LED_G, 0);
    pwm_set_gpio_level(LED_B, 0);
}


/*ANIMAÇÃO DE LED */
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

        printf("Led level %d\n", led_level);

        if(loopAni == 0){break;}
        
    }
}

/*LÓGICA PARA CONTROLAR A LUMINOSIDADE DO LED VIA BOTÃO COM USO DO PWM*/
void controleDeIluminacao(uint led_level){
    loopAni = 0;

    pwm_set_gpio_level(LED_R, led_level);
    //pwm_set_gpio_level(LED_G, led_level);
    pwm_set_gpio_level(LED_B, led_level);

    printf("Led level %d\n", led_level);

}