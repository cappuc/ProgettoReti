#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <errno.h>

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

void fWriteString(FILE *f, char s[]) {
	fseek(f, 0, SEEK_SET);
	fputs(s, f);
	fflush(f);
}

void fWriteInt(FILE *f, int n) {
	char str[10];
	sprintf(str, "%d", n);
	fWriteString(f, str);
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

int inizializzazioneConnessioneClient(int numClient) {
	int idClient;
	FILE *fClient;
	char str[10];
	//TODO:Lettura id Client
	if (idClient == 0) /*Nuovo*/{
		idClient = numClient;
		//TODO:Invia idClient al client

		sprintf(str, "%d", idClient);
		if ((fClient = fopen(str, "w")) == NULL) {
			printf("Errore apertura file client %d", idClient);
			exit(-1);
		} else {
			fWriteInt(fClient,idClient);
			//TODO: inviare sClient al client
			idClient = strtol(str, NULL, 10);
		}

	} else /*Disconnesso*/{
		//TODO:Invia valore a cui era arrivato il client al client
		sprintf(str, "%d", idClient);

		if ((fClient = fopen(str, "r+")) == NULL) {
			printf("Errore apertura file client %d", idClient);
			exit(-1);
		} else {
			fgets(str, 0, fClient);
			//TODO: inviare sClient al client
			idClient = strtol(str, NULL, 10);
		}
	}
	fclose(fClient);
	return idClient;
}

void gestioneConnessioneClient(int idClient, int sockClient) {
	FILE *fClient;
	char buff[BUFF_SIZE];
	int lMsg = 0;
	//TODO: Apertura file fClient
	while (1) {
		//TODO:Lettura messaggio dal client
		if ((lMsg = read(sockClient, buff, BUFF_SIZE)) < 0) {
			//Errore
			printf("Errore: Client ID: %d ErN: %d", idClient, errno);
		} else {
			buff[lMsg] = 0;
			fWriteString(fClient, buff);
		}
	}

}

int main(void) {

	FILE *fNClient;
	int numClient = 0; //Numero client che hanno richiesto connessione
	int idClient = 0;

	fNClient = aperturaFileNClient(&numClient);

	int sockServer = 0;

	int sockClient = 0;

	sockServer = CreaSocketServer();

	while (1) { //TODO:Termina alla terminazione del client associato

		//TODO:Accetta connessioni dai client

		numClient++;
		fWriteInt(fNClient, &numClient);

		switch (fork()) {
		case 0: /*Figlio*/{
			idClient = inizializzazioneConnessioneClient(numClient);
			gestioneConnessioneClient(idClient, sockClient);
			exit(1);		//Comando che non deve essere mai raggiunto
			break;
		}
		default: /*Padre*/{

		}
		}
	}
	fclose(fNClient);
	return EXIT_SUCCESS;
}
