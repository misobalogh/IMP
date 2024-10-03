#ifndef SERIAL_H
#define SERIAL_H

#include <cstddef>  

int serial_open(const char* port_name);
int serial_send(int serial_port, const unsigned char* message, std::size_t size);
void serial_close(int serial_port);

#endif // SERIAL_H
