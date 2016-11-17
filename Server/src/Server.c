#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define CONFIG_FILENAME "config"
#define BUFF_SIZE 255

#define PORTA_SERVER 60000

#define N_MAX_CONNESSIONI 10

FILE * aperturaFileNClient(int *numClient) {
	FILE *fNClient;
	if ((fNClient = fopen(CONFIG_FILENAME, "r+")) == NULL) {
		printf("Errore apertura file numero client");
		exit(-1);
	} else {
		char buff[BUFF_SIZE];
		fgets(buff, BUFF_SIZE, (FILE*) fNClient);
		*numClient = atoi(buff);
		return fNClient;
	}
}

void fWriteInt(FILE *f, int *n) {
	char str[10];
	sprintf(str, "%d", *n);
	fseek(f, 0, SEEK_SET);
	fputs(str, f);
	fflush(f);
}

int CreaSocketServer() {
	int sock;
	struct sockaddr_in sockaddr;

	//Creazione socket
	sock = socket(AF_INET, SOCK_STREAM, 0);

	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(PORTA_SERVER);

	//Il socket deve essere non bloccante
	//errore=fcntl(sock,F_SETFL,O_NONBLOCK);

	bind(sock, (struct sockaddr*) &sockaddr, sizeof(sockaddr));
	listen(sock, N_MAX_CONNESSIONI);

	printf("Server in ascolto...\n");

	return sock;
}

void ChiudiSocketServer(int sock) {
	close(sock);
}

int accettaConnessioni(int sockServer) {
	int sockClient = accept(sockServer, (struct sockaddr *) NULL, NULL);
	return sockClient;
}

int main(void) {

	FILE *fNClient;
	int numClient = 0; //Numero client che hanno richiesto connessione

	fNClient = aperturaFileNClient(&numClient);

	int sockServer = 0;

	sockServer = CreaSocketServer();

	//Accetta connessioni dai client

	switch (fork()) {
	case 0: /*Figlio*/{
		//In ascolto per nuove connessioni
		break;
	}
	default: /*Padre*/{
		//Crea connessione con client
		//Invia valori inizializzazione (idClient = numClient)

		numClient++;
		fWriteInt(fNClient,&numClient);

		//if (idClient == 0) /*Nuovo*/{
		//Invia idClient al client
		//} else /*Disconnesso*/{
		//Invia valore a cui era arrivato il client al client
		//In ascolto per valore
		//Scrittura del valore sul file del relativo client
		//}
		fclose(fNClient);

	}
	}
	return EXIT_SUCCESS;
}
