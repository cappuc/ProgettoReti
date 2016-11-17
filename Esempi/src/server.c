int main(int argc, char *argv[]) {
	int numClient = 0;
	int idClient = 0;
	//numClient = 0 se primo accesso del server altrimenti leggi da file dove era arrivato
	//Accetta connessioni dai client
	switch (fork()) {
	case 0: /*Figlio*/{
		//Crea connessione con client
		//Invia valori inizializzazione (idClient = numClient)
		numClient++; //Scrittura su file di numClient
		if (idClient == 0) /*Nuovo*/{
			//Invia idClient al client
		} else /*Disconnesso*/{
			//Invia valore a cui era arrivato il client al client
			//In ascolto per valore
			//Scrittura del valore sul file del relativo client
		}
	break;
	}
	default : /*Padre*/{
		//In ascolto per nuove connessioni
	}
	}
	return 0;
}
