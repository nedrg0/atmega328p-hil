
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>

#include <serial.h>

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