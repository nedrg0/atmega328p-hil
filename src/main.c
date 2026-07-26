#include <util/delay.h>
#include <avr/interrupt.h>

//#define __AVR_ATmega328__
#include <avr/io.h>

#include "gpio.h"
#include "uart.h"
#include "timer.h"

volatile bool step_due;

ISR(TIMER1_COMPA_vect)
{
    if(step_due) step_due = false;
    else step_due = true;

}

int main()
{
    io_set_direction(IO_LED, IO_OUTPUT);
    sei();
    uart_init();
    timer1_init();

    while(1)
    {
        if(step_due)
        {
             io_set_out(IO_LED, 1);
        }  
        else
        {
            io_set_out(IO_LED, 0);
        }
        uart_putstring("hello world!\r\n");

    }
    return 0;
}