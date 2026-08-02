#include <avr/io.h>
#include <uart.h>
#include <ring_buffer.h>
#include <avr/interrupt.h>

#define UART_TX_BUFFER_SIZE (32)
#define UART_RX_BUFFER_SIZE (64)
/*
* Define RX and TX ring buffers.
*/
static char tx_buffer_[UART_TX_BUFFER_SIZE];
static struct ring_buffer tx_buffer = {.buffer = tx_buffer_, .size = sizeof(tx_buffer_)};

static char rx_buffer_[UART_RX_BUFFER_SIZE];
static struct ring_buffer rx_buffer = {.buffer = rx_buffer_, .size = sizeof(rx_buffer_)};

/*
* Inline functions to enable and disable RX and TX interrupts.
* Makes the code more readable.
*/
static inline void uart_disable_tx_interrupt(void) { UCSR0B &= ~(1 << TXCIE0); }
static inline void uart_enable_tx_interrupt(void)  { UCSR0B |= (1 << TXCIE0); }
static inline void uart_disable_rx_interrupt(void) { UCSR0B &= ~(1 << RXCIE0); }
static inline void uart_enable_rx_interrupt(void)  { UCSR0B |= (1 << RXCIE0); }

/*
* Init the USART registers and interrupt 
*/
void uart_init(void)
{
    //Set BAUD rate
    UBRR0H = (uint8_t) (BAUD >> 8);
    UBRR0L = (uint8_t) (BAUD);

    // Enable RX and TX
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);

    //Frame configuration
    UCSR0C = (0 << USBS0) | (3 << UCSZ00); // 8 data bits
    UCSR0A |= (1 << U2X0);   // enable double-speed mode
    
    uart_enable_tx_interrupt();
    uart_enable_rx_interrupt();

}
/*
* ISRs for RX and TX events.
*/
ISR(USART_TX_vect) 
{
    uart_transmit();
}

ISR(USART_RX_vect)
{
    uart_recive();
}

void uart_transmit(void)
{
    if(!ring_buffer_empty(&tx_buffer)){
        uint8_t data = ring_buffer_pop(&tx_buffer);
        UDR0 = data;
    }
}

void uart_recive(void)
{
    // Clear RXC0 flag
    char data = UDR0; 

    if(!ring_buffer_full(&rx_buffer))
    {
        ring_buffer_push(&rx_buffer, &data);
    }
}

void uart_putchar(char c)
{
    while(ring_buffer_full(&tx_buffer));

    uart_disable_tx_interrupt();
    ring_buffer_push(&tx_buffer, &c);
    uart_enable_tx_interrupt();

    if(UCSR0A & (1 << UDRE0))
    {
        uart_transmit();
    }

}

char uart_getchar(void)
{
    while(ring_buffer_empty(&rx_buffer));

    uart_disable_rx_interrupt();
    char c = (char)ring_buffer_pop(&rx_buffer);
    uart_enable_rx_interrupt();

    return c;
}

void uart_putstring(const char* s)
{
    while(*s) uart_putchar(*s++);
}

void uart_putint(const int32_t u)
{
    uart_putchar((char)( u        & 0xFF));  // byte 0 (LSB)
    uart_putchar((char)((u >> 8)  & 0xFF));  // byte 1
    uart_putchar((char)((u >> 16) & 0xFF));  // byte 2
    uart_putchar((char)((u >> 24) & 0xFF));  // byte 3 (MSB)
}

bool uart_available(void)
{
    return !ring_buffer_empty(&rx_buffer);
}



