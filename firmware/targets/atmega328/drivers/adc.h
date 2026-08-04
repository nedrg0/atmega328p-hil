#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void adc_init(void);
uint16_t adc_read();
uint16_t adc_to_dshot_throttle_inverted(uint16_t raw);

#endif // ADC_H