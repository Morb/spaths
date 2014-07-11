/**
 * @file
 * Questo file contiene le funzioni utilizzate dal programma per salvare un
 * grafo su file e caricarlo successivamente.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cassert>

#include <glib.h>
#include "strutt_dati.h"

using namespace std;

bool salva_su_file (const grafo &G, const char nomefile[]) {

	assert(G.num_nodi > 0);
	assert(nomefile != NULL);

	D4(cout << "Inizio salvataggio su file.." << endl)

	ofstream file(nomefile);
	if (file.is_open()) {
		for (GList *l = G.lista_nodi; l != NULL; l = l->next) {
			nodo *n = (nodo*)l->data;
			for (GList *k = n->lista_adiacenze; k != NULL; k = k->next) {
				arco *a = (arco*)k->data;
				file << (a->id_nodo_adj) << endl;
				file << (a->peso) << endl;

				D4(cout << "Scrivo su file l'arco " << (n->id) << "->" << 
					(a->id_nodo_adj) << endl)
			}
			// Se aggiungo il separatore anche all'ultimo nodo, la funzione
			// carica_da_file aggiungerebbe un altro nodo che in realtà
			// nel grafo non esiste
			if (l->next != NULL)
				file << "-----" << endl;
		}
		file.close();
		return TRUE;
	}
	else {
		file.close();
		return FALSE;
	}
		
}

bool carica_da_file(grafo &G, const char nomefile[]) {

	assert(nomefile != NULL);
	
	D4(cout << "Inizio il caricamento da file.." << endl)

	string linea;
	ifstream file(nomefile);
	int num_nodi = 0;
	int id_nodo_adj = -1;
	int peso = 0;

	if (file.is_open()) {

		// Aggiungo il nodo 0 che deve essere sempre presente
		nodo *n = new nodo;
		n->id = 0;
		n->lista_adiacenze = NULL;
		G.lista_nodi = g_list_append(G.lista_nodi, (void*)n);

		D4(cout << "Aggiungo il nodo " << num_nodi << endl)

		while (getline(file, linea)) {

			// Se non trovo il separatore, significa che devo continuare ad
			// aggiungere archi al nodo corrente
			if (linea.compare("-----") != 0) {

				// Se non è stato settato id_nodo_adj a un valore positivo, 
				// significa che questa linea rappresenta proprio quel valore 
				// (posso farlo perchè id_nodo_adj può essere solo >= 0)
				if (id_nodo_adj == -1)
					id_nodo_adj = atoi(linea.c_str());
				
				// Se invece ho già il valore di id_nodo_adj, il valore di
				// questa linea rappresenta il peso, ed ho i 2 valori necessari
				// per creare un arco ed aggiungerlo alla lista delle adiacenze
				// del nodo corrente
				else {
					peso = atoi(linea.c_str());
					arco *a = new arco;
					a->peso = peso;
					a->id_nodo_adj = id_nodo_adj;
					
					n->lista_adiacenze = g_list_append(n->lista_adiacenze, (void*)a);

					D4(cout << "Carico l'arco " << (n->id) << "->" << 
						(a->id_nodo_adj) << " sul nodo " << num_nodi << endl)
					// Reimposto id_nodo_adj a -1 perchè mi aspetto che il
					// prossimo valore sarà id_nodo_adj
					id_nodo_adj = -1;

				}

			}
			// Se trovo il separatore, significa che gli archi del nodo corrente
			// sono finiti, quindi devo far puntare n ad un nuovo nodo,
			// aumentare la lunghezza del grafo ed aggiungere il nuovo n alla
			// lista dei nodi
			else {
				num_nodi++;
				n = new nodo;
				n->id = num_nodi;
				n->lista_adiacenze = NULL;
				G.lista_nodi = g_list_append(G.lista_nodi, (void*)n);

				D4(cout << "Aggiungo il nodo " << num_nodi << endl)

			}
		}
		G.num_nodi = num_nodi + 1;
		file.close();
		return TRUE;

	}

	else {
		file.close();
		return FALSE;
	}

}