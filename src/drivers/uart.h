#ifndef UART_H
#define UART_H

#ifndef F_CPU
#define F_CPU 16000000UL 
#endif 

#include <defines.h>

#define BAUD_RATE 9600 
#define BAUD (((F_CPU/ (BAUD_RATE * 16UL))) - 1)

/*
* Init the USART registers and interrupt 
*/
void uart_init(void);
/*
* Transmit from the ring buffer
*/
void uart_transmit(void);
/*
* Puts a character in the ring buffer
*/
void uart_putchar(char c);
void uart_putstring(const char* s);



#endif // UART_H