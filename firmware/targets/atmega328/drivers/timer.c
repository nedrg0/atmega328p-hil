#include <timer.h>
#include <avr/io.h>
#include <avr/interrupt.h>

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
