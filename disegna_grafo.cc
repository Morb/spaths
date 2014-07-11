/**
 * @file
 * Questo file contiene le funzioni utilizzate dal programma per disegnare
 * sulla DrawingArea un grafo ed eventualmente un percorso richiesto. Utilizza
 * 5 variabili globali con internal linkage.
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
#include "disegna_grafo.h"

using namespace std;

static guint larghezza_widget; /**< Larghezza del widget, che sarà una
 DrawingArea, sulla quale si andrà a disegnare */
static guint altezza_widget; /**< Altezza del widget, che sarà una
 DrawingArea, sulla quale si andrà a disegnare */
static guint raggio_cerchio; /**< Raggio del cerchio inscritto nella 
DrawingArea */
static const guint raggio_nodo = 24; /**< Raggio predefinito di ogni nodo 
del grafo */
static gfloat step; /**< Ampiezza dell'angolo interno al cerchio diviso per il
numero dei nodi del grafo */

void disegna_arco (cairo_t *cr, guint startX, guint startY, guint endX, 
	guint endY) {

	guint spessore_freccia = 8; /* Spessore predefinito del triangolo
	all'estremità della freccia */
	gdouble angolo_freccia = 0.26; /* Ampiezza Angolo predefinito 
	(in radianti) del triangolo all'estremità della freccia */

	// Disegno la linea
	cairo_new_path(cr);
	cairo_move_to(cr, startX, startY);
	cairo_line_to(cr, endX, endY);
	cairo_stroke(cr);

	// Inizio a calcolare i punti sul piano per la creazione del triangolo
	// alla fine della linea
	gdouble arrowAngle;
	
	if (startX < endX)
		if (startY < endY )
			arrowAngle = atan2(endY - startY, endX - startX);
		else
			arrowAngle = atan2(startY - endY, endX - startX);
	else
		if (startY < endY)
			arrowAngle = atan2(endY - startY, startX - endX);
		else
			arrowAngle = atan2(startY - endY, startX - endX);
	

	gdouble arrowHeight = fabs((spessore_freccia/cos(angolo_freccia)));
	gdouble angle1 = arrowAngle + M_PI + angolo_freccia;
	gdouble angle2 = arrowAngle + M_PI - angolo_freccia;
	guint arrowX1, arrowX2, arrowY1, arrowY2;
	if (startX < endX) {
		arrowX1 = endX + (cos(angle1)*arrowHeight);
		arrowX2 = endX + (cos(angle2)*arrowHeight);
	}
	else {
		arrowX1 = endX - (cos(angle1)*arrowHeight);
		arrowX2 = endX - (cos(angle2)*arrowHeight);
	}
	if (startY < endY) {
		arrowY1 = endY + (sin(angle1)*arrowHeight);
		arrowY2 = endY + (sin(angle2)*arrowHeight);
	}
	else {
		arrowY1 = endY - (sin(angle1)*arrowHeight);
		arrowY2 = endY - (sin(angle2)*arrowHeight);
	}
	
	// Disegno il triangolo alla fine della linea
	cairo_move_to(cr, endX, endY);
	cairo_line_to(cr, arrowX1, arrowY1);
	cairo_line_to(cr, arrowX2, arrowY2);
	cairo_line_to(cr, endX, endY);
	//cairo_fill_preserve(cr);
	cairo_stroke(cr);

	D8(cout << "Punti di creazione freccia: \n")
	D8(cout <<"X1: "<<arrowX1<<" Y1: "<<arrowY1<<endl)
	D8(cout <<"X2: "<<arrowX2<<" Y2: "<<arrowY2<<endl)

}

