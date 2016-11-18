/*
 * Client
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define BUFF_SIZE 100
#define MAX_SLEEP_TIME 10
#define CONFIG_FILENAME "config"

int main(int argc, char *argv[]) {

	srand(time(NULL));

	int idClient = -1;
	int count = 0;

	idClient = loadIdFromConfig();

	//Creazione connessione con server
	//Lettura file
	if (idClient < 1)/*Nuovo*/{
		//Richiesta id al server (invia 0)
		//idClient = idDatoDalServer
		idClient = richiestaIdAlServer();
		count = 0;
	} else /*Gia avviato*/{
		//Invio id (valoreLetto) al server
		//idClient = idPrecedenteSalvatoNelServer
		//count = valorePrimaDellaDisconnessione
		count = richiestaCountAlServer(idClient);
	}
	/*
	 while (1) {
	 //Invio count al server
	 //Sleep
	 count++;
	 invioCountAlServer(count);
	 sleep(rand() % MAX_SLEEP_TIME);
	 }*/
	return 0;
}

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

int richiestaIdAlServer() {
	//Richiede un nuovo id al server e lo salva nel file config
	return 0;
}

int richiestaCountAlServer(int idClient) {
	//richiede l'ultimo count salvato dal server
	return 0;
}

void invioCountAlServer(int count) {
	//invia count al server
}
