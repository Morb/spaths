/**
 * @file
 * Header file con le dichiarazioni della struttura dati comune a tutto il 
 * programma.
 * Contiene inoltre le macro per il debug.
 * Il grafo è rappresentato con una lista di nodi, e ogni nodo contiene la
 * propria lista di adiacenze.
 */
#include <iostream>
#include <string.h>

#include <glib.h>
#include <cassert>

#ifdef DEBUG_MODE
 extern unsigned int MASK ;
 /** 
  * Se è definita DEBUG_MODE attiva le macro
  * altrimenti si disattivano tutte le macro per il debug.
  */
 #define DBG(A, B) {if ((A) & MASK) {B; } }
#else
  #define NDEBUG
  #define DBG(A,B)
#endif

#define  D1(a)   DBG(1, a) /**< Macro per il debug, visualizza le azioni 
richieste dall'utente tramite handler */
#define  D2(a)   DBG(2, a) /**< Macro per il debug, visualizza le azioni 
relative alle operazioni sul grafo (creazione, distruzione, ricerca percorsi). */
#define  D4(a)   DBG(4, a) /**< Macro per il debug, visualizza le azioni relative
alle operazioni su file */
#define  D8(a)   DBG(8, a) /**< Macro per il debug visualizza le azioni relative 
al disegno del grafo sulla DrawingArea */

/**
 * @brief Struttura di ogni nodo
 * @details Ogni nodo contiene l'id di riferimento, rappresentato tramite
 * un valore intero, e la propria lista di adiacenze, rappresentata tramite
 * lista doppia, implementata nella libreria GLib. La lista di adiacenze di
 * ogni nodo contiene dei puntatori a dati di tipo ::arco
 */
struct nodo {
	int id; /**< Identificatore del nodo */
	GList *lista_adiacenze; /**< Lista di adiacenze del nodo */
};

/**
 * @brief Struttura di ogni arco
 * @details La struttura di ogni arco contiene semplicemente l'id del nodo a cui
 * fa riferimento l'arco, e il peso di tale arco, rappresentato tramite
 *  valore intero. Da notare che il peso può essere anche negativo.
 */
struct arco {
	int id_nodo_adj; /**< Identificatore del nodo a cui punta l'arco */
	int peso; /**< Peso dell'arco */
};

/**
 * @brief Struttura generale del grafo
 * @details Grazie a questa struttura è possibile rappresentare un grafo
 * diretto e pesato. E' formato da una lista doppia implementata nella libreria
 * GLib, che fa riferimento a valori di tipo ::nodo e dal numero di nodi
 * presenti nel grafo.
 */
struct grafo {
	int num_nodi; /**< Numero di nodi presenti nel grafo */
	GList *lista_nodi; /**< Lista dei nodi presenti nel grafo */
};
