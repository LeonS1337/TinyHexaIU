/**
 * @file Main_Window.c
 * @brief Verwaltung des Hauptfensters.
 *
 * Dieses Modul orchestriert alle Teilfenster der Benutzeroberfläche.
 * Es initialisiert das `ncurses`-Subsystem, prüft die Terminalgröße
 * und legt Top-Bar, Editor sowie Bottom-Bar an.
 */

#include "Main_Window.h"            // zugehöriger Header
#include <string.h>                  // für memset
#include "Err_Log.h"                // Fehlerprotokollierung
#include "Ncurses_Check.h"          // Makro für ncurses-Fehlerprüfung

/**
 * @brief Zeichnet alle Bereiche des Hauptfensters.
 */
void main_window_draw(Main_Window *self) {
    top_bar_draw(&self->top_bar);                                  // obere Leiste zeichnen
    bottom_bar_draw(&self->bottom_bar, &self->editor, self->editor.buffer); // untere Leiste
    editor_draw(&self->editor);                                    // Editor rendern
}

/**
 * @brief Aufräumfunktion für Fehlerfälle.
 */
static void view_cleanup(void) {
    NCURSES_CHECK(endwin());                                       // ncurses sauber beenden
}

/**
 * @brief Prüft die Terminalgröße.
 */
static void check_terminal_size(void) {
    int minWidth = 78;                                             // Minimalbreite
    int minHeight = 10;                                            // Minimalhöhe
    if (COLS < minWidth || LINES < minHeight) {                    // Größe prüfen
        fatal_error("main_window_init", "terminal size too small");
    }
}

/**
 * @brief Initialisiert das Hauptfenster.
 */
int main_window_init(Main_Window *self, DataBuffer *buffer) {
    memset(self, 0, sizeof(*self));                                // Struktur zurücksetzen
    if (!initscr()) {                                              // ncurses initialisieren
        fatal_error("main_window_init", "initscr");             // Fehlerbehandlung
    }
    err_log_set_cleanup(view_cleanup);                             // Cleanup bei Fehlern setzen
    NCURSES_CHECK(cbreak());                                       // Terminal in cbreak-Modus setzen
    NCURSES_CHECK(noecho());                                       // Eingaben nicht anzeigen
    NCURSES_CHECK(keypad(stdscr, TRUE));                           // Funktionstasten aktivieren
    NCURSES_CHECK(refresh());                                      // Darstellung aktualisieren
    check_terminal_size();                                         // Terminalgröße überprüfen

    int width = COLS;                                              // gesamte Terminalbreite
    int top_height = 3;                                            // Höhe der oberen Leiste
    int bottom_height = 3;                                         // Höhe der unteren Leiste
    int editor_height = LINES - top_height - bottom_height;        // verbleibende Höhe

    top_bar_init(&self->top_bar, width);                           // obere Leiste initialisieren
    editor_init(&self->editor, editor_height, width, top_height, buffer); // Editor einrichten
    bottom_bar_init(&self->bottom_bar, width, top_height + editor_height); // untere Leiste

    return 0;                                                      // Erfolg zurückgeben
}

/**
 * @brief Gibt alle Ressourcen frei.
 */
int main_window_deinit(Main_Window *self) {
    bottom_bar_deinit(&self->bottom_bar);                          // untere Leiste freigeben
    editor_deinit(&self->editor);                                  // Editor freigeben
    top_bar_deinit(&self->top_bar);                                // obere Leiste freigeben
    NCURSES_CHECK(endwin());                                       // ncurses sauber beenden
    err_log_set_cleanup(NULL);                                     // Cleanup zurücksetzen
    memset(self, 0, sizeof(*self));                                // Struktur löschen
    return 0;                                                      // Erfolg melden
}

