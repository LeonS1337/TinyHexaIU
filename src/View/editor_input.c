/**
 * @file editor_input.c
 * @brief Verarbeitung der Tastatureingaben im Editor.
 *
 * Dieses Modul interpretiert sämtliche Tastatureingaben im
 * Editorfenster. Es steuert Cursorbewegung, Scrollen, Bereichswechsel
 * sowie die Eingabe von Hex- oder Binärwerten und aktualisiert bei
 * Bedarf den `DataBuffer`. Durch die Auslagerung der Eingabelogik
 * bleibt der eigentliche Editor-Code schlank und gut wartbar.
 */

//* -------------------------------------- Includes ---------------------------------------
#include "Editor.h"
#include "Utilities.h"
#include "Display_Strategy.h"
#include <ctype.h>
#include <string.h>

//* ---------------------------- Globale Konstanten oder Defines --------------------------
// Vorwärtsdeklarationen der Helferfunktionen
static void advance_cursor(Editor *self);
static void handle_ascii(Editor *self, int key, size_t index);
static void handle_hex_input(Editor *self, int key, size_t index);
static void handle_bin_input(Editor *self, int key, size_t index);
static void move_cursor_vertical(Editor *self, int deltaY, int rowCount, size_t pageBytes);
static void move_cursor_horizontal(Editor *self, int deltaX);
static void clamp_cursor(Editor *self);

// Dispatch based key handling
typedef void (*KeyHandlerFn)(Editor *, int, size_t);
static const KeyHandlerFn dispatch_table[2 /*CursorArea*/][2 /*DisplayMode*/] = {
    /* AREA_HEX */
    { handle_hex_input, handle_bin_input },
    /* AREA_ASCII */
    { handle_ascii,     handle_ascii }
};

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Leitet Tasteneingaben an passende Handler weiter.
 * @param self Editorinstanz
 * @param key  eingelesenes Zeichen
 */
void editor_handle_char(Editor *self, int key) {
    size_t index = self->start_offset +
                   (size_t)self->cursor_y * (size_t)getBytesPerLine(self) +
                   (size_t)self->cursor_x;
    KeyHandlerFn handler = dispatch_table[self->cursor_area][self->display_mode];
    if (handler) {
        handler(self, key, index);
    }
}

/**
 * @brief Bewegt den Cursor um die angegebenen Werte.
 * @param self   Editorinstanz
 * @param deltaY Verschiebung in Zeilen
 * @param deltaX Verschiebung in Spalten
 */
void editor_move_cursor(Editor *self, int deltaY, int deltaX) {
    int rowCount = getmaxy(self->win) - 3;          // nutzbare Zeilen im Fenster
    size_t pageBytes = (size_t)rowCount * (size_t)getBytesPerLine(self); // Bytes pro Seite

    move_cursor_vertical(self, deltaY, rowCount, pageBytes);
    move_cursor_horizontal(self, deltaX);
    clamp_cursor(self);
}

/**
 * @brief Scrollt den sichtbaren Bereich.
 * @param self         Editorinstanz
 * @param delta_offset Verschiebung des Startoffsets
 */
void editor_scroll(Editor *self, int delta_offset) {
    int rows = getmaxy(self->win) - 3;               // nutzbare Zeilen im Fenster
    size_t page = (size_t)rows * (size_t)getBytesPerLine(self); // Bytes pro Seite

    if (delta_offset > 0) { // nach unten scrollen
        size_t max_offset = (self->buffer->size > page) ? self->buffer->size - page : 0; // größter gültiger Offset
        size_t new_offset = self->start_offset + (size_t)delta_offset; // neuen Offset berechnen
        if (new_offset > max_offset) {
            new_offset = max_offset; // über Ende hinaus: auf Maximum begrenzen
        }
        self->start_offset = new_offset; // Scroll anwenden
    } else if (delta_offset < 0) { // nach oben scrollen
        size_t abs_offset = (size_t)(-delta_offset); // Betrag des Offsets
        if (abs_offset > self->start_offset) {
            self->start_offset = 0; // nicht vor den Anfang scrollen
        } else {
            self->start_offset -= abs_offset; // neuen Offset setzen
        }
    }

    // Nach dem Scrollen sicherstellen, dass der Cursor gültig bleibt
    editor_move_cursor(self, 0, 0);
}

/**
 * @brief Wechselt zwischen Hex- und ASCII-Bereich.
 */
void editor_toggle_area(Editor *self) {
    if (self->cursor_area == AREA_HEX) {
        self->cursor_area = AREA_ASCII; // in ASCII-Bereich wechseln
    } else {
        self->cursor_area = AREA_HEX;   // zurück zum Hex-Bereich
    }
    clamp_cursor(self); // Cursorposition validieren
}

