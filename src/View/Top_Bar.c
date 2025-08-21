/**
 * @file Top_Bar.c
 * @brief Darstellung der oberen Statusleiste.
 *
 * Dieses Modul enthält die Implementierungen für die obere
 * Statusleiste. Es zeichnet feste Hinweise zu den wichtigsten
 * Tastenkombinationen und verwaltet ein eigenes `ncurses`-Fenster.
 * Bei Fehlern während der Initialisierung oder Ausgabe wird das
 * zentrale Fehlerlog informiert.
 */

//* -------------------------------------- Includes ---------------------------------------
#include "Top_Bar.h"         // Schnittstelle der Top-Bar
#include <string.h>           // für memset
#include "Err_Log.h"         // Zugriff auf zentrales Fehlerlog
#include "Ncurses_Check.h"   // zentrale Prüfung für ncurses-Aufrufe

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Zeichnet Hinweise zu den Tasten.
 *
 * @param bar Zeiger auf die zu zeichnende Top-Bar
 */
void top_bar_draw(Top_Bar *bar) {
    NCURSES_CHECK(box(bar->win, 0, 0)); // Rahmen um das Fenster zeichnen
    NCURSES_CHECK(mvwprintw(bar->win, 1, 1, "%s",
                  "PgUp/PgDn: Scroll  Tab: Switch  F1: Hex/Bin  F2: Save  F3: Reload  F12: Quit"));
    NCURSES_CHECK(wnoutrefresh(bar->win)); // Ausgaben sammeln, aber noch nicht anzeigen
}
 
//* ---------------------------------- Helfer Funktionen ----------------------------------

//* ----------------------------------- Init und Deinit -----------------------------------
/**
 * @brief Initialisiert die Top-Bar.
 *
 * @param bar   Zeiger auf die zu initialisierende Struktur
 * @param width Breite des Fensters in Zeichen
 * @return 0 bei Erfolg
 */
int top_bar_init(Top_Bar *bar, int width) {
    memset(bar, 0, sizeof(*bar));                // gesamte Struktur auf Null setzen
    bar->win = newwin(3, width, 0, 0);           // Fenster anlegen (3 Zeilen, ganze Breite)
    if (!bar->win) {                             // Fenstererstellung prüfen
        fatal_error("top_bar_init", "newwin"); // Fehler protokollieren und Programm beenden
    }
    return 0;                                    // Erfolg zurückgeben
}

/**
 * @brief Gibt Ressourcen der Top-Bar frei.
 *
 * @param bar Zeiger auf die zu freigebende Struktur
 * @return 0 bei Erfolg
 */
int top_bar_deinit(Top_Bar *bar) {
    if (bar->win) {                               // nur wenn das Fenster existiert
        NCURSES_CHECK(delwin(bar->win));          // Fenster freigeben
    }
    memset(bar, 0, sizeof(*bar));                 // Struktur zurücksetzen
    return 0;                                     // Erfolg zurückgeben
}