void disegna_grafo (GtkWidget *widget, cairo_t *cr, const grafo &G,
	bool stampa_pesi) {

	larghezza_widget = gtk_widget_get_allocated_width(widget);
	altezza_widget = gtk_widget_get_allocated_height(widget);
	raggio_cerchio = (min(larghezza_widget, altezza_widget)-60)/2;

	step = (2 * M_PI) / G.num_nodi;
	gfloat angolo = 0;

	D8(cout <<"Larghezza widget: "<<larghezza_widget<<endl)
	D8(cout <<"Altezza widget: "<<altezza_widget<<endl)
	D8(cout <<"Inizio la costruzione dei cerchi rappresentanti i nodi.." << endl)
	D8(cout <<"Posizioni dei cerchi rappresentanti i nodi: " << endl)

	for (int i=0; i < G.num_nodi; i++) {
		guint posX = (larghezza_widget/2) + (raggio_cerchio * sin(angolo));
		guint posY = (altezza_widget/2) + (raggio_cerchio * cos(angolo));
		D8(cout <<posX<<"\t"<<posY<<endl)
		cairo_new_path(cr);
		cairo_save(cr);
		cairo_arc(cr, posX, posY, raggio_nodo, 0, 2 * M_PI);
		cairo_set_source_rgb(cr, 0, 0.7, 0);
		cairo_fill_preserve(cr);
		cairo_set_source_rgb(cr, 0, 0, 0);
		cairo_stroke(cr);
		angolo += step;
	}
	angolo = 0;

	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, 
		CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 10);

	D8(cout <<"Inizio la costruzione degli archi di adiacenza.." << endl)

	for (GList *l = G.lista_nodi; l != NULL; l = l->next) {
		nodo *n = (nodo*)l->data;
		for (GList *k = n->lista_adiacenze; k != NULL; k = k->next) {
			arco *a = (arco*)k->data;
			cairo_set_source_rgb(cr, 0, 0, 0);

			guint startX = (larghezza_widget/2) + 
			((raggio_cerchio - raggio_nodo) * sin(angolo));

			guint startY = (altezza_widget/2) + 
			((raggio_cerchio - raggio_nodo) * cos(angolo));

			guint endX = (larghezza_widget/2) + 
			((raggio_cerchio - raggio_nodo) * sin(step * (a->id_nodo_adj)));

			guint endY = (altezza_widget/2) + 
			((raggio_cerchio - raggio_nodo) * cos(step * (a->id_nodo_adj)));

			disegna_arco(cr, startX, startY, endX, endY);

			guint pos_pesoX;
			guint pos_pesoY;
			char *p, testo[32];

			if (stampa_pesi) {
				if (startX < endX)
					pos_pesoX = (((endX-startX)/4)*3)+startX;
				else
					pos_pesoX = startX-(((startX-endX)/4)*3);

				if (startY < endY)
					pos_pesoY = (((endY-startY)/4)*3)+startY;
				else
					pos_pesoY = startY-(((startY-endY)/4)*3);

				cairo_set_source_rgb(cr, 0, 0, 255);
				cairo_move_to(cr, pos_pesoX, pos_pesoY);
				sprintf( testo, "%d (%d -> %d)", a->peso, n->id, a->id_nodo_adj );
				p = testo;
				cairo_show_text(cr, p);
				D8(cout << "Scrivo il peso " << testo << " su coordinate ("
					<< pos_pesoX <<"," << pos_pesoY << ")" << endl)
			}

			D8(cout <<"Posizioni dell'arco " << n->id << "->" << a->id_nodo_adj
				<<endl)
			D8(cout <<"startX: "<<startX<<" startY: "<<startY<<endl)
			D8(cout <<"endX: "<<endX<<" endY: "<<endY<<endl)

		}
		angolo += step;
	}

	angolo = 0;
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, 
		CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 25);

	D8(cout <<"Inizio a posizionare il testo con l'id di ogni nodo..\n")

	for (int i=0; i < G.num_nodi; i++) {
		cairo_new_path(cr);
		guint posX = (larghezza_widget/2) + (raggio_cerchio * sin(angolo));
		guint posY = (altezza_widget/2) + (raggio_cerchio * cos(angolo));
		char *p, testo[32];
		sprintf( testo, "%d", i );
		p = testo;
		cairo_move_to(cr, posX-8, posY+10);
		cairo_show_text(cr, p);
		angolo += step;
	}
}

void disegna_percorso(cairo_t *cr, const grafo &G, const int sorgente, 
	const int destinazione) {

	assert(sorgente >= 0 && sorgente < G.num_nodi);
	assert(destinazione >= 0 && destinazione < G.num_nodi);
	assert(G.num_nodi > 0);

	D8(cout<<"Inizio a disegnare in rosso il cammino minimo tra il nodo "<<
		sorgente <<" e il nodo "<< destinazione << endl)

	int *pred = NULL;
	int *dist = NULL;
	guint startX, startY, endX, endY;

	// Imposto il colore rosso
	cairo_set_source_rgb(cr, 255, 0, 0);

	bellman_ford(G, sorgente, pred, dist);

	int cammino = destinazione;
	do  {
		startX = (larghezza_widget/2) + 
			((raggio_cerchio - raggio_nodo) * sin(step * pred[cammino]));

		startY = (altezza_widget/2) + 
			((raggio_cerchio - raggio_nodo) * cos(step * pred[cammino]));

		endX = (larghezza_widget/2) + 
			((raggio_cerchio - raggio_nodo) * sin(step * cammino));

		endY = (altezza_widget/2) + 
			((raggio_cerchio - raggio_nodo) * cos(step * cammino));

		D8(cout<< "Disegno l'arco tra il nodo "<< pred[cammino] << " e il nodo "
			<< cammino <<endl)
		
		disegna_arco(cr, startX, startY, endX, endY);

		cammino = pred[cammino];

	} while (cammino != sorgente);

	delete[] pred;
	delete[] dist;
	
}