/**
 * @file
 * Questo è l'header file di disegna_grafo.cc
 * Contiene le interfacce delle funzioni utilizzate dal programma per disegnare
 * grafi all'interno di un cairo context.
 */
#include <iostream>
#include <fstream>

using namespace std;
 
 /**
  * @brief Disegna un arco all'interno di un cairo context
  * @details Questa funzione disegna una linea, partendo dai due punti di
  * partenza e arrivando ai due punti di fine, definiti nei parametri con
  * coordinate X e Y. Disegna inoltre un triangolo alla fine della linea,
  * in modo da rappresentare graficamente una freccia.
  * 
  * @param cr Il cairo context su cui disegnare la freccia
  * @param startX Le coordinate X del punto di partenza
  * @param startY Le coordinate Y del punto di partenza
  * @param endX Le coordinate X del punto di arrivo
  * @param endY Le coordinate Y del punto di arrivo
  */
void disegna_arco (cairo_t *cr, guint startX, guint startY, guint endX, 
	guint endY);

/**
 * @brief Disegna un grafo all'interno di un cairo context
 * @details Calcola l'altezza e la larghezza della DrawingArea passata come 
 * parametro per avere il raggio del cerchio inscritto all'interno.
 * Questo cerchio non verrà disegnato ma sarà usato come base per disegnare i
 * nodi in modo che siano equidistanti tra di loro e abbiano una posizione 
 * univoca, facilmente calcolabile mediante sin() e cos().
 * Si disegnano poi i nodi sul perimetro del cerchio calcolato in precedenza e gli 
 * archi, utilizzando la funzione ::disegna_arco . Se il parametro stampa_pesi
 * è TRUE, allora vengono scritti i pesi sopra ogni arco. Per fare in modo che
 * i pesi non si sovrascrivano, il peso viene scritto a 3/4 dell'arco e non a
 * metà. Infine su ogni nodo viene scritto l'id corrispondente.
 * 
 * @param widget Il widget sul quale disegnare il grafo, che sarà di tipo 
 * DrawingArea
 * @param cr Il cairo context su cui disegnare il grafo
 * @param G Il grafo da disegnare
 * @param stampa_pesi Booleano settato a TRUE se devono essere scritti i pesi
 * sugli archi
 */
void disegna_grafo (GtkWidget *widget, cairo_t *cr, const grafo &G,
	bool stampa_pesi);
/**
 * @brief Disegna il percorso più breve per arrivare da un nodo a un altro nodo
 * @details Utilizza la funzione ::bellman_ford per trovare i cammini minimi a
 * partire dal nodo sorgente, specificato come parametro. Dopodichè verrà
 * chiamata la funzione ::disegna_arco per tracciare gli archi, partendo dal
 * nodo di destinazione finchè non si è arrivati dal nodo di partenza.
 * 
 * @param cr Il cairo context su cui tracciare il percorso
 * @param G Il grafo su cui calcolare i cammini minimi
 * @param sorgente Il nodo sorgente
 * @param destinazione Il nodo destinazione
 */
void disegna_percorso(cairo_t *cr, const grafo &G, const int sorgente, 
	const int destinazione);