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
    printf("Temperatura: %.2f\n", tempC);
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