/**
 * @file
 * Questo è l'header file di operazioni_grafo.cc
 * Contiene le interfacce delle funzioni utilizzate dal programma per lavorare
 * su tipi di dato grafo.
 */
#include <iostream>
#include <fstream>

using namespace std;

/**
 * @brief Genera un grafo casuale debolmente connesso
 * @details Alloca il numero di nodi passato come parametro in memoria e li 
 * appende alla lista dei nodi del grafo, assegnando ad ognuno il proprio id ed
 * inizializzando temporaneamente la loro lista di adiacenze a NULL.
 * Dopodichè vengono create le adiacenze: per rendere il grafo connesso è stato
 * deciso che esiste sempre l'arco che collega il nodo N con il nodo N+1.
 * Inoltre ad ogni nodo viene aggiunto un numero casuale di archi che puntano
 * ad altri nodi, scelti sempre casualmente.
 * @see strutt_dati.h per avere più informazioni su come è strutturato il tipo
 * di dato grafo. 
 * 
 * @param nodi Il numero di nodi del grafo da generare
 * @param min_peso Il peso minimo degli archi
 * @param max_peso Il peso massimo degli archi
 * @return Il grafo generato
 */
grafo genera_grafo_casuale(int nodi, int min_peso, int max_peso);

 /**
  * @brief Distrugge il grafo passato per riferimento, deallocando tutta la
  * memoria da lui precedentemente allocata
  * @details Scorre tutta la lista dei nodi del grafo e per ogni nodo scorre la
  * lista di adiacenze, deallocando tutti gli archi e tutti i nodi.
  * Il numero dei nodi del grafo viene poi settato a 0 e il puntatore alla
  * lista dei nodi viene settato a NULL.
  * 
  * @param G Il grafo da eliminare
  */
void distruggi_grafo(grafo &G);

/**
 * @brief Calcola i cammini minimi di un grafo a partire da un determinato nodo,
 * utilizzando l'algoritmo di Bellman-Ford
 * @details Vengono allocati in memoria due array delle dimensioni del numero di
 * nodi del grafo che dovranno contenere (per ogni nodo) la distanza dal nodo
 * sorgente (dist) e l'indice del nodo precedente (pred) per calcolare il cammino minimo.
 * Le distanze vengono inizializzate al massimo per il valore intero, mentre
 * gli indici dei nodi precedenti vengono inizializzati a -1.
 * Si effettua il rilassamento degli archi per il num_nodi-1, ed un ulteriore
 * volta per controllare che non vi siano cicli negativi nel grafo.
 * Se ci sono cicli negativi, gli array pred e dist saranno deallocati e i loro
 * puntatori saranno settati a NULL.
 * Se non esiste un cammino per un ipotetico nodo K, pred[K] sarà -1.
 * 
 * @param G Il grafo su cui calcolare i cammini minimi
 * @param source Il nodo sorgente da cui partire per calcolare i cammini minimi
 * @param pred Il puntatore all'array dei predecessori, deve essere NULL
 * @param dist Il puntatore all'array delle distanze, deve essere NULL
 */
void bellman_ford(const grafo &G, const int source, int *&pred, int *&dist);

/**
 * @brief Stampa le adiacenze del grafo in ingresso su un output stream passato
 * per riferimento
 * @details Scorre tutta la lista dei nodi e per ogni nodo stampa la lista delle
 * adiacenze sull'output stream passato per riferimento.
 * 
 * @param G Il grafo da stampare
 * @param os L'output stream su cui stampare le adiacenze del grafo
 */
void stampa_adiacenze(const grafo &G, ostream &os);