#ifndef PROTOCOL_H
#define PROTOCOL_H
 
#include <stdint.h>
#include <stdbool.h>
 
/* Use float (not double) -- avr-gcc's double is 4 bytes, same as float,
 * but don't rely on that matching x86's 8-byte double. float is explicit
 * and identical in size on both ends. */
 
#define FRAME_START_SENSOR 0xA5   /* HIL -> AVR */
#define FRAME_START_MOTOR   0x5A   /* AVR -> HIL */
 
typedef struct __attribute__((packed)) {
    float accel[3];
    float gyro[3];
} State_t;   /* 13 * 4 = 52 bytes payload */
 
typedef struct __attribute__((packed)) {
    uint16_t motor_cmd[4];
} MotorCommand_t; /* 8 bytes payload */

void protocol_send_sensor_packet(int fd, const State_t* pkt)
{
    const uint8_t *bytes = (const uint8_t *)pkt;
    uint8_t checksum = 0;
    uint8_t header[2] = { FRAME_START_SENSOR, (uint8_t)sizeof(State_t) };
 
    for (uint8_t i = 0; i < sizeof(State_t); i++)
    {
        checksum ^= bytes[i];
    }
 
    write(fd, header, sizeof(header));
    write(fd, bytes, sizeof(State_t));
    write(fd, &checksum, 1);
}

typedef enum { WAIT_START, WAIT_LEN, WAIT_PAYLOAD, WAIT_CHECKSUM } RxState;
 
static RxState state = WAIT_START;
static uint8_t payload_buf[sizeof(MotorCommand_t)];
static uint8_t payload_len;
static uint8_t payload_idx;
static uint8_t checksum_accum;
 
bool protocol_poll_motor_cmd(int fd, MotorCommand_t *out)
{
    uint8_t byte;
    ssize_t n;
 
    /* fd is O_NONBLOCK, so read() returns -1/EAGAIN immediately once
     * there's nothing left -- this drains everything currently queued. */
    while ((n = read(fd, &byte, 1)) == 1)
    {
        switch (state)
        {
        case WAIT_START:
            if (byte == FRAME_START_MOTOR)
            {
                state = WAIT_LEN;
            }
            break;
 
        case WAIT_LEN:
            payload_len = byte;
            if (payload_len != sizeof(MotorCommand_t))
            {
                state = WAIT_START;
                break;
            }
            payload_idx = 0;
            checksum_accum = 0;
            state = WAIT_PAYLOAD;
            break;
 
        case WAIT_PAYLOAD:
            payload_buf[payload_idx++] = byte;
            checksum_accum ^= byte;
            if (payload_idx == payload_len)
            {
                state = WAIT_CHECKSUM;
            }
            break;
 
        case WAIT_CHECKSUM:
            state = WAIT_START;
            if (byte == checksum_accum)
            {
                memcpy(out, payload_buf, sizeof(MotorCommand_t));
                return true;
            }
            break;
        }
    }
    return false;
}
#endif // PROTOCOL_H
 