/**
 * @brief Schaltet zwischen Hex- und Binärdarstellung um.
 */
void editor_toggle_display_mode(Editor *self) {
    size_t absoluteIndex = self->start_offset +
                           (size_t)self->cursor_y * (size_t)getBytesPerLine(self) +
                           (size_t)self->cursor_x; // aktuelle absolute Position
    self->display_mode = (self->display_mode == DISPLAY_HEX) ? DISPLAY_BIN : DISPLAY_HEX; // Modus wechseln
    self->strategy = (self->display_mode == DISPLAY_HEX) ? &HEX_STRATEGY : &BIN_STRATEGY; // passende Strategie
    self->hex_pending = -1;       // angefangene Hex-Eingabe zurücksetzen
    self->bin_pending_bits = 0;   // Bitzähler leeren
    self->bin_pending_value = 0;  // Zwischenspeicher löschen
    int bytesPerLine = getBytesPerLine(self);
    int rows = getmaxy(self->win) - 3;
    size_t pageBytes = (size_t)rows * (size_t)bytesPerLine; // Größe einer Seite
    if (absoluteIndex < self->start_offset ||
        absoluteIndex >= self->start_offset + pageBytes) {
        self->start_offset = (absoluteIndex / (size_t)bytesPerLine) * (size_t)bytesPerLine; // Seite neu ausrichten
    }
    size_t relative = absoluteIndex - self->start_offset; // relative Position auf Seite
    self->cursor_y = (int)(relative / (size_t)bytesPerLine);
    self->cursor_x = (int)(relative % (size_t)bytesPerLine);
}

/**
 * @brief Setzt den Editor in den Ausgangszustand.
 *
 * Alle Cursordaten und Zwischenspeicher werden gelöscht, damit der Editor
 * mit einer frischen Ansicht startet.
 */
void editor_reset(Editor *self) {
    self->start_offset = 0;                // Anzeige auf Dateianfang setzen
    self->cursor_y = 0;                    // Cursorzeile zurücksetzen
    self->cursor_x = 0;                    // Cursorspalte zurücksetzen
    self->cursor_area = AREA_HEX;          // Hex-Bereich aktivieren
    self->hex_pending = -1;                // Offenen Hex-Nibble verwerfen
    self->bin_pending_bits = 0;            // Anzahl gemerkter Binärbits löschen
    self->bin_pending_value = 0;           // Zwischengespeicherten Binärwert löschen
    self->display_mode = DISPLAY_HEX;      // Standardmäßig Hexanzeige wählen
    self->strategy = (self->display_mode == DISPLAY_HEX)
                       ? &HEX_STRATEGY
                       : &BIN_STRATEGY;    // passende Strategie setzen
}

//* ---------------------------------- Helfer Funktionen ----------------------------------
// Behandelt ASCII-Zeichen direkt als Byte
static void handle_ascii(Editor *self, int key, size_t index) {
    if (isprint(key)) { // nur druckbare Zeichen akzeptieren
        data_buffer_set_byte(self->buffer, index, (unsigned char)key); // Byte schreiben
        advance_cursor(self); // Cursor weiterbewegen
    }
}

// Verarbeitet Eingaben im Hexadezimalmodus
static void handle_hex_input(Editor *self, int key, size_t index) {
    if (!isxdigit(key)) return; // nur 0-9a-f zulassen
    int value = (key <= '9') ? key - '0' : (tolower(key) - 'a' + 10); // Zeichen in Zahl wandeln
    if (self->hex_pending < 0) {
        self->hex_pending = value; // ersten Nibble merken
    } else {
        unsigned char byte = (unsigned char)((self->hex_pending << 4) | value); // Byte bilden
        data_buffer_set_byte(self->buffer, index, byte); // Byte in Buffer schreiben
        self->hex_pending = -1; // Zustand zurücksetzen
        advance_cursor(self); // Cursor zur nächsten Position bewegen
    }
}

// Verarbeitet Eingaben im Binärmodus
static void handle_bin_input(Editor *self, int key, size_t index) {
    if (key != '0' && key != '1') return; // nur 0 oder 1 akzeptieren
    // Wert aufbauen: bestehende Bits nach links und neues Bit anhängen
    self->bin_pending_value =
        (unsigned char)((self->bin_pending_value << 1) | (key - '0'));
    self->bin_pending_bits++; // Bitzähler erhöhen
    if (self->bin_pending_bits == 8) { // vollständiges Byte erreicht
        data_buffer_set_byte(self->buffer, index, self->bin_pending_value);
        self->bin_pending_bits = 0;    // Zähler zurücksetzen
        self->bin_pending_value = 0;   // Zwischenspeicher leeren
        advance_cursor(self);          // Cursor weiter
    }
}

