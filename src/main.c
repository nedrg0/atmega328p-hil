#include <util/delay.h>
#include <avr/interrupt.h>

//#define __AVR_ATmega328__
#include <avr/io.h>

#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "ode_solver.h"

volatile bool step_due;
volatile uint8_t missed_ticks = 0;

ISR(TIMER1_COMPA_vect)
{
    if(step_due)
    {
        missed_ticks++;
    }
    step_due = true;


}
#define QCONST(x) ((q16_16)((x) * 65536.0 + 0.5))   // +0.5 for rounding, all our constants are positive

// Motor parameters 
static const q16_16 R    = QCONST(1.0);      // Ohms
//static const q16_16 L    = QCONST(0.5);      // Henries
static const q16_16 Ke   = QCONST(0.01);     // V/(rad/s), back-EMF constant
static const q16_16 Kt   = QCONST(0.01);     // N*m/A, torque constant
//static const q16_16 J    = QCONST(0.01);     // kg*m^2, rotor inertia
static const q16_16 B    = QCONST(0.001);    // N*m*s, viscous friction
static const q16_16 Vsup = QCONST(12.0);     // Supply voltage

// Precompute reciprocals once 
static const q16_16 inv_L = QCONST(1.0/0.5);    // 1/L = 2.0
static const q16_16 inv_J = QCONST(1.0/0.01);   // 1/J = 100.0

void motor_ode_func(q16_16* x, q16_16* dx)
{
    q16_16 i     = x[0];
    q16_16 omega = x[1];

    // di/dt = (V - R*i - Ke*omega) / L
    q16_16 di = q_sub(q_sub(Vsup, q_mul(R, i)), q_mul(Ke, omega));
    dx[0] = q_mul(di, inv_L);

    // domega/dt = (Kt*i - B*omega) / J
    q16_16 domega = q_sub(q_mul(Kt, i), q_mul(B, omega));
    dx[1] = q_mul(domega, inv_J);
}
int main()
{
    sei();
    uart_init();
    timer1_init();
    q16_16 x[2] = {ftoq(0.0), ftoq(0.0)};
    uint8_t xdim = sizeof(x)/sizeof(x[0]);

    while(1)
    {
        if(step_due)
        {
            step_due = false;
            ct_qode2_step(motor_ode_func, x, xdim, ftoq(0.01));
            uart_putint(x[0]);
            uart_putint(x[1]);
            uart_putchar(missed_ticks);
        }

    }
    return 0;
}