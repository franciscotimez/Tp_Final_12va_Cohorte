#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "SerialManager.h"

#define STD_OUTPUT 1
#define SIGINT_MSG "\r\nEXIT FOR SIGINT\r\n"
#define SERIAL_PORT 1
#define BAUDRATE 115200

#define BUFFER_LENGTH 11

char buffer[BUFFER_LENGTH];

void sigint_handler(int sig)
{
	write(STD_OUTPUT, SIGINT_MSG, sizeof(SIGINT_MSG));
	serial_close();
	exit(1);
}

int main(void)
{
	printf("Inicio Serial Service\r\n");

	// Struct de las signals
	struct sigaction signal_int;

	// Configuracion SIGINT
	signal_int.sa_handler = sigint_handler;
	signal_int.sa_flags = 0; //SA_RESTART;
	sigemptyset(&signal_int.sa_mask);

	if (sigaction(SIGINT, &signal_int, NULL) == -1)
	{
		perror("Error en sigaction!!");
		exit(1);
	}

	// LANZO EL TREAD PARA ATENDER EL SOCKET

	// Recibo ">SW:X,Y\r\n" X = Numero de switch Y = Estado a setear
	// Envio ">OUT:X,Y\r\n" X = Numero de switch Y = Estado a setear
	while (serial_open(SERIAL_PORT, BAUDRATE) != 0)
	{
		printf("No se puede iniciar puerto Serie USB%d!\r\n", SERIAL_PORT);
		sleep(1);
	}
	printf("Puerto serie iniciado!\r\n");
	while (1)
	{
		int tec, state;
		if (serial_receive(buffer, BUFFER_LENGTH) > 0)
		{
			printf("From CIAA: %s", buffer);
			sscanf(buffer, ">SW:%d,%d", &tec, &state);
			//printf("Send to CIAA: >OUT:%d,%d\r\n", tec, state);
			sprintf(buffer, ">OUT:%d,%d\r\n", tec, state);
			serial_send(buffer, BUFFER_LENGTH);
		}
		usleep(10000);
	}

	exit(EXIT_SUCCESS);
	return 0;
}
