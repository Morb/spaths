/**
 * @file
 * Questo file contiene le funzioni utilizzate dal programma per lavorare
 * sul tipo di dato grafo: creazione, distruzione, ricerca dei cammini minimi e
 * stampa delle adiacenze.
 */
#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <fstream>

#include <glib.h>
#include <gtk/gtk.h>
#include "strutt_dati.h"
#include "operazioni_grafo.h"

using namespace std;

grafo genera_grafo_casuale(int nodi, int min_peso, int max_peso) {

	D2(cout << "Inizio creazione grafo casuale.." << endl)

	assert(max_peso >= min_peso);
	assert(nodi > 1);
	
	grafo G;
	srand(time(NULL));
	G.num_nodi = nodi;
	G.lista_nodi = NULL;

	// Inizializzazione dei nodi: vengono allocati num_nodi e vengono
	// inizializzate le loro liste di adiacenza a NULL
	for (int i=0; i<G.num_nodi; i++) {
		nodo *n = new nodo;
		n->id = i;
		n->lista_adiacenze = NULL;
		G.lista_nodi = g_list_append(G.lista_nodi, (void*)n);
	}

	// Creazione della adiacenze: per rendere il grafo connesso si crea sempre
	// un arco che collega il nodo n con il nodo n+1, inoltre vengono aggiunti
	// altri x archi (con x preso a caso fra 0 e num_nodi-1) ad altri nodi
	// scelti casualmente. Per assicurarsi che i nodi selezionati a caso non
	// siano già stati presi, viene allocato un array di booleani (inizializzato
	// tutto a FALSE grazie alle due parentesi tonde) che ha valore TRUE
	// nell'espressione adiacente_a[k] se il nodo k è già presente nella lista
	// di adiacenze.
	for (GList *l = G.lista_nodi; l != NULL; l = l->next) {
		nodo *n = (nodo*)l->data;

		int n_adiacenze = rand() % (nodi-1);

		D2(cout<<"Creazione di "<< n_adiacenze <<" adiacenze per il nodo "<< 
			(n->id) << endl)

		bool *adiacente_a = new bool[nodi]();
		adiacente_a[n->id] = TRUE;

		// L'ultimo nodo non deve avere un arco al nodo n+1
		if ((n->id) < (nodi-1)) {
			adiacente_a[(n->id)+1] = TRUE;
			arco *a = new arco;
			a->peso = rand() % (max_peso - min_peso + 1) + min_peso;
			a->id_nodo_adj = (n->id)+1;
			n->lista_adiacenze = g_list_append(n->lista_adiacenze, (void*)a);
		} 
		
		for (int j=0; j<n_adiacenze; j++) {
			int k = 0;
			do
				k = rand() % nodi;
			while (adiacente_a[k]);

			adiacente_a[k] = TRUE;
			arco *a = new arco;
			a->id_nodo_adj = k;
			a->peso = rand() % (max_peso - min_peso + 1) + min_peso;
			n->lista_adiacenze = g_list_append(n->lista_adiacenze, (void*)a);

			D2(cout<<"\tAdiacenza del nodo "<< (n->id) << " con il nodo " <<
				(a->id_nodo_adj) << " di peso " << (a->peso) << " aggiunta" << 
				endl)
		}
		delete[] adiacente_a;
	}
	return G;
}

void distruggi_grafo(grafo &G) {

	D2(cout << "Inizio eliminazione del grafo.." << endl)

	GList *l = G.lista_nodi;
	while (l != NULL) {
		nodo *n = (nodo*)l->data;
		GList *next = l->next;
		for (GList *k = n->lista_adiacenze; k != NULL; k = k->next) {
			arco *a = (arco*)k->data;

			D2(cout << "Elimino arco " << (n->id) << "->" << (a->id_nodo_adj) <<
				endl)

			delete a;
		}
		GList *k = n->lista_adiacenze;
		g_list_free(k);

		D2(cout << "Elimino nodo " << (n->id) << endl)

		delete n;
		l = next;
	}
	g_list_free(l);
	G.lista_nodi = NULL;
	G.num_nodi = 0;
}

void bellman_ford(const grafo &G, const int sorgente, int *&pred, int *&dist) {
	
	D2(cout << "Inizio calcolo dei cammini minimi..." << endl)

	assert(G.num_nodi > 0);
	assert(sorgente >= 0 && sorgente < G.num_nodi);
	assert(pred == NULL && dist == NULL);

	int max_int = numeric_limits<int>::max();
	pred = new int[G.num_nodi];
	dist = new int[G.num_nodi];

	// Inizializzazione del grafo: imposta tutte le distanze a max_int,
	// eccetto quella del nodo sorgente che sarà impostata a 0.
	// Vengono inoltre impostati tutti i predecessori a -1
	for (GList *l = G.lista_nodi; l != NULL; l = l->next) {
		nodo *n = (nodo*)l->data;
		dist[n->id] = max_int;
		pred[n->id] = -1;
	}
	dist[sorgente] = 0;

	// Rilassamento degli archi per n-1 volte
	for (int i=1; i <= (G.num_nodi-1); i++) {
		for (GList *l = G.lista_nodi; l != NULL; l = l->next) {
			nodo *n = (nodo*)l->data;
			const int u = n->id;
			if (dist[n->id] != max_int) {
				for (GList *k = n->lista_adiacenze; k != NULL; k = k->next) {
					arco *a = (arco*)k->data;
					const int v = a->id_nodo_adj;
					const int w = a->peso;
					if (dist[v] > dist[u] + w) {
						dist[v] = dist[u] + w;
						pred[v] = u;
					}
				}
			}
				 
		}
	}

	// Un ulteriore ciclo di rilassamento degli archi per verificare che
	// non ci siano cicli negativi, nel caso ci siano cicli negativi vengono
	// deallocati gli array con distanze e predecessori e i puntatori vengono
	// settati a NULL
	for (GList *l = G.lista_nodi; l != NULL; l = l->next) {
		nodo *n = (nodo*)l->data;
		const int u = n->id;
		if (dist[n->id] != max_int) {
			for (GList *k = n->lista_adiacenze; k != NULL; k = k->next) {
				arco *a = (arco*)k->data;
				const int v = a->id_nodo_adj;
				const int w = a->peso;
				if (dist[v] > dist[u] + w) {
					delete[] pred;
					delete[] dist;
					pred = NULL;
					dist = NULL;
					D2(cout << "Il grafo contiene un ciclo negativo" << endl)
					return;
				}
			}
		}
	} 
}

void stampa_adiacenze(const grafo &G, ostream &os) {
	D2(cout << "Inizio a stampare il grafo sull'ostream.." << endl)

	for (GList *l = G.lista_nodi; l != NULL; l = l->next) {
		nodo *n = (nodo*)l->data;
		os<< "Adiacenze del nodo " << (n->id) << endl;
		GList *lista_ad = n->lista_adiacenze;
		for (GList *k = lista_ad; k != NULL; k = k->next) {
			arco *a = (arco*)k->data;
			os<< "\t " << (a->id_nodo_adj) << " di peso " << (a->peso) <<endl;
		}		 
	}
}
