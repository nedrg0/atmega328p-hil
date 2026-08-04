#include <util/delay.h>
#include <avr/interrupt.h>

//#define __AVR_ATmega328__
#include <avr/io.h>

#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "protocol.h"
#include "adc.h"
volatile bool sample = 0;
volatile uint32_t ticks = 0;
volatile uint16_t cmd = 0;
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
    io_pwm_init();
    timer0_init();
    timer2_init();
    adc_init();

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
                cmd = adc_read();
                uint16_t scaled_cmd = adc_to_dshot_throttle_inverted(cmd);
                
                m_cmd.motor_cmd[0] =scaled_cmd;
                m_cmd.motor_cmd[1] =scaled_cmd;
                m_cmd.motor_cmd[2] =scaled_cmd;
                m_cmd.motor_cmd[3] =scaled_cmd;

                protocol_send_motor_command(&m_cmd);

                timer0_set_duty_cycle_A(m_cmd.motor_cmd[0]);
                timer0_set_duty_cycle_B(m_cmd.motor_cmd[1]);
                timer2_set_duty_cycle_A(m_cmd.motor_cmd[2]);
                timer2_set_duty_cycle_B(m_cmd.motor_cmd[3]);

            }
            sample = false;
        }
    }
    return 0;
}