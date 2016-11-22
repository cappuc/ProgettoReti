/*
 * Client
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>

#define BUFF_SIZE 100
#define MAX_SLEEP_TIME 10
#define CONFIG_FILENAME "config"
#define SERVER_PORT 60000
#define BUFFER_SIZE 20

int loadIdFromConfig() {
	FILE *config;
	char buffer[BUFF_SIZE];
	int id = -1;

	if ((config = fopen(CONFIG_FILENAME, "r")) != NULL) {
		fgets(buffer, BUFF_SIZE, config);
		fclose(config);
		id = strtol(buffer, NULL, 10);
		if (id > 0) {
			printf("[CONFIG] Caricato ID da config: %d\n", id);
			return id;
		} else {
			remove(CONFIG_FILENAME);
		}
	}
	printf("[CONFIG] Nessun file di config trovato\n");
	return -1;
}

void saveIdToConfig(int id) {
	FILE *config;
	char buff[BUFFER_SIZE];

	if ((config = fopen(CONFIG_FILENAME, "w")) != NULL) {
		sprintf(buff, "%d", id);
		fputs(buff, config);
		fclose(config);
	}
}

void sendIntToServer(int sock, int n) {
	char buff[BUFFER_SIZE];
	sprintf(buff, "%d", n);
	if (write(sock, buff, sizeof(buff)) <= 0) {
		printf("Server disconnesso\n");
		exit(-4);
	}
}

int readIntFromServer(int sock) {
	char buff[BUFFER_SIZE];
	int res, n;
	if ((res = read(sock, buff, sizeof(buff))) <= 0) {
		printf("Errore\n");
		return -1;
	} else {
		n = strtol(buff, NULL, 10);
		return n;
	}
}

int requestIdFromServer(int sock) {
	//Richiede un nuovo id al server e lo salva nel file config
	int id = -1;
	sendIntToServer(sock, 0);
	id = readIntFromServer(sock);
	if (id > 0) {
		saveIdToConfig(id);
		return id;
	}
	return -1;
}

int requestCountFromServer(int sock, int idClient) {
	//richiede l'ultimo count salvato dal server
	int count = 0;
	sendIntToServer(sock, idClient);
	if ((count = readIntFromServer(sock)) <= 0) {
		return 0;
	}
	return count;
}

void sendCountToServer(int sock, int count) {
	sendIntToServer(sock, count);
}

int main(int argc, char *argv[]) {

	int sockfd;
	struct hostent *ip;
	struct sockaddr_in serverAddr;

	int idClient = -1;
	int count = 0;

	srand(time(NULL));

	if (argc != 2) {
		printf("Parametri richiesti: IP_SERVER\n");
		exit(-1);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Errore nel socket\n");
		exit(-2);
	}

	ip = gethostbyname(argv[1]);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	bcopy(ip->h_addr, &serverAddr.sin_addr, ip->h_length);

	if (connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
		printf("Impossibile connettersi al server\n");
		exit(-3);
	}

	printf("Connesso al server\n");

	idClient = loadIdFromConfig();

	//Creazione connessione con server
	//Lettura file
	if (idClient < 1)/*Nuovo*/{
		//Richiesta id al server (invia 0)
		//idClient = idDatoDalServer
		idClient = requestIdFromServer(sockfd);
		printf("ID ricevuto dal server: %d\n", idClient);
		count = 0;
	} else /*Gia avviato*/{
		//Invio id (valoreLetto) al server
		//idClient = idPrecedenteSalvatoNelServer
		//count = valorePrimaDellaDisconnessione
		printf("Richiesta count al server\n");
		count = requestCountFromServer(sockfd, idClient);
		printf("Ricevuto count da server: %d\n", count);
	}

	while (1) {
		//Invio count al server
		//Sleep
		count++;
		sendCountToServer(sockfd, count);
		printf("Inviato count: %d\n", count);
		sleep(rand() % MAX_SLEEP_TIME);
	}
	return 0;
}
