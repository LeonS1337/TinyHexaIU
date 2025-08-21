/**
 * @file Utilities.c
 * @brief Hilfsfunktionen zur Umwandlung von Bytes.
 *
 * Hier befinden sich die Implementierungen der im Header
 * deklarierten Umwandlungsroutinen. Die Funktionen wandeln einzelne
 * Bytes in Bit- bzw. Hexdarstellungen und werden vor allem vom
 * View-Modul genutzt, um die Darstellung der Daten zu formatieren.
 * Es werden bewusst nur elementare Operationen verwendet, um die
 * Wiederverwendbarkeit zu erhöhen.
 */
//* -------------------------------------- Includes ---------------------------------------
#include "Utilities.h"

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Wandelt ein Byte in eine achtstellige Bitfolge.
 *
 * @param value Eingabewert
 * @param out   Ausgabe-Buffer mit Platz für 9 Zeichen
 */
void byte_to_bit_string(unsigned char value, char *out) {
    // Bits von MSB bis LSB durchlaufen
    for (int bit = 7; bit >= 0; bit--) {
        out[7 - bit] = (value & (1 << bit)) ? '1' : '0';
    }
    out[8] = '\0'; // String abschließen
}

/**
 * @brief Wandelt ein Byte in eine zweistellige Hexzahl.
 *
 * @param value Eingabewert
 * @param out   Ausgabe-Buffer mit Platz für 3 Zeichen
 */
void byte_to_hex_string(unsigned char value, char *out) {
    static const char *hex_chars = "0123456789abcdef";
    out[0] = hex_chars[(value >> 4) & 0xF]; // höherwertiges Nibble
    out[1] = hex_chars[value & 0xF];        // niederwertiges Nibble
    out[2] = '\0';                         // String abschließen
}

//* ---------------------------------- Helfer Funktionen ----------------------------------

//* ----------------------------------- Init und Deinit -----------------------------------

