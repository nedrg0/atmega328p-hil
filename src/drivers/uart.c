#include <avr/io.h>
#include <uart.h>
#include <ring_buffer.h>
#include <avr/interrupt.h>

#define UART_BUFFER_SIZE (16)
static char buffer[UART_BUFFER_SIZE];
static struct ring_buffer tx_buffer = {.buffer = buffer, .size = sizeof(buffer)};

static inline void uart_disable_interrupt(void)
{
    //Disable USART  TX interrupt
    UCSR0B &= ~(1 << TXCIE0);
}
static inline void uart_enable_interrupt(void)
{
    //Enable USART  TX interrupt
    UCSR0B |= (1 << TXCIE0);
}
void uart_init(void)
{
    //Set BAUD rate
    UBRR0H = (uint8_t) (BAUD >> 8);
    UBRR0L = (uint8_t) (BAUD);

    // Enable RX and TX
    UCSR0B |= (1 << TXEN0);

    //Frame configuration
    UCSR0C = (0 << USBS0) | (3 << UCSZ00); // 8 data bits

    uart_enable_interrupt();

}

ISR(USART_TX_vect, ISR_BLOCK) 
{
    uart_transmit();
}

void uart_transmit(void)
{
    if(!ring_buffer_empty(&tx_buffer)){
        uint8_t data = ring_buffer_pop(&tx_buffer);
        UDR0 = data;
    }
}

void uart_putchar(char c)
{
    while(ring_buffer_full(&tx_buffer));

    uart_disable_interrupt();
    ring_buffer_push(&tx_buffer, &c);
    uart_enable_interrupt();

    if(UCSR0A & (1 << UDRE0))
    {
        uart_transmit();
    }

}

void uart_putstring(const char* s)
{
    while(*s) uart_putchar(*s++);
}

