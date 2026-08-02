#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef enum
{
    IO_ADC_0,   // PC0
    IO_ADC_1,   //PC1
    IO_ADC_2,   //PC2
    IO_ADC_3,   //PC3
    IO_ADC_4,   //PC4
    IO_ADC_5,   //PC5
    IO_LED,        // PB5
} io_e;

typedef enum
{
    IO_INPUT,
    IO_OUTPUT,
}io_dir_e;

void io_set_direction(io_e io, io_dir_e dir);
void io_set_out(io_e io, uint8_t value);
void io_pwm_init(void);


#endif // GPIO_H