/**
 * @mainpage SPaths
 * Questo programma genera grafi casuali orientati debolmente connessi, li 
 * disegna tramite interfaccia grafica e visualizza il percorso più breve tra 2 
 * nodi di esso, se esiste.\n
 * 
 * Si invoca da linea di comando senza alcun parametro addizionale:\n
 * - \c ./SPaths
 *  
 *  Il grafo orientato è implementato mediante liste di adiacenza.\n
 *  La ricerca del percorso più breve è effettuata con l'algoritmo di
 *  Bellman-Ford (vedi ::bellman_ford).
 *  @par Moduli
 *  - \b SPaths.cc: \n contiene la funzione main, necessaria per l'avvio del
 *  programma e gli handler per l'interfaccia grafica. \n Utilizza
 *  \c strutt_dati.h, \c operazioni_grafo.h, \c disegna_grafo.h, 
 *  \c operazioni_file.h
 *  - \b strutt_dati.h: \n contiene le strutture dati utilizzate da tutti i
 *  moduli.
 *  - \b operazioni_grafo.cc: \n contiene le funzioni utilizzate dal programma
 *  per lavorare su tipi di dato grafo. \n Utilizza \c strutt_dati.h e
 *  \c operazioni_grafo.h
 *  - \b disegna_grafo.cc: \n contiene le funzioni utilizzate dall'interfaccia
 *  grafica per disegnare il grafo ed i percorsi minimi. \n Utilizza 
 *  \c strutt_dati.h, \c disegna_grafo.h e \c operazioni_grafo.h
 *  - \b operazioni_file.cc: \n contiene le funzioni utilizzate per salvare
 *  il grafo corrente su file e per caricarlo successivamente. \n Utilizza
 *  \c strutt_dati.h
 *  
 *  @see strutt_dati.h Per vedere come è stato implementato il grafo
 *  @author Federico Terzi
 *  @version 1.0
 */

 /**
  * @file
  * Il file principale contenente la funzione ::main .
  * Il file utilizza 6 variabili globali con internal linkage, che contengono
  * informazioni sul grafo, sul builder utilizzato dall'interfaccia grafica e
  * sul percorso da visualizzare.
  * Inoltre sono presenti tutti gli handler necessari all'interfaccia grafica.
  */
#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <limits>
#include <sstream>

#include <glib.h>
#include <gtk/gtk.h>
#include "strutt_dati.h"
#include "operazioni_grafo.h"
#include "disegna_grafo.h"
#include "operazioni_file.h"

using namespace std;

#ifdef DEBUG_MODE

unsigned int MASK = 0;

#endif

static grafo G;
static bool stampa_pesi = TRUE;
static bool dis_percorso = FALSE;
static int sorg_percorso = -1;
static int dest_percorso = -1;
static GtkBuilder *builder;

/**
 * @brief Visualizza una dialog di errore
 * @details Utilizzando i due array di char in ingresso, mostra una finestra
 * di errore.
 * 
 * @param str_princ Stringa principale della dialog
 * @param str_secon Stringa secondaria della dialog
 */
static void mostra_error_alert (const char *str_princ, const char *str_secon) {
    GtkLabel *label_principale = 
	GTK_LABEL(gtk_builder_get_object(builder, "label_principale")) ;

    char markup_str[100] = "<span weight=\"bold\" size=\"larger\">" ;
    strcat(markup_str, str_princ) ;
    strcat(markup_str, "</span>") ;
    gtk_label_set_markup(label_principale, markup_str);

    GtkLabel *label_secondaria = 
	GTK_LABEL(gtk_builder_get_object(builder, "label_secondaria")) ;
    gtk_label_set_text(label_secondaria, str_secon);

    GtkWidget *error_alert = 
	GTK_WIDGET(gtk_builder_get_object(builder, "error_alert")) ;
    gtk_widget_show_all(error_alert) ;
}

/**
 * @brief Aggiorna la barra di stato dell'interfaccia grafica
 * @details Controlla se è stato inizializzato un grafo, controllandone il
 * numero dei nodi e aggiorna la label sinistra della barra di stato di 
 * conseguenza. Inoltre controlla se è stato richiesto di disegnare un percorso 
 * e aggiorna la label destra della barra di stato di conseguenza.
 */
