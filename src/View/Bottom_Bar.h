#ifndef BOTTOM_BAR_H   // Schutz vor mehrfacher Einbindung
#define BOTTOM_BAR_H   // Beginn des Include-Guards

/**
 * @file Bottom_Bar.h
 * @brief Schnittstelle der unteren Statusleiste.
 *
 * Dieser Header beschreibt die Statusleiste am unteren Rand des
 * Hauptfensters. Sie zeigt sowohl den Zustand der Datei als auch
 * verschiedene numerische Interpretationen des aktuell ausgewählten
 * Bytes. Dazu wird sie mit Referenzen auf `Editor` und `DataBuffer`
 * versorgt und stellt Funktionen zur Initialisierung, Ausgabe und
 * Aufräumung bereit.
 */

#ifdef _WIN32                       // spezieller Pfad für Windows
#include <ncurses/ncurses.h>        // ncurses unter Windows
#else                               // andere Plattformen
#include <ncurses.h>                // ncurses Standard-Header
#endif                              // Ende der Plattformwahl
#include "Editor.h"                // Zugriff auf Editor-Struktur

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Strukturen --------------------------------------
/**
 * @brief Struktur der Bottom-Bar.
 */
typedef struct {                       // Struktur der Bottom-Bar
    WINDOW *win; /**< Fenster für die Bottom Bar */
} BottomBar;                           // Typname der Struktur

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Zeichnet die Bottom-Bar.
 */
void bottom_bar_draw(BottomBar *self, const Editor *editor, const DataBuffer *buffer); // Statusleiste zeichnen

//* ----------------------------------- Init und Deinit -----------------------------------
/**
 * @brief Initialisiert die Bottom-Bar.
 */
int bottom_bar_init(BottomBar *self, int width, int starty); // Initialisierung

/**
 * @brief Gibt Ressourcen der Bottom-Bar frei.
 */
int bottom_bar_deinit(BottomBar *self); // Ressourcen freigeben

#endif // BOTTOM_BAR_H   // Ende des Include-Guards

