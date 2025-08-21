/**
 * @file Bottom_Bar.c
 * @brief Anzeige der unteren Statusleiste.
 *
 * Dieses Modul setzt die in `Bottom_Bar.h` deklarierten Funktionen
 * um. Es visualisiert Dateistatus und Byteinformationen, berechnet
 * unterschiedliche numerische Interpretationen und nutzt dazu den
 * `Editor` sowie den `DataBuffer`. Fehler bei der Darstellung werden
 * über das zentrale Fehlerlog gemeldet.
 */

//* -------------------------------------- Includes ---------------------------------------
#include "Bottom_Bar.h"      // eigene Schnittstelle einbinden
#include <string.h>           // Funktionen wie memset
#include <stdint.h>           // feste Breiten für Integer-Typen
#include "Err_Log.h"         // Fehlerbehandlung und Logging
#include "Ncurses_Check.h"   // Makro zur Fehlerkontrolle

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Funktionen --------------------------------------
// Vorwärtsdeklaration der Helferfunktion zur Byte-Interpretation
static unsigned long long read_unsigned_le(const DataBuffer *buffer, size_t index,
                                           int byteCount); // Anzahl der Bytes

/**
 * @brief Zeichnet Status und Byte-Informationen.
 */
void bottom_bar_draw(BottomBar *self, const Editor *editor, const DataBuffer *buffer) {
    NCURSES_CHECK(wmove(self->win, 1, 1));          // Cursor auf Innenbereich setzen
    NCURSES_CHECK(wclrtoeol(self->win));            // Zeile bis zum Rand löschen
    NCURSES_CHECK(box(self->win, 0, 0));            // Rahmen neu zeichnen

    const char *stateText;                         // Zeiger auf Status-Text
    if (!buffer->ever_changed) {                   // Datei wurde nie verändert?
        stateText = "NOTHING changed";            // Hinweis entsprechend setzen
    } else if (buffer->edited) {                   // Änderungen noch nicht gespeichert?
        stateText = "file not saved";             // Hinweis setzen
    } else {                                       // sonst: bereits gespeichert
        stateText = "file saved";                 // Hinweis setzen
    }

    int bytesPerLine = getBytesPerLine(editor);    // Bytes pro Zeile ermitteln
    size_t absoluteIndex = editor->start_offset +  // Ausgangsoffset addieren
                           (size_t)editor->cursor_y * (size_t)bytesPerLine +
                           (size_t)editor->cursor_x; // Cursorposition berücksichtigen

    unsigned char byteValue = 0;                   // Standardwert für Byte
    if (absoluteIndex < buffer->size) {            // liegt Index im Buffer?
        byteValue = buffer->bytes[absoluteIndex];  // Byte auslesen
    }

    unsigned long long rawUnsigned32 = read_unsigned_le(buffer, absoluteIndex, 4); // 32 Bit lesen
    signed char signedValue8 = (signed char)byteValue;      // als s8 interpretieren
    unsigned char unsignedValue8 = byteValue;               // als u8 interpretieren
    int32_t signedValue32 = (int32_t)rawUnsigned32;         // als s32 interpretieren
    uint32_t unsignedValue32 = (uint32_t)rawUnsigned32;     // als u32 interpretieren

    NCURSES_CHECK(mvwprintw(self->win, 1, 1,                 // Text im Fenster ausgeben
                  "%s s8:%d u8:%u s32:%d u32:%u",
                  stateText,                                 // Status-Text
                  (int)signedValue8,                         // Wert als s8
                  (unsigned int)unsignedValue8,              // Wert als u8
                  (int)signedValue32,                        // Wert als s32
                  (unsigned int)unsignedValue32));           // Wert als u32

    NCURSES_CHECK(wnoutrefresh(self->win));                 // in Ausgabe-Buffer schreiben
}

//* ---------------------------------- Helfer Funktionen ----------------------------------
/**
 * @brief Liest einen ganzzahligen Wert im Little-Endian-Format.
 *
 * Die Funktion holt nacheinander mehrere Bytes aus dem Daten-Buffer und baut
 * daraus einen "unsigned long long" zusammen. Fehlen Bytes am Ende, werden
 * sie als 0 interpretiert.
 *
 * @param buffer    Buffer mit den Quelldaten
 * @param index     Startposition im Buffer
 * @param byteCount Anzahl der zu lesenden Bytes
 * @return Zusammengesetzter Wert
 */
static unsigned long long read_unsigned_le(const DataBuffer *buffer, size_t index,
                                           int byteCount) {
    unsigned long long value = 0;                   // Ergebnis initialisieren
    for (int i = 0; i < byteCount; i++) {           // Schleife über alle Bytes
        size_t currentIndex = index + (size_t)i;    // aktuelle Position bestimmen
        unsigned char currentByte = 0;              // Standardwert außerhalb
        if (currentIndex < buffer->size) {          // liegt Position im Buffer?
            currentByte = buffer->bytes[currentIndex]; // Byte auslesen
        }
        value |= ((unsigned long long)currentByte) << (8 * i); // Byte verschieben
    }
    return value;                                  // zusammengesetzten Wert liefern
}

//* ----------------------------------- Init und Deinit -----------------------------------
/**
 * @brief Initialisiert die Bottom-Bar.
 */
int bottom_bar_init(BottomBar *self, int width, int starty) {
    memset(self, 0, sizeof(*self));            // gesamte Struktur auf 0 setzen
    self->win = newwin(3, width, starty, 0);   // neues Fenster erzeugen
    if (!self->win) {                          // Fenstererzeugung fehlgeschlagen?
        fatal_error("bottom_bar_init", "newwin"); // Fehler melden und abbrechen
    }
    return 0;                                  // Erfolg zurückgeben
}

/**
 * @brief Gibt Ressourcen der Bottom-Bar frei.
 */
int bottom_bar_deinit(BottomBar *self) {
    if (self->win) {                         // Existiert ein Fenster?
        NCURSES_CHECK(delwin(self->win));    // Fenster freigeben
    }
    memset(self, 0, sizeof(*self));          // Struktur auf 0 zurücksetzen
    return 0;                                // Erfolg melden
}

