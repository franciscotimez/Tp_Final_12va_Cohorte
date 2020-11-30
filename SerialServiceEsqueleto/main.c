#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "SerialManager.h"

#define STD_OUTPUT 1
#define SIGINT_MSG "\r\nEXIT FOR SIGINT\r\n"
#define CLOSE_SOCKET_MSG "SOCKET CERRADO\r\n"
#define THREAD_CANCEL_MSG "THREAD CANCELADO\r\n"
#define THREAD_JOIN_MSG "THREAD JOIN\r\n"
#define SERIAL_PORT 1
#define BAUDRATE 115200

#define BUFFER_LENGTH 11

char bufferIn[BUFFER_LENGTH];
char bufferOut[BUFFER_LENGTH];

int tec, state;
int ifSend = 0;

pthread_t thread_ws;
int socket_ws;
int ws_fd;
//pthread_mutex_t mutexData = PTHREAD_MUTEX_INITIALIZER;

void *start_thread(void *arg);
void Serial_Service(void);
void sigint_handler(int sig);
void bloquearSign(void);
void desbloquearSign(void);

/*********************************************************************
 * 					MAIN FUNCTION
**********************************************************************/
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

	bloquearSign();
	// LANZO EL TREAD PARA ATENDER EL SOCKET
	int ret;
	ret = pthread_create(&thread_ws,
						 NULL,
						 start_thread,
						 NULL);
	if (ret)
	{
		perror("pthread_fail!");
		exit(1);
	}
	desbloquearSign();

	Serial_Service();

	exit(EXIT_SUCCESS);
	return 0;
}

/*********************************************************************
 * 					Serial_Service FUNCTION
**********************************************************************/
// Recibo ">SW:X,Y\r\n" X = Numero de switch Y = Estado a setear
// Envio ">OUT:X,Y\r\n" X = Numero de switch Y = Estado a setear
void Serial_Service(void)
{
	if (serial_open(SERIAL_PORT, BAUDRATE) != 0)
	{
		printf("No se puede iniciar puerto Serie USB%d!\r\n", SERIAL_PORT);
		exit(1);
	}
	printf("Puerto serie iniciado!\r\n");
	while (1)
	{
		if (serial_receive(bufferIn, BUFFER_LENGTH) > 0)
		{
			if (send(ws_fd, bufferIn, BUFFER_LENGTH, 0) == -1)
			{
				if (close(ws_fd) == 0)
					printf("server: Cliente desconectado!\r\n");
			}
		}
		usleep(10000);
	}
}

/*********************************************************************
 * 					sigint_handler FUNCTION
**********************************************************************/
void sigint_handler(int sig)
{
	write(STD_OUTPUT, SIGINT_MSG, sizeof(SIGINT_MSG));

	// Cierro el socket
	if (close(ws_fd) == 0)
		write(STD_OUTPUT, CLOSE_SOCKET_MSG, sizeof(CLOSE_SOCKET_MSG));

	if (close(socket_ws) == 0)
		write(STD_OUTPUT, CLOSE_SOCKET_MSG, sizeof(CLOSE_SOCKET_MSG));

	if (pthread_cancel(thread_ws) == 0)
		write(STD_OUTPUT, THREAD_CANCEL_MSG, sizeof(THREAD_CANCEL_MSG));

	if (pthread_join(thread_ws, NULL) == 0)
		write(STD_OUTPUT, THREAD_JOIN_MSG, sizeof(THREAD_JOIN_MSG));

	// Cierro puerto serie
	serial_close();
	exit(1);
}

/*********************************************************************
 * 					start_thread FUNCTION
**********************************************************************/
// FUNCION A EJECUTAR EN EL THREAD LANZADO EN MAIN
void *start_thread(void *arg)
{
	socklen_t addr_len;
	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;
	char buffer[BUFFER_LENGTH];

	int nBytesReceive = 0;
	int nBytesSend = 0;

	printf("INICIO THREAD!\r\n");

	// Creamos socket
	socket_ws = socket(AF_INET, SOCK_STREAM, 0);

	// Cargamos datos de IP:PORT del server
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(10000);
	//serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (inet_pton(AF_INET, "127.0.0.1", &(serveraddr.sin_addr)) <= 0)
	{
		fprintf(stderr, "ERROR invalid server IP\r\n");
		exit(1);
	}
	printf("Server: Creado!\r\n");

	// Abrimos puerto con bind()
	if (bind(socket_ws, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
	{
		close(socket_ws);
		perror("listener: bind\r\n");
		exit(1);
	}
	printf("Server: puerto bindeado!\r\n");

	// Seteamos socket en modo Listening
	if (listen(socket_ws, 10) == -1) // backlog=10
	{
		perror("error en listen\r\n");
		exit(1);
	}
	printf("Server: Socket listen!\r\n");

	while (1)
	{
		// Ejecuto accept() para recibir conexiones entrantes
		addr_len = sizeof(struct sockaddr_in);
		if ((ws_fd = accept(socket_ws, (struct sockaddr *)&clientaddr,
							&addr_len)) == -1)
		{
			perror("error en accept\r\n");
			exit(1);
		}
		printf("Server: Socket accept!\r\n");

		char ipClient[32];
		inet_ntop(AF_INET, &(clientaddr.sin_addr), ipClient, sizeof(ipClient));
		printf("server:  conexion desde:  %s\r\n", ipClient);

		nBytesReceive = 1;
		while (nBytesReceive > 0)
		{
			printf("server: escuchando al cliente\r\n");
			nBytesReceive = read(ws_fd, buffer, BUFFER_LENGTH);
			buffer[nBytesReceive] = 0x00;
			printf("Recibi %d bytes.:%s\n", nBytesReceive, buffer);
			serial_send(buffer, nBytesReceive);
		}

		// Cerramos conexion con cliente
		if (close(ws_fd) == 0)
			printf("server: Cliente desconectado!\r\n");
	}

	return 0;
}

/*********************************************************************
 * 					bloquearSign FUNCTION
**********************************************************************/
void bloquearSign(void)
{
	sigset_t set;
	int s;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	//sigaddset(&set, SIGUSR1);
	pthread_sigmask(SIG_BLOCK, &set,
					NULL);
}

/*********************************************************************
 * 					desbloquearSign FUNCTION
**********************************************************************/
void desbloquearSign(void)
{
	sigset_t set;
	int s;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	//sigaddset(&set, SIGUSR1);
	pthread_sigmask(SIG_UNBLOCK, &set,
					NULL);
}