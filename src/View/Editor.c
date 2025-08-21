/**
 * @file Editor.c
 * @brief Verwaltung des Editorfensters.
 *
 * Dieses Modul enthält die Kernlogik zur Lebenszyklusverwaltung des
 * Editorfensters. Es initialisiert die benötigten `ncurses`-Strukturen
 * und stellt Aufräumfunktionen bereit. Zeichnen und Eingabe werden in
 * getrennten Dateien umgesetzt, sodass die Verwaltung hier
 * übersichtlich bleibt. Der Editor dient als Bindeglied zwischen
* Anzeige-Strategien und Daten-Buffer.
 */

//* -------------------------------------- Includes ---------------------------------------
#include "Editor.h"
#include <string.h>
#include "Utilities.h"
#include "Display_Strategy.h"
#include "Err_Log.h"
#include "Ncurses_Check.h" // prüft ncurses-Rückgaben

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Funktionen --------------------------------------

//* ---------------------------------- Helfer Funktionen ----------------------------------

//* ----------------------------------- Init und Deinit -----------------------------------
/**
 * @brief Initialisiert den Editor.
 *
 * Legt das Fenster an und setzt alle Startwerte, damit eine Datei angezeigt
 * werden kann.
 */
int editor_init(Editor *self, int height, int width, int starty, DataBuffer *buffer) {
    memset(self, 0, sizeof(*self));                 // Struktur leeren
    self->win = newwin(height, width, starty, 0);   // Fenster erzeugen
    if (!self->win) {                               // Prüfung auf Erfolg
        fatal_error("editor_init", "newwin");    // Fehlerbehandlung
    }
    self->buffer = buffer;                          // Buffer merken
    self->start_offset = 0;                         // Anzeige ab Byte 0
    self->cursor_y = 0;                             // Cursor in erste Zeile
    self->cursor_x = 0;                             // Cursor in erste Spalte
    self->cursor_area = AREA_HEX;                   // Hex-Bereich aktivieren
    self->hex_pending = -1;                         // Kein offener Hex-Nibble
    self->bin_pending_bits = 0;                     // Keine Binärbits vorgemerkt
    self->bin_pending_value = 0;                    // Zwischenspeicher leeren
    self->display_mode = DISPLAY_HEX;               // Standarddarstellung
    self->strategy = (self->display_mode == DISPLAY_HEX)
                        ? &HEX_STRATEGY
                        : &BIN_STRATEGY;            // passende Strategie wählen
    NCURSES_CHECK(keypad(self->win, TRUE));         // Funktionstasten aktivieren
    return 0;                                       // Erfolg zurückgeben
}

/**
 * @brief Gibt den Editor frei.
 */
int editor_deinit(Editor *self) {
    if (self->win) {                               // Existiert ein Fenster?
        NCURSES_CHECK(delwin(self->win));          // Fenster freigeben
    }
    memset(self, 0, sizeof(*self));                // Struktur zurücksetzen
    return 0;                                      // Erfolg melden
}