static void aggiorna_barra_stato() {
    GtkLabel *label_grafo = 
    GTK_LABEL(gtk_builder_get_object(builder, "label_grafo"));
    GtkLabel *label_percorso = 
    GTK_LABEL(gtk_builder_get_object(builder, "label_percorso"));

    char testo_grafo[100];
    sprintf(testo_grafo, "Grafo di %d nodi visualizzato", G.num_nodi);
    gtk_label_set_text(label_grafo, G.num_nodi ? testo_grafo : 
        "Nessun grafo caricato");

    char testo_percorso[100];
    sprintf(testo_percorso, "Percorso dal nodo %d al nodo %d visualizzato", 
        sorg_percorso, dest_percorso);
    gtk_label_set_text(label_percorso, dis_percorso ? testo_percorso : 
        "Nessun percorso selezionato");
}

/**
 * @brief Aggiorna la textview contenente le adiacenze del grafo
 * @details Utilizza la funzione stampa_adiacenze per stampare le adiacenze
 * del grafo su un ostringstream, dopodichè inserisce il risultato nella
 * textview.
 */
static void aggiorna_text_view_grafo() {

    GtkTextView *textview_grafo =
    GTK_TEXT_VIEW(gtk_builder_get_object(builder, "textview1"));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(textview_grafo);

    ostringstream os;
    stampa_adiacenze(G, os);

    gtk_text_buffer_set_text(buffer, os.str().c_str(), -1);

}

/**
 * @brief Handler utilizzato per chiudere le dialog a partire dai segnali
 * dei bottoni
 * @details Viene chiuso il top level del GtkButton passato per parametro.
 * 
 * @param button Il GtkButton che richiede la chiusura della propria finestra
 */
extern "C" void hide_main_window(GtkButton *button, gpointer user_data) {
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

/**
 * @brief Handler utilizzato per chiudere il programma.
 * 
 */
extern "C" gboolean handler_delete_event(GtkWidget *widget, GdkEvent  *event, 
    gpointer user_data) {

    D1(cout << "Richiesta chiusura del programma..." << endl)

    distruggi_grafo(G);
    gtk_main_quit();
    return TRUE;
}

/**
 * @brief Handler utilizzato dal dialog per impedire di distruggersi tramite
 * pressione del tasto Esc
 */
extern "C" gboolean handler_dialog_delete_event(GtkWidget *widget, 
    GdkEvent  *event, gpointer user_data) {

    D1(cout << "Impedisco la distruzione della dialog..." << endl)

    gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(widget)));
    
    return TRUE;
}

/**
 * @brief Handler utilizzato per caricare il grafo dal file grafo.txt
 * @details Se è stato già creato un grafo, viene distrutto. Viene poi caricato
 * il grafo con la funzione ::carica_da_file e vengono aggiornate la DrawingArea,
 * la barra di stato e la textview con le informazioni sul grafo.
 * Se non è possibile aprire il file in lettura, viene mostrata una dialog di
 * errore.
 */
extern "C" void load_handler(GtkWidget *widget, gpointer data) {
    if (G.num_nodi != 0)
        distruggi_grafo(G);

    D1(cout << "Richiesto caricamento da file grafo.txt..." << endl)

    if (!carica_da_file(G, "grafo.txt"))
        mostra_error_alert("Caricamento del grafo fallito",
            "Non è stato possibile aprire il file grafo.txt per la lettura");

    GtkWidget *drawingarea = GTK_WIDGET(
        gtk_builder_get_object(builder, "drawingarea1"));

    gtk_widget_queue_draw(drawingarea);

    aggiorna_barra_stato();
    aggiorna_text_view_grafo();
}

/**
 * @brief Handler utilizzato per salvare il grafo corrente sul file grafo.txt
 * @details Se il grafo corrente è stato inizializzato (e quindi G.num_nodi > 0)
 * viene salvato su file grazie alla funzione ::salva_su_file.
 * Se non è possibile aprire il file in scrittura o se il grafo non è stato
 * creato, viene mostrata una dialog di errore.
 */
extern "C" void save_handler(GtkWidget *widget, gpointer data) {
    if (G.num_nodi == 0) {
        mostra_error_alert("Salvataggio del grafo fallito", 
            "Non è stato generato nessun grafo.");
        return;
    }

    D1(cout << "Richiesto salvataggio su file grafo.txt..." << endl)

    if (!salva_su_file(G, "grafo.txt"))
        mostra_error_alert("Salvataggio del grafo fallito",
            "Non è stato possibile aprire il file grafo.txt per la scrittura");
}

