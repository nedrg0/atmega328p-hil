
#include "adc.h"
#include "avr/io.h"
#include <stdint.h>

void adc_init(void)
{
    //Enable ADC
    ADCSRA |= (1 << ADEN);
    // Enable auto trigger; Free running mode is on by default
    ADCSRA |= (1 << ADATE);
    // Set internal voltage reference 
    ADMUX |=  (1 << REFS0) | (1 << REFS1);
    ADCSRA |= (1 << ADSC);
    // Prescaler 128
    ADCSRA |= (1 << ADPS2) | (1<< ADPS1) | (1 << ADPS0);


}

uint16_t adc_read()
{
    ADCSRA |= (1 << ADIF);
    uint16_t ret_val= ADCL ;
    uint16_t ret_val_h = (ADCH << 8);

    return ret_val | ret_val_h;
}

#define MOTOR_CMD_MIN 48
#define MOTOR_CMD_MAX 2047

uint16_t adc_to_dshot_throttle_inverted(uint16_t raw)
{
    return MOTOR_CMD_MIN
         + (uint16_t)(((uint32_t)(1023 - raw) * (MOTOR_CMD_MAX - MOTOR_CMD_MIN)) / 1023UL);
}