/**
 * @file
 * Questo è l'header file di operazioni_file.cc
 * Contiene le interfacce delle funzioni utilizzate dal programma per salvare
 * un grafo su file e per caricarlo successivamente.
 */

#include <iostream>
#include <fstream>

using namespace std;

/**
 * @brief Salva un grafo passato per parametro su un file di testo
 * @details Prova ad aprire il file passato per parametro in scrittura e scorre
 * la lista dei nodi e la lista delle adiacenze per scriverle sul file.
 * La prima linea rappresenterà il nodo a cui punta l'arco, mentre quella
 * successiva rappresenterà il peso dell'arco. Il separatore "-----" delimita
 * la fine degli archi per il nodo corrente.
 * 
 * @param G Il grafo da scrivere su file
 * @param nomefile Il nome del file su cui scrivere il grafo
 * 
 * @return TRUE se l'operazione è andata a buon fine, FALSE altrimenti
 */
bool salva_su_file (const grafo &G, const char nomefile[]);

/**
 * @brief Carica un grafo da file sul grafo passato per riferimento
 * @details Prova ad aprire il file passato per parametro in lettura e lo
 * interpreta linea per linea. Il primo valore che deve leggere sarà l'id del
 * nodo a cui deve puntare l'arco che esce dal nodo corrente (partendo da 0),
 * mentre il valore successivo sarà il peso di quell'arco.
 * Continua ad aggiungere archi sul nodo corrente finchè non trova il separatore
 * "-----", quando lo trova passa ad aggiungere archi al nodo successivo.
 * 
 * @param G Il riferimento del grafo da caricare
 * @param nomefile Il nome del file su cui è salvato il grafo
 * 
 * @return TRUE se l'operazione è andata a buon fine, FALSE altrimenti
 */
bool carica_da_file(grafo &G, const char nomefile[]);