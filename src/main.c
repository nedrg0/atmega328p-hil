#include <util/delay.h>
#include <avr/interrupt.h>

//#define __AVR_ATmega328__
#include <avr/io.h>

#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "protocol.h"

int main()
{
    sei();
    uart_init();

    State_t state;
    MotorCommand_t m_cmd = {0};
    m_cmd.motor_cmd[0] = 10;
    while(1)
    { 
        if(protocol_poll_state(&state))
        {
            protocol_send_motor_command(&m_cmd);
        }
    }
    return 0;
}