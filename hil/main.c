#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <stdbool.h>
#include "protocol.h"

#define CT_FODE4_STEP_IMPLEMENTATION
#include "ode_solver.h"

#define LOOP_HZ 1000
#define STEP_NS (1000000000L / LOOP_HZ)
#define TIMESTEP ((1.0 / LOOP_HZ)) 

#define SEND_PACKET_HZ (120) // Freq. of packet sending 
#define N_TICKS (LOOP_HZ / SEND_PACKET_HZ)

// Parameters for simulation 
#define m (0.5)
#define g (9.81)
#define Ix (2.32 * 10e-3)
#define Iy (2.32 * 10e-3)
#define Iz (4.00 * 10e-3)
#define L (0.175)
#define gamma (0.02454)
#define kF (double)(6.11 * 1e-8)
#define kM (double)(1.5 * 1e-9)

typedef struct 
{
    float x[13];
}State_raw_t;

void ode_func(float tn, float* x, float* dx, float* u, uint8_t xdim)
{
    //Torque vectors
    float T, Tx, Ty, Tz;
    T = (float) (kF * (u[0]*u[0] + u[1]*u[1] + u[2]*u[2] + u[3]*u[3] ));
    Tx = (float) (kF * (u[1]*u[1] - u[2]*u[2]));
    Ty = (float) (kF * (u[0]*u[0] - u[3]*u[3]));
    Tz = (float) (kM * (u[0]*u[0]-u[1]*u[1] + u[2]*u[2] - u[3] *u[3]));

    //Position x,y,z
    dx[0] = x[3];
    dx[1] = x[4];
    dx[2] = x[5];

    // Velocities vx, vy, vz
    dx[3] = 2 * T / m * (x[7]*x[9] - x[6] * x[8]);
    dx[4] = 2 * T / m * (x[8]*x[9] + x[6] * x[7]);
    dx[5] = T / m * (x[6]*x[6] - x[7]*x[7] -x[8]*x[8] + x[9]*x[9]) - g;
    // Quaternion
    dx[6] = 0.5 * (-x[10] * x[7] - x[11] * x[8] - x[12] * x[9]);
    dx[7] = 0.5 * (x[10] * x[6] + x[12] * x[8] - x[11] * x[9]);
    dx[8] = 0.5 * (x[11] * x[6] - x[12] * x[7] + x[10] * x[9]);
    dx[9] = 0.5 * (x[12] * x[6] + x[11] * x[7] - x[10] * x[8]);

    //Angular velocities
    dx[10] =  1.0/Ix *(Tx - x[11]*x[12] * (Iz - Iy));
    dx[11] =  1.0/Iy *(Ty - x[10]*x[12] * (Iz - Ix));
    dx[12] =  1.0/Iz *(Tz - x[11]*x[10] * (Iy - Ix));

}

static void solve_step(State_t* state, State_raw_t* state_raw, MotorCommand_t* m_cmd)
{
    float x_[13];
    float dx[13];
    float u[4];

    uint8_t xdim = sizeof(x_)/sizeof(x_[0]);
    for(int i = 0 ; i < xdim; i++) x_[i] = state_raw->x[i];

    u[0] = m_cmd->motor_cmd[0];
    u[1] = m_cmd->motor_cmd[1];
    u[2] = m_cmd->motor_cmd[2];
    u[3] = m_cmd->motor_cmd[3];

    ct_fode4_step(ode_func, 0.0, dx, x_, xdim, u, TIMESTEP);


    for(int i = 0 ; i < xdim; i++) state_raw->x[i] = x_[i];

    state->accel[0] = dx[3]; 
    state->accel[1] = dx[4]; 
    state->accel[2] = dx[5]; 

    state->gyro[0] = dx[10];
    state->gyro[1] = dx[11];
    state->gyro[2] = dx[12];
}

static int open_serial(const char* dev, speed_t baud)
{
    int fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(fd < 0 )
    {
        perror("Failed to open serial.");
        return -1;
    }

    struct termios tty;
    tcgetattr(fd, &tty);
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);
    cfmakeraw(&tty);
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSANOW, &tty);

    return fd;
}

static int try_read_packet(int fd, MotorCommand_t *m_cmd)
{
    static uint8_t buf[sizeof(MotorCommand_t)];
    static ssize_t have = 0;

    struct pollfd pfd = {.fd = fd, .events = POLLIN};
    if (poll(&pfd, 1, 0) <= 0) return 0;

    ssize_t n = read(fd, buf + have, sizeof(buf) - have);
    if (n > 0) have += (ssize_t)n;
    if(have < sizeof(buf)) return 0;

    memcpy(m_cmd, buf, sizeof(MotorCommand_t));
    have = 0;
    return 1;

}
static void add_ns(struct timespec *t, long ns) {
    t->tv_nsec += ns;
    while (t->tv_nsec >= 1000000000L) { t->tv_nsec -= 1000000000L; t->tv_sec++; }
}

int main()
{
    int uart = open_serial("/dev/ttyUSB0", B115200);
    if(uart < 0 ) return 1;

    State_t state = {0};

    State_raw_t state_raw = {0};
    state_raw.x[6] = 1; // Init quaternion

    MotorCommand_t motor_cmd = {0};

    struct timespec deadline, now, start_time;
    clock_gettime(CLOCK_MONOTONIC, &deadline);
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    uint32_t tick_count = 0;
    
    while(1)
    {
        solve_step(&state, &state_raw, &motor_cmd);
        if(tick_count % N_TICKS == 0)
        {
            protocol_send_sensor_packet(uart, &state);
        }
        bool recv_m_cmd = protocol_poll_motor_cmd(uart, &motor_cmd);
        if(tick_count % 50 == 0) 
        {

            printf("ax=%f | ay=%f | az=%f \n", state.accel[0], state.accel[1], state.accel[2]);
            printf("wx=%f | wy=%f | wz=%f \n", state.gyro[0], state.gyro[1], state.gyro[2]);
            printf("x = %f | y = %f | z = %f\n", state_raw.x[0], state_raw.x[1], state_raw.x[2]);
            printf("motor: %d | %d | %d | %d\n", motor_cmd.motor_cmd[0],motor_cmd.motor_cmd[1],motor_cmd.motor_cmd[2],motor_cmd.motor_cmd[3]);

        }
            tick_count++;
        if (tick_count % 1000 == 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &now);
            double elapsed = (now.tv_sec - start_time.tv_sec)
                            + (now.tv_nsec - start_time.tv_nsec) / 1e9;
            printf("1000 iterations in %.4f s (target: 1.0000 s)\n", elapsed);
            start_time = now;  /* reset window so each print covers the next 1000 */
        }  
        add_ns(&deadline, STEP_NS);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
    }

    close(uart);
    return 0;

}
