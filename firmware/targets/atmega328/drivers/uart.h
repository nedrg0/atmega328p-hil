#ifndef UART_H
#define UART_H

#ifndef F_CPU
#define F_CPU 16000000UL 
#endif 

#include <stdbool.h>

#define BAUD_RATE 115200
#define BAUD (((F_CPU/ (BAUD_RATE * 8UL))) - 1)

/*
* Init the USART registers and interrupt 
*/
void uart_init(void);
/*
* Transmit from the ring buffer.
* Read from ring buffer.
*/
void uart_transmit(void);
void uart_recive(void);
/*
* Puts a character in the ring buffer
*/
void uart_putchar(char c);
void uart_putstring(const char* s);
void uart_putint(const int32_t u);

char uart_getchar(void);
 
bool uart_available(void);


#endif // UART_H