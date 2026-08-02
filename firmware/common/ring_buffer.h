#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

struct ring_buffer
{
    char *buffer;
    uint8_t size;
    volatile uint8_t head;
    volatile uint8_t tail;
};
/* 
*   Pushes an element to the ring buffer
*/
void ring_buffer_push(volatile struct ring_buffer *rb, const char* data);
/* 
*   Pops an element from the ring buffer
*/
uint8_t ring_buffer_pop(volatile struct ring_buffer *rb);
/* 
*   Peeks at the last element.
*/
uint8_t ring_buffer_peek(const volatile struct ring_buffer *rb);
/* 
*   Checks if the ring buffer is empty. 
*/
bool ring_buffer_empty(const volatile struct ring_buffer *rb);
/* 
*   Checks if the ring buffer is full. 
*/
bool ring_buffer_full(const volatile struct ring_buffer *rb);


#endif