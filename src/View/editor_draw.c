/**
 * @file editor_draw.c
 * @brief Zeichenroutinen für den Editor.
 *
 * Dieses Modul übernimmt die komplette Darstellung des Editors auf
 * dem Bildschirm. Es zeichnet Rahmen, Überschriften, die eigentlichen
 * Datenzeilen sowie den Cursor. Die Formatierung der Bytewerte
 * richtet sich nach der aktiven `DisplayStrategy`. Alle
 * Zeichenfunktionen prüfen auf Fehler und melden diese über das
 * zentrale Fehlersystem.
 */

//* -------------------------------------- Includes ---------------------------------------
#include "Editor.h"
#include "Utilities.h"
#include "Display_Strategy.h"
#include <string.h>
#include <ctype.h>
#include "Err_Log.h"
#include "Ncurses_Check.h" // Makro für sichere ncurses-Aufrufe

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Funktionen --------------------------------------
// Vorwärtsdeklarationen der Helferfunktionen
static void draw_editor_frame(Editor *self);
static void draw_editor_header(Editor *self);
static void draw_editor_line(Editor *self, int row);
static void update_editor_cursor(Editor *self);

// Gesamten Editorinhalt zeichnen
void editor_draw(Editor *self) {
    int rowCount = getmaxy(self->win) - 3; // verfügbare Zeilen für Daten
    NCURSES_CHECK(werase(self->win));      // Fensterinhalt löschen
    draw_editor_frame(self);   // Rahmen zeichnen
    draw_editor_header(self);  // Kopfzeile ausgeben
    for (int row = 0; row < rowCount; row++) { // jede Zeile ausgeben
        draw_editor_line(self, row);
    }
    update_editor_cursor(self); // Cursor positionieren
    NCURSES_CHECK(wrefresh(self->win)); // Fenster aktualisieren
}

//* ---------------------------------- Helfer Funktionen ----------------------------------
// Zeichnet einen einfachen Rahmen
static void draw_editor_frame(Editor *self) {
    NCURSES_CHECK(box(self->win, 0, 0)); // Rahmen um das Fenster zeichnen
}

// Zeichnet Überschriften für Offset, Daten und ASCII
static void draw_editor_header(Editor *self) {
    NCURSES_CHECK(mvwprintw(self->win, 1, 1, "%s", "Offset")); // Spaltenüberschrift für Offset
    int asciiStartX = getAsciiStartX(self); // Startposition für ASCII-Bereich
    NCURSES_CHECK(mvwprintw(self->win, 1, HEX_START_X, "%s", self->strategy->header_label()));
    NCURSES_CHECK(mvwprintw(self->win, 1, asciiStartX, "%s", "ASCII"));
}

// Zeichnet eine Datenzeile
static void draw_editor_line(Editor *self, int row) {
    NCURSES_CHECK(mvwprintw(self->win, row + 2, 1, "%08lx", (unsigned long)idx(self, row, 0)));
    int cellWidth = getCellWidth(self);   // Breite einer Zelle im Hex-Bereich
    int bytesPerLine = getBytesPerLine(self); // Anzahl der Bytes pro Zeile
    int asciiStartX = getAsciiStartX(self);   // Startposition des ASCII-Bereichs

    // Hex- oder Binärwerte zeichnen
    for (int col = 0; col < bytesPerLine; col++) {
        size_t index = idx(self, row, col);              // Index im Buffer
        int x = HEX_START_X + col * cellWidth;           // x-Position der Zelle
        if (index < self->buffer->size) {                // Daten vorhanden
            char cellString[10];
            self->strategy->format_byte(self->buffer->bytes[index], cellString); // Byte formatieren
            NCURSES_CHECK(mvwprintw(self->win, row + 2, x, "%s", cellString));
        } else { // außerhalb des Buffers
            NCURSES_CHECK(mvwprintw(self->win, row + 2, x, "%s", self->strategy->empty_cell()));
        }
    }

    // ASCII-Repräsentation zeichnen
    for (int col = 0; col < bytesPerLine; col++) {
        size_t index = idx(self, row, col);
        int x = asciiStartX + col; // Spaltenposition im ASCII-Bereich
        if (index < self->buffer->size) {
            unsigned char currentChar = self->buffer->bytes[index];
            if (isprint(currentChar)) { // druckbares Zeichen anzeigen
                NCURSES_CHECK(mvwaddch(self->win, row + 2, x, (chtype)currentChar));
            } else { // nicht druckbares Zeichen als Punkt
                NCURSES_CHECK(mvwaddch(self->win, row + 2, x, (chtype)'.'));
            }
        } else { // außerhalb des Buffers: Leerzeichen
            NCURSES_CHECK(mvwaddch(self->win, row + 2, x, (chtype)' '));
        }
    }
}

// Positioniert den Cursor im aktiven Bereich
static void update_editor_cursor(Editor *self) {
    int cursorY = self->cursor_y + 2; // y-Position innerhalb des Fensters
    int cellWidth = getCellWidth(self); // Breite einer Zelle
    int asciiStartX = getAsciiStartX(self); // Start des ASCII-Bereichs
    int cursorX = (self->cursor_area == AREA_HEX)
                      ? HEX_START_X + self->cursor_x * cellWidth
                      : asciiStartX + self->cursor_x; // x-Position abhängig vom Bereich
    NCURSES_CHECK(wmove(self->win, cursorY, cursorX)); // Cursor setzen
}

//* ----------------------------------- Init und Deinit -----------------------------------
