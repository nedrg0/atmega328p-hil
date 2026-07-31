#include <string.h>
#include <protocol.h>
#include <uart.h>

/*
 *      Define FSM states for the polling of sensor readings from the Simulator.
 *      Protocol: [START_FRAME_BYTE] [LENGTH] [PAYLOAD] [XOR_CHECKSUM]  
 *      
 *      START_FRAME_BYTE depends on direction, and the entire frame is dropped if the byte doesn't match any direction defined in protocol.h
 *      Also if the checksum is bad, the entire frame is dropped
*/
typedef enum {WAIT_START, WAIT_LEN, WAIT_PAYLOAD, WAIT_CHECKSUM} RxState;

static RxState state = WAIT_START;
static uint8_t payload_buf[sizeof(State_t)];
static uint8_t payload_len, payload_idx, checksum_accum;

bool protocol_poll_state(State_t *s)
{
    while(uart_available())
    {
        uint8_t byte = (uint8_t) uart_getchar();

        switch(state)
        {
            case WAIT_START:
                /*
                 *  Drops the frame if the start bytes don't match 
                */
                if(byte == FRAME_START_SENSOR)
                {
                    state = WAIT_LEN;
                }
                break;
            
            case WAIT_LEN:
                payload_len = byte;
                /*
                 *  Drops the frame if the recived data is not what is expected. 
                */
                if ( payload_len != sizeof(State_t))
                {
                    state = WAIT_START;
                    break;
                }
                
                payload_idx = 0;
                checksum_accum= 0;
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
                /*
                 *  Resets the frame even if the checksum fails.  
                */
                state = WAIT_START;

                if(byte == checksum_accum)
                {
                    memcpy(s, payload_buf, sizeof(State_t));
                    return true;
                }

                break;
        }
    }
    return false;
}


void protocol_send_motor_command(const MotorCommand_t *m_cmd)
{
    const uint8_t *bytes = (const uint8_t *)m_cmd;
    uint8_t checksum =0;

    uart_putchar((char)FRAME_START_MOTOR);
    uart_putchar((char)sizeof(MotorCommand_t));

    for(uint8_t i = 0; i < sizeof(MotorCommand_t); i++)
    {
        checksum ^= bytes[i];
        uart_putchar((char)bytes[i]);
    }

    uart_putchar((char) checksum);
}
