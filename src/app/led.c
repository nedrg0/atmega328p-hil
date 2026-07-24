#include "led.h"
#include <avr/io.h>

void led_init()
{
    DDRB |= (1<<PB5);
}

void led_toggle()
{
        PORTB ^= (1 << PB5) ;
}