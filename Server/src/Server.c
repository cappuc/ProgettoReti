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
		if ((fNClient = fopen(CONFIG_FILENAME, "w+")) != NULL) {
			fWriteInt(fNClient, 1);
		} else {
			printf("[ERRORE] Impossibile creare il file di config\n");
			exit(-1);
		}
	} else {
		char buff[BUFF_SIZE];
		fgets(buff, BUFF_SIZE, (FILE*) fNClient);
		*numClient = strtol(buff, NULL, 10);
	}
	return fNClient;
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
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("[ERRORE] Impossibile creare il socket\n");
		exit(-1);
	}

	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(PORTA_SERVER);

	if (bind(sock, (struct sockaddr*) &sockaddr, sizeof(sockaddr)) < 0) {
		printf("[ERRORE] Impossibile creare il socket\n");
		exit(-1);
	}
	if (listen(sock, N_MAX_CONNESSIONI) < 0) {
		printf("[ERRORE] Impossibile creare il socket\n");
		exit(-1);
	}

	printf("[INFO] Server in ascolto...\n");

	return sock;
}

void ChiudiSocketServer(int sock) {
	close(sock);
}

int accettaConnessioni(int sockServer) {
	return accept(sockServer, (struct sockaddr *) NULL, NULL);
}

void sendStrigToClient(int sock, char* str) {
	int count = 0;
	while (count < sizeof(str)) {
		count += write(sock, str, sizeof(str));
	}
}

void sendIntToClient(int sock, int n) {
	char buff[BUFF_SIZE];
	sprintf(buff, "%d", n);
	sendStrigToClient(sock, buff);
}

int readIntFromClient(int sock) {
	char buff[BUFF_SIZE];
	if (read(sock, buff, sizeof(buff)) <= 0) {
		printf("[ERRORE] Crash client\n");
		exit(-1);
	} else {
		return strtol(buff, NULL, 10);
	}
}

int inizializzazioneConnessioneClient(int sock, int numClient) {
	int idClient;
	FILE *fClient;
	char str[10];

	idClient = readIntFromClient(sock);
	if (idClient == 0) /*Nuovo*/{
		idClient = numClient;
		sprintf(str, "%s%d", CLIENTS_FOLDER, idClient);
		if ((fClient = fopen(str, "w")) == NULL) {
			printf("[ERRORE] Errore apertura file client %d", idClient);
			exit(-1);
		} else {
			fWriteInt(fClient, 0);
			sendIntToClient(sock, idClient);
		}

	} else /*Disconnesso*/{
		sprintf(str, "%s%d", CLIENTS_FOLDER, idClient);

		if ((fClient = fopen(str, "r+")) == NULL) {
			printf("[ERRORE] Errore apertura file client %d", idClient);
			exit(-1);
		} else {
			printf("[INFO] Lettura count da file: %s\n", str);
			fseek(fClient, 0, SEEK_SET);
			fgets(str, 10, fClient);
			printf("[INFO] Count letto da file %s\n", str);
			sendStrigToClient(sock, str);
		}
	}
	fclose(fClient);
	return idClient;
}

void gestioneConnessioneClient(int idClient, int sockClient) {
	FILE *fClient;
	char buff[BUFF_SIZE];
	int lMsg = 0, count = 0;
	char str[10];

	sprintf(str, "%s%d", CLIENTS_FOLDER, idClient);
	if ((fClient = fopen(str, "r+")) == NULL) {
		printf("[ERRORE] Errore apertura file client %d\n", idClient);
		exit(-1);
	}

	do {
		if ((lMsg = read(sockClient, buff, BUFF_SIZE)) <= 0) {
			printf("[ERRORE] Client disconnesso: %d\n", idClient);
			ChiudiSocketServer(sockClient);
			exit(-1);
		} else {
			buff[lMsg] = 0;
			count = strtol(buff, NULL, 10);
			if (count != -1) {
				printf("[INFO] Count ricevuto da client %d: %s\n", idClient, buff);
				fWriteString(fClient, buff);
			} else {
				printf("[INFO] Client %d ha chiuso la connessione\n", idClient);
			}
		}
	} while (count != -1);
	fclose(fClient);
}

int main(void) {

	FILE *fNClient;
	int numClient = 0; //Numero client che hanno richiesto connessione
	int idClient = 0;

	fNClient = aperturaFileNClient(&numClient);

	int sockServer = 0;

	int sockClient = 0;

	sockServer = CreaSocketServer();

	while (1) {
		sockClient = accettaConnessioni(sockServer);
		numClient++;
		fWriteInt(fNClient, numClient);

		switch (fork()) {
		case 0: /*Figlio*/{
			fclose(fNClient);
			idClient = inizializzazioneConnessioneClient(sockClient, numClient);
			printf("[INFO] Client connesso: %d\n", idClient);
			gestioneConnessioneClient(idClient, sockClient);
			exit(1);
			break;
		}
		default: /*Padre*/{

		}
		}
	}
	fclose(fNClient);
	return EXIT_SUCCESS;
}
