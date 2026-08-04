#include <timer.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

void timer1_init(void)
{
    //CTC mode
    TCCR1B |= (1 << WGM12);

    // 1ms period
    OCR1A = OCR1A_VALUE;

    // Set prescaler to 64
    TCCR1B |= (1 << CS11) | (1 << CS10);

    //Enable compater-match interrupt
    TIMSK1 |= (1 << OCIE1A);

}
void timer2_init(void)
{
    // Set fast PWM mode for timer 2; both A and B 
    TCCR2B |= (1 << WGM20) | (1 << WGM21) ;
    TCCR2A |= (1 << WGM20) | (1 << WGM21) ;

    // Set non-inverting output  for Timer0A and Timer0B
    TCCR2A |= (1 << COM2A1) | (1 << COM2B1);

    // Set prescalar to 8 w/ 16MHz -> f_PWM = up to 7.81 KHz
    TCCR2B |= (1 << CS21);
    TCCR2A |= (1 << CS21);

}
void timer0_init(void)
{

    // Set fast PWM mode for timer 0; both A and B 
    TCCR0B |= (1 << WGM00) | (1 << WGM01) ;
    TCCR0A |= (1 << WGM00) | (1 << WGM01) ;
    
    // Set non-inverting output  for Timer0A and Timer0B
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);

    // Set prescalar to 8 w/ 16MHz -> f_PWM = up to 7.81 KHz
    TCCR0B |= (1 << CS21);
    TCCR0A |= (1 << CS21);

}

/*
 * Set individual pwm outputs
*/
void timer0_set_duty_cycle_A(uint8_t duty_cycle) {OCR0A = duty_cycle ;}
void timer0_set_duty_cycle_B(uint8_t duty_cycle) {OCR0B = duty_cycle ;}
void timer2_set_duty_cycle_A(uint8_t duty_cycle) {OCR2A = duty_cycle ;}
void timer2_set_duty_cycle_B(uint8_t duty_cycle) {OCR2B = duty_cycle ;}