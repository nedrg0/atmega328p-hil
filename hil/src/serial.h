#ifndef SERIAL_H
#define SERIAL_H

#include <termios.h>

int open_serial(const char* dev, speed_t baud);

#endif // SERIAL_H