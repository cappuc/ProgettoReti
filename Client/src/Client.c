/*
 * Client
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define BUFF_SIZE 100
#define CONFIG_FILENAME "config"

int main(int argc, char *argv[]) {

	int valoreLetto = 0;
	int idClient = -1;
	int count = 0;

	idClient = loadIdFromConfig();

	//Creazione connessione con server
	//Lettura file
	if (valoreLetto == -1)/*Nuovo*/{
		//Richiesta id al server (invia 0)
		//idClient = idDatoDalServer
		count = 0;
	} else /*Gia avviato*/{
		//Invio id (valoreLetto) al server
		//idClient = idPrecedenteSalvatoNelServer
		//count = valorePrimaDellaDisconnessione
	}
	/*
	 while (1) {
	 //Invio count al server
	 //Sleep
	 count++;
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
