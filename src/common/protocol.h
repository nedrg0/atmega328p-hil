#ifndef PROTOCOL_H
#define PROTOCOL_H
 
#include <stdint.h>
#include <stdbool.h>
 
 
#define FRAME_START_SENSOR 0xA5   /* HIL -> AVR */
#define FRAME_START_MOTOR   0x5A   /* AVR -> HIL */
/*
*   Defines the State struct of the system 
*   For the quadcopter model would be 13 * 4 (Float size) = 52 Bytes + 2 Bytes start frame = 54 Bytes
*   
*/ 
typedef struct __attribute__((packed)) {
    float pos[2];
} State_t;    
/*
*   Defines the motor command struct sent 
*   4 PWM signals to be sent to the SImulator   
*/ 
typedef struct __attribute__((packed)) {
    uint16_t motor_cmd[4];
} MotorCommand_t; /* 8 bytes payload */

 
/*
*   Polls the state from the serial com port 
*   Sends a motor command struct via UART   
*/ 
bool protocol_poll_state(State_t *state);
void protocol_send_motor_command(const MotorCommand_t *m_cmd);

#endif // PROTOCOL_H
 
