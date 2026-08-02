#include <gpio.h>
#include <avr/io.h>
#include <timer.h>


void io_set_direction(io_e io, io_dir_e dir)
{
    switch(io)
    {
        case IO_ADC_0:
            DDRC |= (dir << 0);
            break;
        case IO_ADC_1:
            DDRC |= (dir << 1);
            break;
        case IO_ADC_2:
            DDRC |= (dir << 2);
            break;
        case IO_ADC_3:
            DDRC |= (dir << 3);
            break;
        case IO_ADC_4:
            DDRC |= (dir << 4);
            break;
        case IO_ADC_5:
            DDRC |= (dir << 5);
            break;
        case IO_LED:
            DDRB |= (dir << 5);
            break;
    }
}
void io_set_out(io_e io, uint8_t value)
{
    switch(io)
    {
        case IO_ADC_0:
        case IO_ADC_1:
        case IO_ADC_2:
        case IO_ADC_3:
        case IO_ADC_4:
        case IO_ADC_5:
        case IO_LED:
            PORTB = (PORTB & ~(1 << 5)) | (value << 5);
    }
}
void io_pwm_init(void)
{
    // Set pins PD6, PB3:1 to output
    DDRD |= (1 << DDD6) | (1 << DDD3) | (1 << DDD5); 
    DDRB |= (1 << DDB3); 

}
