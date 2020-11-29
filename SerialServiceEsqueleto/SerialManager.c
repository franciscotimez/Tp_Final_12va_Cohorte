#include "rs232.h"
#include "SerialManager.h"

static int portNumber;

int serial_open(int pn,int baudrate)
{
	portNumber = pn;
	return OpenComport(portNumber,baudrate);
}


void serial_send(char* pData,int size)
{
	SendBuf(portNumber, (unsigned char*)pData, size);
}

void serial_close(void)
{
	CloseComport(portNumber);
}

int serial_receive(char* buf,int size)
{
	return PollComport(portNumber, (unsigned char*) buf,size);
}

