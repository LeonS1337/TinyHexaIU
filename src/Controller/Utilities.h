#ifndef UTILITIES_H
#define UTILITIES_H

/**
 * @file Utilities.h
 * @brief Deklaration von Hilfsfunktionen zur Byteumwandlung.
 *
 * Dieses Modul stellt einfache Funktionen bereit, um einzelne
 * Bytes in Zeichenketten für binäre oder hexadezimale Darstellung
 * umzuwandeln. Die Routinen werden in verschiedenen Teilen der View
 * eingesetzt, um Ausgaben zu formatieren, und besitzen keinerlei
 * interne Zustände oder komplexe Abhängigkeiten.
 */

//* -------------------------------------- Includes ---------------------------------------

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Strukturen --------------------------------------

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Wandelt ein Byte in eine achtstellige Bitfolge.
 * @param value Eingabewert
 * @param out   Ausgabe-Buffer mit Platz für 9 Zeichen
 */
void byte_to_bit_string(unsigned char value, char *out);

/**
 * @brief Wandelt ein Byte in eine zweistellige Hexzahl.
 * @param value Eingabewert
 * @param out   Ausgabe-Buffer mit Platz für 3 Zeichen
 */
void byte_to_hex_string(unsigned char value, char *out);

//* ----------------------------------- Init und Deinit -----------------------------------

#endif // UTILITIES_H
