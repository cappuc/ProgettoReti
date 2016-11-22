#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <errno.h>

#define CONFIG_FILENAME "config"
#define CLIENTS_FOLDER "./clients/"
#define BUFF_SIZE 255

#define PORTA_SERVER 60000

#define N_MAX_CONNESSIONI 10

FILE * aperturaFileNClient(int *numClient) {
	FILE *fNClient;
	if ((fNClient = fopen(CONFIG_FILENAME, "r+")) == NULL) {
		fNClient = fopen(CONFIG_FILENAME, "w+");
		fWriteInt(fNClient, 1);
	} else {
		char buff[BUFF_SIZE];
		fgets(buff, BUFF_SIZE, (FILE*) fNClient);
		*numClient = strtol(buff, NULL, 10);
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

	bind(sock, (struct sockaddr*) &sockaddr, sizeof(sockaddr));
	listen(sock, N_MAX_CONNESSIONI);

	printf("Server in ascolto...\n");

	return sock;
}

void ChiudiSocketServer(int sock) {
	close(sock);
}

int accettaConnessioni(int sockServer) {
	return accept(sockServer, (struct sockaddr *) NULL, NULL);
}

void sendStrigToClient(int sock, char* str) {
	write(sock, str, sizeof(str));
}

void sendIntToClient(int sock, int n) {
	char buff[BUFF_SIZE];
	sprintf(buff, "%d", n);
	//write(sock, buff, sizeof(buff));
	sendStrigToClient(sock, buff);
}

int readIntFromClient(int sock) {
	char buff[BUFF_SIZE];
	int res, n;
	if ((res = read(sock, buff, sizeof(buff))) < 0) {
		printf("Errore\n");
		return -1;
	} else {
		n = strtol(buff, NULL, 10);
		return n;
	}
}

int inizializzazioneConnessioneClient(int sock, int numClient) {
	int idClient;
	FILE *fClient;
	char str[10];
	//TODO:Lettura id Client
	idClient = readIntFromClient(sock);
	if (idClient == 0) /*Nuovo*/{
		idClient = numClient;
		sprintf(str, "%s%d", CLIENTS_FOLDER, idClient);
		printf("%s\n", str);
		if ((fClient = fopen(str, "w")) == NULL) {
			printf("Errore apertura file client %d", idClient);
			exit(-1);
		} else {
			fWriteInt(fClient, 0);
			//TODO: inviare idClient(str) al client
			sendIntToClient(sock, idClient);
		}

	} else /*Disconnesso*/{
		//TODO:Invia valore a cui era arrivato il client al client
		sprintf(str, "%s%d", CLIENTS_FOLDER, idClient);

		if ((fClient = fopen(str, "r+")) == NULL) {
			printf("Errore apertura file client %d", idClient);
			exit(-1);
		} else {
			printf("Lettura count da file: %s\n", str);
			fseek(fClient, 0, SEEK_SET);
			fgets(str, 10, fClient);
			printf("Count letto da file %s\n", str);
			//TODO: inviare count(str) al client
			sendStrigToClient(sock, str);
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
	char str[10];
	sprintf(str, "%s%d", CLIENTS_FOLDER, idClient);
	if ((fClient = fopen(str, "r+")) == NULL) {
		printf("Errore apertura file client %d\n", idClient);
		exit(-1);
	}

	while (1) {
		//TODO:Lettura messaggio dal client
		//TODO:Gestire disconnessione
		if ((lMsg = read(sockClient, buff, BUFF_SIZE)) <= 0) {
			//Errore
			printf("Errore: Client ID: %d ErN: %d\n", idClient, errno);
			ChiudiSocketServer(sockClient);
			exit(-1);
		} else {
			buff[lMsg] = 0;
			printf("Count ricevuto: %s\n", buff);
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
		sockClient = accettaConnessioni(sockServer);
		numClient++;
		fWriteInt(fNClient, numClient);

		switch (fork()) {
		case 0: /*Figlio*/{
			idClient = inizializzazioneConnessioneClient(sockClient, numClient);
			printf("Client connesso %d\n", idClient);
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
