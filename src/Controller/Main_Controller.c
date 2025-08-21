/**
 * @file Main_Controller.c
 * @brief Steuert die Hauptlogik der Anwendung.
 *
 * Diese Implementierung verarbeitet alle Benutzereingaben und
 * vermittelt zwischen `DataBuffer` (Model) und den Fensterklassen
 * im View-Bereich. Der Controller sorgt für Navigation, Umschalten
 * der Darstellung sowie Speichern oder Neuladen der Datei. Er
 * kapselt damit den gesamten Lebenszyklus der Bearbeitung und hält
 * die Interaktion der Module zusammen.
 */
//* -------------------------------------- Includes ---------------------------------------
#include "Main_Controller.h"
#include <stdio.h>
#include <string.h>
#include "Err_Log.h"

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Verarbeitet eine einzelne Benutzereingabe.
 *
 * Je nach Taste werden Aktionen wie Scrollen oder Speichern ausgeführt.
 *
 * @param self Zeiger auf den Controller
 * @param key  Eingelesene Taste
 */
void main_controller_handle_input(Main_Controller *self, int key) {
    // Berechnung der Seitengröße: Zeilen * Bytes pro Zeile
    int row_count = getmaxy(self->view.editor.win) - 3;       // nutzbare Zeilen ohne Rahmen und Kopfzeile
    int bytes_per_line = getBytesPerLine(&self->view.editor); // Bytes pro Zeile ermitteln
    size_t page = (size_t)row_count * (size_t)bytes_per_line; // Bytes, die eine Seite umfasst

    // Tastatureingabe auswerten und passende Aktion ausführen
    switch (key) {
    case KEY_UP: // Pfeiltaste nach oben
        // Cursor eine Zeile nach oben verschieben
        editor_move_cursor(&self->view.editor, -1, 0);
        // Angefangene Hex-Eingabe verwerfen
        self->view.editor.hex_pending = -1;
        break;
    case KEY_DOWN: // Pfeiltaste nach unten
        // Cursor eine Zeile nach unten bewegen
        editor_move_cursor(&self->view.editor, 1, 0);
        self->view.editor.hex_pending = -1; // angefangene Eingabe zurücksetzen
        break;
    case KEY_LEFT: // Pfeiltaste nach links
        // Cursor eine Spalte nach links
        editor_move_cursor(&self->view.editor, 0, -1);
        self->view.editor.hex_pending = -1;
        break;
    case KEY_RIGHT: // Pfeiltaste nach rechts
        // Cursor eine Spalte nach rechts
        editor_move_cursor(&self->view.editor, 0, 1);
        self->view.editor.hex_pending = -1;
        break;
    case '\t': // Tabulator wechselt Hex/ASCII Bereich
        editor_toggle_area(&self->view.editor); // sichtbaren Bereich umschalten
        self->view.editor.hex_pending = -1;
        break;
    case KEY_NPAGE: // Bild nach unten
        // Eine Seite weiter scrollen
        editor_scroll(&self->view.editor, (int)page);
        self->view.editor.hex_pending = -1;
        break;
    case KEY_PPAGE: // Bild nach oben
        // Eine Seite zurück scrollen
        editor_scroll(&self->view.editor, -(int)page);
        self->view.editor.hex_pending = -1;
        break;
    case KEY_F(1): // Anzeige zwischen Hex und Binär umschalten
        editor_toggle_display_mode(&self->view.editor);      // Modus wechseln
        main_window_draw(&self->view);                       // Ansicht neu zeichnen
        break;
    case KEY_F(2): // Datei speichern
        // Änderungen auf Datenträger schreiben, Fehler sind fatal
        if (data_buffer_save_file(&self->buffer, self->file_path) == 0) {
            printf("File %s saved\n", self->file_path);
        }
        break;
    case KEY_F(3): // Änderungen verwerfen und neu laden
        // Datei erneut laden und Editor zurücksetzen
        data_buffer_load_file(&self->buffer, self->file_path);
        editor_reset(&self->view.editor);                    // Editor-Ansicht zurücksetzen
        break;
    default: // reguläre Zeicheneingabe
        editor_handle_char(&self->view.editor, key);
        break;
    }
}

/**
 * @brief Startet die Hauptschleife des Controllers.
 *
 * @param self Zeiger auf den Controller
 * @return 0 bei normalem Ende
 */
int main_controller_run(Main_Controller *self) {
    // Aktuelle Datei im Terminal anzeigen
    printf("TinyHexa is running file: %s\n", self->file_path);

    // Anfangszustand der Oberfläche zeichnen
    main_window_draw(&self->view);

    // Hauptschleife: Eingaben lesen bis F12 gedrückt wird
    int input_key;
    while ((input_key = wgetch(self->view.editor.win)) != KEY_F(12)) {
        main_controller_handle_input(self, input_key); // Eingabe verarbeiten
        main_window_draw(&self->view);                 // Anzeige aktualisieren
    }

    return 0; // reguläres Ende
}

//* ---------------------------------- Helfer Funktionen ----------------------------------

//* ----------------------------------- Init und Deinit -----------------------------------
/**
 * @brief Initialisiert den Controller und lädt eine Datei.
 *
 * @param self      Zeiger auf den Controller
 * @param file_path Pfad zur Datei
 * @return 0 bei Erfolg
 */
int main_controller_init(Main_Controller *self, const char *file_path) {
    // Struktur mit Nullen füllen, um definierte Ausgangswerte zu haben
    memset(self, 0, sizeof(*self));

    // Übergebenen Dateipfad kopieren und sicher terminieren
    strncpy(self->file_path, file_path, sizeof(self->file_path) - 1);
    self->file_path[sizeof(self->file_path) - 1] = '\0';

    // Daten-Buffer initialisieren und Fehler protokollieren
    if (data_buffer_init(&self->buffer) != 0) {
        fatal_error("main_controller_init", "data_buffer_init");
    }

    // Datei in den Buffer laden, bei Fehler wird beendet
    data_buffer_load_file(&self->buffer, self->file_path);

    // grafische Oberfläche starten
    if (main_window_init(&self->view, &self->buffer) != 0) {
        fatal_error("main_controller_init", "main_window_init");
    }

    return 0; // Erfolg
}

/**
 * @brief Gibt alle Ressourcen des Controllers frei.
 *
 * @param self Zeiger auf den Controller
 * @return 0 bei Erfolg
 */
int main_controller_deinit(Main_Controller *self) {
    // Oberfläche schließen und Ressourcen freigeben
    main_window_deinit(&self->view);

    // Buffer freigeben
    data_buffer_deinit(&self->buffer);

    // Controller-Struktur säubern
    memset(self, 0, sizeof(*self));
    return 0;
}

