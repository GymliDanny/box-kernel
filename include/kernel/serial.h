#ifndef SERIAL_H
#define SERIAL_H

int serial_init(void);
char read_serial(void);
void write_serial(char a);
void serial_writestring(const char *str);

#endif
