#include <ring_buffer.h>

void ring_buffer_push(volatile struct ring_buffer *rb, const char* data)
{
    rb->buffer[rb->head] =*data;
    rb->head++;

    if (rb->head == rb->size)
    {
        rb->head = 0;
    }
}
uint8_t ring_buffer_pop(volatile struct ring_buffer *rb)
{
    const uint8_t data = rb->buffer[rb->tail];
    rb->tail++;

    if(rb->tail == rb->size)
    {
        rb->tail = 0;
    }
    return data;
}
uint8_t ring_buffer_peek(const volatile struct ring_buffer *rb)
{
    return rb->buffer[rb->tail];
}
bool ring_buffer_empty(const volatile struct ring_buffer *rb)
{
    return rb->tail == rb->head;
}
bool ring_buffer_full(const volatile struct ring_buffer *rb)
{
    uint8_t idx_after_head = rb->head +1;
    if(idx_after_head == rb->size)
    {
        idx_after_head = 0;
    }
    return rb->tail == idx_after_head; 
}
