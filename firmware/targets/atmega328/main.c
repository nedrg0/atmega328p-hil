#include <util/delay.h>
#include <avr/interrupt.h>

//#define __AVR_ATmega328__
#include <avr/io.h>

#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "protocol.h"
volatile bool sample = 0;
volatile uint32_t ticks = 0;
volatile uint8_t cmd = 90;
ISR(TIMER1_COMPA_vect)
{
    sample = true;
    if(ticks++ == 2000) cmd = 74;
}


int main()
{
    // Init. routines
    sei();
    uart_init();
    timer1_init();
    io_pwm_init();
    timer0_init();
    timer2_init();

    timer0_set_duty_cycle_A(100);
    timer0_set_duty_cycle_B(50);
    timer2_set_duty_cycle_A(25);
    timer2_set_duty_cycle_B(0);
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
                uint16_t cmd_norm = cmd;
                
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