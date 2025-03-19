#include "hardware/adc.h"

#define ADC_GPIO 26
#define ADC_INT 4 // ADC4 = Sensor interno do RP2040

// Função para ler a temperatura ambiente do sensor TMP36 (conectado ao ADC0 - GPIO26)
// --- Função para ler a temperatura do sensor interno do RP2040 ---
float read_internal_temp() {
    adc_select_input(ADC_INT); // ADC4 = Sensor interno do RP2040
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / 4096; // Converte para tensão (0-3.3V)
    return 27.0f - (voltage - 0.706f) / 0.001721f; // Fórmula do datasheet
}



void setupADC(){
    // Inicializa o ADC para leitura da temperatura
    adc_init();
    // Configura GPIO26 (ADC0)
    adc_gpio_init(ADC_INT);
}