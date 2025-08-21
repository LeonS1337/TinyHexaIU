#ifndef DISPLAY_STRATEGY_H
#define DISPLAY_STRATEGY_H

/**
 * @file Display_Strategy.h
 * @brief Strategien für Hex- oder Binäranzeige.
 *
 * Dieser Header beschreibt das Strategiemuster zur Darstellung der
 * Bytewerte im Editor. Über einen Satz von Funktionszeigern werden
 * Parameter wie Bytes pro Zeile, Zellbreite und Formatierung
 * gekapselt. Dadurch kann der Editor flexibel zwischen hexadezimaler
 * und binärer Anzeige wechseln, ohne seine Kernlogik anpassen zu
 * müssen.
 */

#ifdef _WIN32
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif
#include <ctype.h>

struct Editor; /**< Vorwärtsdeklaration */
 
//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Strukturen --------------------------------------
/**
 * @brief Funktionszeiger für verschiedene Anzeigeparameter.
 */
typedef struct DisplayStrategy {
    int (*bytes_per_line)(void);              /**< Bytes pro Zeile */
    int (*cell_width)(void);                  /**< Zeichenbreite einer Zelle */
    int (*is_valid_char)(int ch);             /**< Prüft Eingabezeichen */
    void (*format_byte)(unsigned char byte, char *out); /**< Formatiert Byte */
    const char *(*header_label)(void);        /**< Überschrift */
    const char *(*empty_cell)(void);          /**< Leere Zelle */
    void (*reset_pending)(struct Editor *self); /**< Eingaben zurücksetzen */
} DisplayStrategy;

//* ------------------------------------- Funktionen --------------------------------------

//* ----------------------------------- Init und Deinit -----------------------------------

//* ---------------------------- Globale Konstanten oder Defines --------------------------
/** Hexadezimale Darstellung */
extern const DisplayStrategy HEX_STRATEGY;
/** Binäre Darstellung */
extern const DisplayStrategy BIN_STRATEGY;

#endif // DISPLAY_STRATEGY_H
