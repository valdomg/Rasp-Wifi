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