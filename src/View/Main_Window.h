#ifndef MAIN_WINDOW_H                        // Schutz vor mehrfacher Einbindung
#define MAIN_WINDOW_H                        // Beginn des Include-Guards

/**
 * @file Main_Window.h
 * @brief Schnittstelle des Hauptfensters.
 *
 * Dieser Header beschreibt die zentrale Fensterstruktur, welche
 * obere und untere Statusleiste sowie Editor vereint.
 */

#ifdef _WIN32                                 // Plattformabhängige Einbindung
#include <ncurses/ncurses.h>                  // ncurses für Windows
#else                                         // Unix-Variante
#include <ncurses.h>                          // ncurses für Unix-Systeme
#endif
#include "Top_Bar.h"                          // Schnittstelle der oberen Leiste
#include "Editor.h"                           // Schnittstelle des Editors
#include "Bottom_Bar.h"                       // Schnittstelle der unteren Leiste

/**
 * @brief Enthält alle Bereiche des Hauptfensters.
 */
typedef struct {
    Top_Bar top_bar;      /**< obere Leiste */
    Editor editor;        /**< Editor-Bereich */
    BottomBar bottom_bar; /**< untere Leiste */
} Main_Window;

/**
 * @brief Zeichnet alle Bereiche.
 */
void main_window_draw(Main_Window *self);

/**
 * @brief Initialisiert das Hauptfenster.
 */
int main_window_init(Main_Window *self, DataBuffer *buffer);

/**
 * @brief Gibt Ressourcen frei.
 */
int main_window_deinit(Main_Window *self);

#endif // MAIN_WINDOW_H                       // Ende des Include-Guards