/**
 * @brief Handler utilizzato dalla dialog1 per creare un grafo
 * @details Utilizza i valori degli spinbutton e la funzione 
 * ::genera_grafo_casuale per generare un grafo casuale, resetta
 * le variabili globali relative al percorso e richiede l'aggiornamento della
 * DrawingArea, della barra di stato e della textview contenente le adiacenze
 * del grafo.
 */
extern "C" void crea_grafo_handler(GtkWidget *widget, gpointer data) {

    GtkSpinButton *spinbutton_nodi = GTK_SPIN_BUTTON(
    	gtk_builder_get_object(builder, "spinbutton_nodi"));
    int num_nodi = gtk_spin_button_get_value_as_int(spinbutton_nodi);

    GtkSpinButton *spinbutton_minpeso = GTK_SPIN_BUTTON(
    	gtk_builder_get_object(builder, "spinbutton_minpeso"));
    int minpeso = gtk_spin_button_get_value_as_int(spinbutton_minpeso);

    GtkSpinButton *spinbutton_maxpeso = GTK_SPIN_BUTTON(
    	gtk_builder_get_object(builder, "spinbutton_maxpeso"));
    int maxpeso = gtk_spin_button_get_value_as_int(spinbutton_maxpeso);

    if (minpeso > maxpeso) {
    	mostra_error_alert("Creazione nuovo grafo fallita", 
    		"Il peso minimo degli archi deve essere minore del peso massimo");
    	return;
    }

    if (G.num_nodi != 0)
        distruggi_grafo(G);

    dis_percorso = FALSE;
    sorg_percorso = -1;
    dest_percorso = -1;

    D1(cout << "Richiesta creazione di un nuovo grafo di " << num_nodi << 
        " nodi..." << endl)

    G = genera_grafo_casuale(num_nodi, minpeso, maxpeso);

	stampa_pesi = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
		gtk_builder_get_object(builder, "checkbutton1")));

    GtkWidget *drawingarea = GTK_WIDGET(
		gtk_builder_get_object(builder, "drawingarea1"));

    gtk_widget_queue_draw(drawingarea);

    aggiorna_barra_stato();
    aggiorna_text_view_grafo();
    
    gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(widget)));

}

/**
 * @brief Disegna il percorso richiesto dalla dialog2
 * @details Prende in ingresso i nodi di partenza e di arrivo grazie agli
 * SpinButton della dialog2, e utilizza la funzione ::bellman_ford per
 * controllare che esista un percorso tra i due nodi selezionati e che non
 * vi sia un ciclo negativo nel grafo. Successivamente chiede un refresh
 * alla DrawingArea, e sarà il ::draw_handler che si occuperà di disegnare il 
 * percorso.
 */
extern "C" void disegna_percorso_handler(GtkWidget *widget, gpointer data) {

    if (G.num_nodi == 0) {
    	mostra_error_alert("Impossibile trovare il percorso", 
    		"Non è stato generato nessun grafo.");
    	return;
    }

    GtkSpinButton *spinbutton_partenza = GTK_SPIN_BUTTON(
    	gtk_builder_get_object(builder, "spinb_partenza"));
    int partenza = gtk_spin_button_get_value_as_int(spinbutton_partenza);

    GtkSpinButton *spinbutton_arrivo = GTK_SPIN_BUTTON(
    	gtk_builder_get_object(builder, "spinb_arrivo"));
    int arrivo = gtk_spin_button_get_value_as_int(spinbutton_arrivo);

    if (partenza >= G.num_nodi) {
    	mostra_error_alert("Impossibile trovare il percorso", 
    		"Il nodo di partenza non esiste.");
    	return;
    }

    if (arrivo >= G.num_nodi) {
    	mostra_error_alert("Impossibile trovare il percorso", 
    		"Il nodo di arrivo non esiste.");
    	return;
    }

    D1(cout << "Richiesto percorso da " << partenza << " a " << arrivo << endl)

    int *pred = NULL;
    int *dist = NULL;
    bellman_ford(G, partenza, pred, dist);
    if ((pred == NULL) && (dist == NULL)) {
    	mostra_error_alert("Impossibile trovare il percorso", 
    		"Il grafo contiene un ciclo negativo.");
    	delete[] pred;
    	delete[] dist;
    	return;
    }

    if (pred[arrivo] == -1) {
    	mostra_error_alert("Impossibile trovare il percorso",
    		"Non esiste un percorso fra i due nodi scelti.");
    	delete[] pred;
    	delete[] dist;
    	return;
    }

    delete[] pred;
    delete[] dist;

    sorg_percorso = partenza;
    dest_percorso = arrivo;
    dis_percorso = TRUE;

    GtkWidget *drawingarea = GTK_WIDGET(
		gtk_builder_get_object(builder, "drawingarea1"));

    gtk_widget_queue_draw(drawingarea);

    aggiorna_barra_stato();

    gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(widget)));

}

