#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <stdbool.h>
#include <sys/socket.h>

#include "protocol.h"
#include "serial.h"
#include "solver.h"

#define STEP_NS (1000000000L / LOOP_HZ)

#define SEND_PACKET_UART_HZ (120) // Freq. of packet sending 
#define N_TICKS_UART (LOOP_HZ / SEND_PACKET_UART_HZ)

#define SEND_PACKET_VIZ_HZ (500)
#define N_TICKS_VISUAL (LOOP_HZ / SEND_PACKET_VIZ_HZ)



static void add_ns(struct timespec *t, long ns) {
    t->tv_nsec += ns;
    while (t->tv_nsec >= 1000000000L) { t->tv_nsec -= 1000000000L; t->tv_sec++; }
}

int main()
{
    int uart = open_serial("/dev/ttyUSB0", B115200);
    if(uart < 0 ) return 1;

    int viz;
    struct sockaddr_in viz_addr;
    viz = open_viz_socket(&viz_addr);

    State_t state = {0};

    State_raw_t state_raw = {0};
    state_raw.x[6] = 1; // Init quaternion

    MotorCommand_t motor_cmd = {0};

    struct timespec deadline, now, start_time;
    uint32_t tick_count = 0;

    clock_gettime(CLOCK_MONOTONIC, &deadline);
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    while(1)
    {
        solve_step(&state, &state_raw, &motor_cmd);
        if(tick_count % N_TICKS_UART == 0)
        {
            protocol_send_sensor_packet(uart, &state);
        }
        bool recv_m_cmd = protocol_poll_motor_cmd(uart, &motor_cmd);
        if(tick_count % N_TICKS_VISUAL == 0) 
        {
            if(true){
            printf("w1 = %f |w2 = %f |w3 = %f |w4 = %f |\n", state_raw.x[13], state_raw.x[14], state_raw.x[15], state_raw.x[16]);
            printf("ax=%f | ay=%f | az=%f \n", state.accel[0], state.accel[1], state.accel[2]);
            printf("wx=%f | wy=%f | wz=%f \n", state.gyro[0], state.gyro[1], state.gyro[2]);
            printf("x = %f | y = %f | z = %f\n", state_raw.x[0], state_raw.x[1], state_raw.x[2]);
            printf("vx = %f | vy = %f | vz = %f\n", state_raw.x[3], state_raw.x[4], state_raw.x[5]);
            printf("motor: %d | %d | %d | %d\n", motor_cmd.motor_cmd[0],motor_cmd.motor_cmd[1],motor_cmd.motor_cmd[2],motor_cmd.motor_cmd[3]);
            }
            send_viz_packet(viz, &viz_addr, &state_raw, &motor_cmd);

        }
            tick_count++;
         
        if (tick_count % 1000 == 0 && false)
        {
            clock_gettime(CLOCK_MONOTONIC, &now);
            double elapsed = (now.tv_sec - start_time.tv_sec)
                            + (now.tv_nsec - start_time.tv_nsec) / 1e9;
            printf("1000 iterations in %.4f s (target: %f s)\n", elapsed, (1000.0/ (double)LOOP_HZ));
            start_time = now;  /* reset window so each print covers the next 1000 */
        }
         
        add_ns(&deadline, STEP_NS);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
    }

    close(uart);
    return 0;

}
