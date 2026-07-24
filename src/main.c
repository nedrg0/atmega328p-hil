#include <util/delay.h>

//#define __AVR_ATmega328__
#include <avr/io.h>

#include "led.h"

int main()
{
    led_init();
    while(1)
    {
        led_toggle();
        _delay_ms(1000);

        led_toggle();
        _delay_ms(1000);
    }
    return 0;
}