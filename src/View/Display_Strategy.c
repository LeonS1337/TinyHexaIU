/**
 * @file Display_Strategy.c
 * @brief Implementierung der Anzeige-Strategien.
 *
 * Dieses Modul definiert zwei konkrete Strategien zur Darstellung
 * des Buffers: eine hexadezimale und eine binäre Variante. Jede
 * Strategie stellt eine Reihe von Funktionszeigern bereit, die vom
 * Editor während Zeichnen und Eingabeverarbeitung genutzt werden.
 * Durch diese Kapselung lässt sich die Darstellung zur Laufzeit
 * umschalten, ohne den Editorcode zu verändern.
 */

//* -------------------------------------- Includes ---------------------------------------
#include "Display_Strategy.h"
#include "Utilities.h"
#include "Editor.h" ///< Zugriff auf den Editor zum Rücksetzen offener Eingaben

//* ---------------------------- Globale Konstanten oder Defines --------------------------
// Vorwärtsdeklarationen der Helferfunktionen
static int hex_bytes_per_line(void);
static int bin_bytes_per_line(void);
static int hex_cell_width(void);
static int bin_cell_width(void);
static int hex_is_valid_char(int ch);
static int bin_is_valid_char(int ch);
static void hex_format_byte(unsigned char byte, char *out);
static void bin_format_byte(unsigned char byte, char *out);
static const char *hex_header_label(void);
static const char *bin_header_label(void);
static const char *hex_empty_cell(void);
static const char *bin_empty_cell(void);
static void hex_reset_pending(Editor *self);
static void bin_reset_pending(Editor *self);

/** Strategie für die Hex-Darstellung. */
const DisplayStrategy HEX_STRATEGY = {
    .bytes_per_line = hex_bytes_per_line,
    .cell_width = hex_cell_width,
    .is_valid_char = hex_is_valid_char,
    .format_byte = hex_format_byte,
    .header_label = hex_header_label,
    .empty_cell = hex_empty_cell,
    .reset_pending = hex_reset_pending
};

/** Strategie für die Binär-Darstellung. */
const DisplayStrategy BIN_STRATEGY = {
    .bytes_per_line = bin_bytes_per_line,
    .cell_width = bin_cell_width,
    .is_valid_char = bin_is_valid_char,
    .format_byte = bin_format_byte,
    .header_label = bin_header_label,
    .empty_cell = bin_empty_cell,
    .reset_pending = bin_reset_pending
};

//* ------------------------------------- Funktionen --------------------------------------

//* ---------------------------------- Helfer Funktionen ----------------------------------
// Parameterfunktionen
/** @brief Bytes pro Zeile in der Hex-Ansicht. */
static int hex_bytes_per_line(void) { return 16; }

/** @brief Bytes pro Zeile in der Binär-Ansicht. */
static int bin_bytes_per_line(void) { return 4; }

/** @brief Breite einer Hex-Zelle. */
static int hex_cell_width(void) { return 3; }

/** @brief Breite einer Binär-Zelle. */
static int bin_cell_width(void) { return 9; }

/** @brief Prüft, ob ein Zeichen für die Hex-Eingabe gültig ist. */
static int hex_is_valid_char(int ch) { return isxdigit(ch); }

/** @brief Prüft, ob ein Zeichen für die Binär-Eingabe gültig ist. */
static int bin_is_valid_char(int ch) { return ch == '0' || ch == '1'; }

// Formatierung der Byte-Ausgabe
/**
 * @brief Formatiert ein Byte als Hex-Zeichenkette.
 * @param byte  Wert, der ausgegeben werden soll
 * @param out   Ziel-Buffer, mindestens 4 Zeichen lang
 */
static void hex_format_byte(unsigned char byte, char *out) {
    byte_to_hex_string(byte, out); // Byte in zwei Hex-Zeichen umwandeln
    out[2] = ' ';                  // Leerzeichen als Trenner anhängen
    out[3] = '\0';                // String terminieren
}

/**
 * @brief Formatiert ein Byte als Binär-Zeichenkette.
 * @param byte  Wert, der ausgegeben werden soll
 * @param out   Ziel-Buffer, mindestens 10 Zeichen lang
 */
static void bin_format_byte(unsigned char byte, char *out) {
    byte_to_bit_string(byte, out); // Acht Bit in Zeichen wandeln
    out[8] = ' ';                  // Leerzeichen als Trenner anhängen
    out[9] = '\0';                // String terminieren
}

/** @brief Überschrift für die Hex-Spalte. */
static const char *hex_header_label(void) { return "Hex"; }

/** @brief Überschrift für die Binär-Spalte. */
static const char *bin_header_label(void) { return "Bin"; }

/** @brief Rückgabe für eine leere Hex-Zelle. */
static const char *hex_empty_cell(void) { return "   "; }

/** @brief Rückgabe für eine leere Binär-Zelle. */
static const char *bin_empty_cell(void) { return "         "; }

// Zurücksetzen angefangener Eingaben
/** @brief Setzt ausstehende Hex-Eingaben zurück. */
static void hex_reset_pending(Editor *self) { self->hex_pending = -1; }

/** @brief Setzt ausstehende Binär-Eingaben zurück. */
static void bin_reset_pending(Editor *self) {
    self->bin_pending_bits = 0;   // Anzahl bereits eingegebener Bits löschen
    self->bin_pending_value = 0;  // Zwischengespeicherten Wert löschen
}

//* ----------------------------------- Init und Deinit -----------------------------------
