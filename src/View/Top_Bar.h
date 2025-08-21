#ifndef TOP_BAR_H            // Verhindert doppelte Einbindung
#define TOP_BAR_H            // Beginn des Include-Guards

/**
 * @file Top_Bar.h
 * @brief Schnittstelle der oberen Statusleiste.
 *
 * Dieser Header definiert die kleine Fensterkomponente, die am
 * oberen Bildschirmrand Hinweise zu Tastenkürzeln anzeigt. Die
 * Top-Bar ist weitgehend unabhängig vom restlichen Programm und
 * bietet lediglich Funktionen zum Zeichnen sowie zum Anlegen und
 * Freigeben des entsprechenden `ncurses`-Fensters.
 */

#ifdef _WIN32
#include <ncurses/ncurses.h> // Windows-Spezifikum
#else
#include <ncurses.h>         // Standard-ncurses auf Unix
#endif

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Strukturen --------------------------------------
/**
 * @brief Struktur der Top-Bar.
 */
typedef struct {           // Daten der Top-Bar
    WINDOW *win; /**< ncurses-Fenster der Top-Bar */
} Top_Bar;                 // Typname der Top-Bar-Struktur

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Zeichnet die Top-Bar.
 */
void top_bar_draw(Top_Bar *bar);      // zeichnet die Top-Bar

//* ----------------------------------- Init und Deinit -----------------------------------
/**
 * @brief Initialisiert die Top-Bar.
 */
int top_bar_init(Top_Bar *bar, int width); // legt die Top-Bar an

/**
 * @brief Gibt Ressourcen der Top-Bar frei.
 */
int top_bar_deinit(Top_Bar *bar);     // gibt Ressourcen frei

#endif // TOP_BAR_H          // Ende des Include-Guards