static void move_cursor_vertical(Editor *self, int deltaY, int rowCount, size_t pageBytes) {
    int bytesPerLine = getBytesPerLine(self); // Bytes pro Zeile ermitteln

    if (deltaY < 0) { // Bewegung nach oben
        if (self->cursor_y > 0) { // innerhalb des Fensters
            self->cursor_y--; // Zeile nach oben
        } else if (self->start_offset >= (size_t)bytesPerLine) {
            self->start_offset -= (size_t)bytesPerLine; // eine Seite nach oben scrollen
        }
    } else if (deltaY > 0) { // Bewegung nach unten
        size_t nextIndex = idx(self, self->cursor_y + 1, self->cursor_x); // Index in nächster Zeile
        if (self->cursor_y < rowCount - 1 && nextIndex < self->buffer->size) {
            self->cursor_y++; // Zeile nach unten
        } else if (self->start_offset + pageBytes < self->buffer->size) {
            self->start_offset += (size_t)bytesPerLine; // eine Seite nach unten scrollen
        }
    }
}

static void move_cursor_horizontal(Editor *self, int deltaX) {
    int bytesPerLine = getBytesPerLine(self); // Bytes pro Zeile ermitteln

    // Wechsel zwischen Hex- und ASCII-Bereich per Pfeiltasten vermeiden
    if (deltaX < 0) { // nach links
        if (self->cursor_x > 0) { // nicht über Zeilenanfang hinaus
            self->cursor_x--;
        }
    } else if (deltaX > 0) { // nach rechts
        size_t nextIndex = idx(self, self->cursor_y, self->cursor_x + 1); // Index rechts
        if (self->cursor_x < bytesPerLine - 1 && nextIndex < self->buffer->size) {
            self->cursor_x++; // Cursor verschieben
        }
    }

    // Im aktuellen Anzeigemodus angefangene Eingabe verwerfen
    self->strategy->reset_pending(self);
}

static void clamp_cursor(Editor *self) {
    size_t lineBytes = 0;
    size_t lineStart = idx(self, self->cursor_y, 0); // Startindex der aktuellen Zeile

    // Anzahl der pro Zeile darstellbaren Bytes ermitteln
    size_t maxPerLine = (size_t)getBytesPerLine(self);

    if (lineStart < self->buffer->size) { // es gibt Daten in dieser Zeile
        lineBytes = self->buffer->size - lineStart; // verbleibende Bytes bestimmen
        if (lineBytes > maxPerLine) {
            lineBytes = maxPerLine; // auf Zeilenbreite begrenzen
        }
    }
    if (self->cursor_x >= (int)lineBytes) {
        // Cursor am Ende der vorhandenen Daten positionieren
        self->cursor_x = (lineBytes == 0) ? 0 : (int)lineBytes - 1;
    }
}

// Cursor nach rechts verschieben und am Zeilenende einen Zeilenumbruch auslösen
static void advance_cursor(Editor *self) {
    int bytesPerLine = getBytesPerLine(self); // Anzahl Bytes pro Zeile
    size_t nextIndex = idx(self, self->cursor_y, self->cursor_x + 1); // Index des nächsten Bytes

    if (self->cursor_x < bytesPerLine - 1 && nextIndex < self->buffer->size) {
        // Innerhalb derselben Zeile einen Schritt nach rechts gehen
        self->cursor_x++;
    } else if (nextIndex < self->buffer->size) {
        // Am Zeilenende: zur nächsten Zeile springen, wenn noch Daten folgen
        int rowCount = getmaxy(self->win) - 3;                           // nutzbare Zeilen im Fenster
        size_t pageBytes = (size_t)rowCount * (size_t)bytesPerLine;       // Bytes pro Seite
        self->cursor_x = 0;                                              // Spalte auf Anfang setzen
        move_cursor_vertical(self, 1, rowCount, pageBytes);              // eine Zeile nach unten
    } else {
        // Am Dateiende: Cursor verbleibt auf dem letzten Element
        // self->cursor_x unverändert lassen
    }

    // Angefangene Eingabe im aktuellen Modus verwerfen
    self->strategy->reset_pending(self);
    // Cursorposition innerhalb der Daten begrenzen
    clamp_cursor(self);
}

//* ----------------------------------- Init und Deinit -----------------------------------
