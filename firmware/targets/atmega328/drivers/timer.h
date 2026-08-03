#ifndef TIMER_H
#define TIMER_H
#include <stdbool.h>
#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL 
#endif 

#define TIMER1_PRESCALER   64UL
#define TICK_FREQ_HZ       500UL   // Hz 

#define OCR1A_VALUE  (F_CPU / (TIMER1_PRESCALER * TICK_FREQ_HZ) - 1)


/*
 * Init Timer 1 in CTC mode
 * Init TImer 2 in Fast PWM mode
 * Init Timer 0 in Fast PWM mode 
*/
void timer2_init(void);
void timer1_init(void);
void timer0_init(void);


void timer0_set_duty_cycle_A(uint8_t duty_cycle);
void timer0_set_duty_cycle_B(uint8_t duty_cycle);
void timer2_set_duty_cycle_A(uint8_t duty_cycle);
void timer2_set_duty_cycle_B(uint8_t duty_cycle);
#endif // TIMER_H