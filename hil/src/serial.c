
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "protocol.h"
#include "solver.h"
#include "serial.h" 

int open_serial(const char* dev, speed_t baud)
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

int open_viz_socket(struct sockaddr_in *addr)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(addr, 0, sizeof(addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(VIZ_PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr->sin_addr);
    return fd;
}
void send_viz_packet(int fd, const struct sockaddr_in *addr, const State_raw_t *state_raw, const MotorCommand_t *cmd)
{
    Packet_t pkt;
    pkt.pos[0] = state_raw->x[0];
    pkt.pos[1] = state_raw->x[1];
    pkt.pos[2] = state_raw->x[2];
    pkt.vel[0] = state_raw->x[3];
    pkt.vel[1] = state_raw->x[4];
    pkt.vel[2] = state_raw->x[5];
    pkt.quat[0] = state_raw->x[6];
    pkt.quat[1] = state_raw->x[7];
    pkt.quat[2] = state_raw->x[8];
    pkt.quat[3] = state_raw->x[9];
    pkt.avel[0] = state_raw->x[10];
    pkt.avel[1] = state_raw->x[11];
    pkt.avel[2] = state_raw->x[12];
    pkt.mcmd[0] = cmd->motor_cmd[0];
    pkt.mcmd[1] = cmd->motor_cmd[1];
    pkt.mcmd[2] = cmd->motor_cmd[2];
    pkt.mcmd[3] = cmd->motor_cmd[3];

    sendto(fd, &pkt, sizeof(pkt), MSG_DONTWAIT, (struct sockaddr *)addr, sizeof(*addr));
}