/**
 * @brief Handler che si occupa di aprire la dialog1 per la creazione di un
 * nuovo grafo.
 */
extern "C" void nuovo_grafo_handler (GtkMenuItem *menuitem, 
	gpointer user_data) {
	GtkWidget *nuovo_grafo_dialog = GTK_WIDGET(
		gtk_builder_get_object(builder, "dialog1"));
	gtk_widget_show_all(nuovo_grafo_dialog);
}

/**
 * @brief Handler che elimina il grafo tramite la funzione ::distruggi_grafo e 
 * resetta le informazioni sul percorso. Inoltre aggiorna la barra di stato e
 * la textview con le adiacenze del grafo.
 */
extern "C" void elimina_grafo_handler (GtkMenuItem *menuitem, 
	gpointer user_data) {

    D1(cout << "Richiesta eliminazione del grafo corrente..." << endl)

    dis_percorso = FALSE;
    sorg_percorso = -1;
    dest_percorso = -1;

	GtkWidget *drawingarea = GTK_WIDGET(
	gtk_builder_get_object(builder, "drawingarea1"));

    gtk_widget_queue_draw(drawingarea);

    distruggi_grafo(G);

    aggiorna_text_view_grafo();
    aggiorna_barra_stato();
}

/**
 * @brief Handler che si occupa di aprire la dialog2 per la ricerca di un
 * percorso
 */
extern "C" void trova_percorso_handler (GtkMenuItem *menuitem, 
	gpointer user_data) {
	GtkWidget *trova_percorso_dialog = GTK_WIDGET(
		gtk_builder_get_object(builder, "dialog2"));
	gtk_widget_show_all(trova_percorso_dialog);
}

/**
 * @brief Handler che mostra la dialog contenente le adiacenze del grafo,
 * scritte su una textview.
 */
extern "C" void mostra_adiacenze_handler (GtkMenuItem *menuitem, 
    gpointer user_data) {
    GtkWidget *mostra_adiacenze_dialog = GTK_WIDGET(
        gtk_builder_get_object(builder, "dialog3"));
    gtk_widget_show_all(mostra_adiacenze_dialog);
}

/**
 * @brief Handler che si occupa di disegnare sia il grafo che il percorso
 * desiderato sulla DrawingArea della finestra principale del programma.
 * 
 * @details Controlla se il grafo è stato inizializzato e disegna il grafo
 * mediante la funzione ::disegna_grafo, inoltre se è stato richiesto di
 * disegnare un percorso, chiama la funzione ::disegna_percorso
 */
extern "C" gboolean 
draw_handler(GtkWidget *widget, cairo_t *cr, gpointer data) {

	if (G.num_nodi > 0) {
		disegna_grafo(widget, cr, G, stampa_pesi);
		if (dis_percorso)
			disegna_percorso(cr, G, sorg_percorso, dest_percorso);
	}
    return FALSE;
}

/**
 * @brief Handler che apre un'istanza di firefox in background per mostrare
 * la documentazione
 */
extern "C" void help_handler(GtkWidget *widget, gpointer data) {

    D1(cout << "Apro la documentazione su firefox..." << endl)
    system("firefox doc/html/index.html &");
}

/**
 * @brief Funzione principale del programma
 * @details Setta i nodi iniziali del grafo a 0 ed inizializza l'interfaccia
 * tramite un builder, utilizzando il file "interfaccia.glade" e connettendo i 
 * segnali.
 * 
 * @return Valore di ritorno del programma
 */
int main(int argc, char *argv[]) {

	G.num_nodi = 0;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();

	gtk_builder_add_from_file(builder, "interfaccia.glade", NULL);

	gtk_builder_connect_signals(builder, NULL);

	gtk_main();
	return 0;
}
