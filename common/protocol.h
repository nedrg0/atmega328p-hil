#ifndef PROTOCOL_H
#define PROTOCOL_H
 
#include <stdint.h>
#include <stdbool.h>
 
#define PROTOCOL_VERSION 1
 
#define FRAME_START_SENSOR (0xA5)   /* HIL -> AVR */
#define FRAME_START_MOTOR   (0x5A)   /* AVR -> HIL */
/*
*   Defines the State struct of the system 
*   For the quadcopter model would be 13 * 4 (Float size) = 52 Bytes + 2 Bytes start frame = 54 Bytes
*   
*/ 
typedef struct __attribute__((packed)) {
    float accel[3];
    float gyro[3];
} State_t;    
/*
*   Defines the motor command struct sent 
*   4 PWM signals to be sent to the SImulator   
*/ 
typedef struct __attribute__((packed)) {
    uint16_t motor_cmd[4];
} MotorCommand_t; /* 8 bytes payload */

#define STATE_FRAME_SIZE (2 + sizeof(State_t) + 1)
#define MOTOR_FRAME_SIZE (2 _ sizeof(MotorCommand_t) + 1)
/* Interface */ 


bool protocol_poll_state(State_t *state);
 
void protocol_send_motor_command(const MotorCommand_t *m_cmd);
 
/* --- Host side --- */
 
void protocol_send_sensor_packet(int fd, const State_t *pkt);
 
bool protocol_poll_motor_cmd(int fd, MotorCommand_t *out);

#endif // PROTOCOL_H
 
