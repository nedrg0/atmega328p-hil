#include <util/delay.h>

//#define __AVR_ATmega328__
#include <avr/io.h>

#include "gpio.h"

int main()
{
    io_set_direction(IO_LED, IO_OUTPUT);
    while(1)
    {
        io_set_out(IO_LED, 1);
        _delay_ms(200);

        io_set_out(IO_LED, 0);
        _delay_ms(200);
    }
    return 0;
}