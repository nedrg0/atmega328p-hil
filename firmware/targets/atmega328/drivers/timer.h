#ifndef TIMER_H
#define TIMER_H
#include <stdbool.h>

#ifndef F_CPU
#define F_CPU 16000000UL 
#endif 

#define TIMER1_PRESCALER   64UL
#define TICK_FREQ_HZ       50UL   // Hz 

#define OCR1A_VALUE  (F_CPU / (TIMER1_PRESCALER * TICK_FREQ_HZ) - 1)


/*
* Init Timer 1 in CTC mode
*/
void timer1_init(void);


#endif // TIMER_H