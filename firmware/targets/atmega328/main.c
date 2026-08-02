#include <util/delay.h>
#include <avr/interrupt.h>

//#define __AVR_ATmega328__
#include <avr/io.h>

#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "protocol.h"
volatile bool sample = 0;

ISR(TIMER1_COMPA_vect)
{
    sample = true;
}


int main()
{
    // Init. routines
    sei();
    uart_init();
    timer1_init();

    State_t state;
    MotorCommand_t m_cmd = {0};
    m_cmd.motor_cmd[0] = 0;
    m_cmd.motor_cmd[1] =0; 
    m_cmd.motor_cmd[2] =0;
    m_cmd.motor_cmd[3] =0;
    while(1)
    { 
        if(sample)
        {
            if(protocol_poll_state(&state))
            {
                float z = state.accel[2];
                uint16_t cmd_norm = 0;
                if(z <= 9.0) 
                {
                    cmd_norm = 6000;
                } 
                else if (z > 11.0)
                {
                    cmd_norm = 2240;
                }
                
                m_cmd.motor_cmd[0] = cmd_norm;
                m_cmd.motor_cmd[1] = cmd_norm;
                m_cmd.motor_cmd[2] = cmd_norm;
                m_cmd.motor_cmd[3] = cmd_norm;
                protocol_send_motor_command(&m_cmd);
            }
            sample = false;
        }
    }
    return 0;
}