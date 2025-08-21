#ifndef EDITOR_H
#define EDITOR_H

/**
 * @file Editor.h
 * @brief Schnittstelle des Editorfensters.
 *
 * Dieser Header definiert die Struktur und die öffentlichen
 * Funktionen des Editorbereichs. Der Editor ist verantwortlich für
 * die Darstellung und Navigation innerhalb der geladenen Daten und
 * nutzt Strategien, um zwischen hexadezimaler und binärer Ansicht zu
 * wechseln. Er interagiert mit dem `DataBuffer`, ohne dessen
 * Lebenszyklus selbst zu verwalten.
 */

//* -------------------------------------- Includes ---------------------------------------
#ifdef _WIN32
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif
#include "Data_Buffer.h"
#include "Display_Strategy.h"

//* ---------------------------- Globale Konstanten oder Defines --------------------------
/** Breiten und Startpositionen */
#define OFFSET_COL_WIDTH 8                     /**< Breite der Offset-Spalte */
#define HEX_COL_WIDTH 3                        /**< Breite einer Hex-Zelle */
#define BIN_COL_WIDTH 9                        /**< Breite einer Binär-Zelle */
#define HEX_START_X (1 + OFFSET_COL_WIDTH + 1) /**< Start des Hex-Bereichs */

//* ------------------------------------- Strukturen --------------------------------------
/** Bereich für den Cursor */
typedef enum {
    AREA_HEX,   /**< linker Hex-Bereich */
    AREA_ASCII  /**< rechter ASCII-Bereich */
} CursorArea;

/** Anzeigearten */
typedef enum {
    DISPLAY_HEX, /**< Hexadezimal */
    DISPLAY_BIN  /**< Binär */
} DisplayMode;

/**
 * @brief Struktur für den Editor.
 */
typedef struct Editor {
    WINDOW *win;                        /**< Fenster für den Editor */
    DataBuffer *buffer;                 /**< Zeiger auf die Daten */
    size_t start_offset;                /**< Startoffset der Ansicht */
    int cursor_y;                       /**< Cursorposition Zeile */
    int cursor_x;                       /**< Cursorposition Spalte */
    CursorArea cursor_area;             /**< Aktiver Bereich */
    int hex_pending;                    /**< Offener Hex-Nibble */
    int bin_pending_bits;               /**< Anzahl eingegebener Binärbits */
    unsigned char bin_pending_value;    /**< Zwischenspeicher für Binärwerte */
    DisplayMode display_mode;           /**< Aktueller Anzeigemodus */
    const struct DisplayStrategy *strategy; /**< Strategie für Darstellung */
} Editor;

//* ------------------------------------- Funktionen --------------------------------------
/** Zeichnet den Inhalt des Editors. */
void editor_draw(Editor *self);

/** Bewegt den Cursor relativ. */
void editor_move_cursor(Editor *self, int delta_y, int delta_x);

/** Wechselt zwischen Hex- und ASCII-Bereich. */
void editor_toggle_area(Editor *self);

/** Wechselt zwischen Hex- und Binärdarstellung. */
void editor_toggle_display_mode(Editor *self);

/** Scrollt die Ansicht um den Offset. */
void editor_scroll(Editor *self, int delta_offset);

/** Setzt den Editor auf die Anfangsposition. */
void editor_reset(Editor *self);

/** Verarbeitet Zeichen und aktualisiert den Buffer. */
void editor_handle_char(Editor *self, int key);

/* Inline-Hilfsfunktionen -------------------------------------------------- */

/**
 * @brief Ermittelt die Breite einer Zelle.
 */
static inline int getCellWidth(const Editor *self) {
    return self->strategy->cell_width();
}

/**
 * @brief Gibt Bytes pro Zeile zurück.
 */
static inline int getBytesPerLine(const Editor *self) {
    return self->strategy->bytes_per_line();
}

/**
 * @brief Berechnet die Startposition des ASCII-Bereichs.
 */
static inline int getAsciiStartX(const Editor *self) {
    int bytesPerLine = getBytesPerLine(self);
    return HEX_START_X + bytesPerLine * getCellWidth(self) + 1;
}

/**
 * @brief Berechnet den absoluten Byte-Index.
 */
static inline size_t idx(const Editor *self, int row, int col) {
    return self->start_offset + (size_t)row * (size_t)getBytesPerLine(self) + (size_t)col;
}

//* ----------------------------------- Init und Deinit -----------------------------------
/** Initialisiert den Editor. */
int editor_init(Editor *self, int height, int width, int starty, DataBuffer *buffer);

/** Gibt Ressourcen frei. */
int editor_deinit(Editor *self);

#endif // EDITOR_H
