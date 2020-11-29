
#ifndef SERIAL_MANAGER_INCLUDED
#define SERIAL_MANAGER_INCLUDED

int serial_open(int pn,int baudrate);
void serial_send(char* pData,int size);
void serial_close(void);
int serial_receive(char* buf,int size);

#endif

