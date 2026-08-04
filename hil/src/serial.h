#ifndef SERIAL_H
#define SERIAL_H

#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "protocol.h"
#include "solver.h"

typedef struct 
{
    float pos[3], vel[3], quat[4], avel[3], mcmd[4];
}Packet_t;


#define VIZ_PORT 9000

int open_serial(const char* dev, speed_t baud);
int open_viz_socket(struct sockaddr_in *addr);
void send_viz_packet(int fd, const struct sockaddr_in *addr, const State_raw_t *state_raw, const MotorCommand_t *cmd);

#endif // SERIAL_H