/**
 * @file main.c
 * @brief Einstiegspunkt der Anwendung.
 *
 * Diese Datei bündelt den Startvorgang des Programms. Sie wertet
 * Kommandozeilenargumente aus, fragt bei Bedarf einen gültigen
 * Dateipfad vom Nutzer ab und übergibt diesen an den
 * Hauptcontroller. Darüber hinaus verwaltet sie das geordnete
 * Herunterfahren der Oberfläche sowie das optionale Speichern von
 * Änderungen. Damit fungiert sie als zentraler Knotenpunkt, der
 * Model-, View- und Controller-Komponenten zusammenführt.
 */

//* -------------------------------------- Includes ---------------------------------------
#include <stdio.h>
#include <string.h>
#include "File_Manager.h"
#include "Main_Controller.h"

/**
 * @brief Fragt den Benutzer nach einem gültigen Pfad.
 */
static void request_valid_path(char *path, size_t size);

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Startet das Programm und verwaltet den Lebenszyklus.
 *
 * Das Programm erwartet optional einen Dateipfad. Ist dieser ungültig,
 * wird der Nutzer nach einem korrekten Pfad gefragt. Anschließend
 * wird der Controller initialisiert und ausgeführt.
 *
 * @param argc Anzahl der Argumente
 * @param argv Argumentfeld
 * @return 0 bei Erfolg, sonst 1
 */
int main(int argc, char *argv[]) {
    char file_path[256]; // Speicher für den Dateipfad

    // Pfad aus Argumenten verwenden oder vom Nutzer anfordern
    if (argc >= 2 && file_exists(argv[1])) {
        strncpy(file_path, argv[1], sizeof(file_path) - 1); // Argument kopieren
        file_path[sizeof(file_path) - 1] = '\0';            // String terminieren
    } else {
        request_valid_path(file_path, sizeof(file_path));   // gültigen Pfad anfordern
    }

    // Controller anlegen und initialisieren
    Main_Controller controller;
    if (main_controller_init(&controller, file_path) != 0) {
        return 1; // Start fehlgeschlagen
    }

    // Hauptschleife ausführen
    int run_result = main_controller_run(&controller);

    // Status des Buffers sichern
    int unsaved_changes = controller.buffer.edited;          // ungespeicherte Änderungen
    int ever_changed = controller.buffer.ever_changed;       // ob jemals geändert wurde
    char final_path[256];
    strncpy(final_path, controller.file_path, sizeof(final_path) - 1); // Pfad sichern
    final_path[sizeof(final_path) - 1] = '\0';

    // View herunterfahren
    main_window_deinit(&controller.view);

    // Offene Änderungen bei Bedarf speichern
    if (unsaved_changes) {
        printf("Would you like to save the file? Press Y to save or N to discard: ");
        int answer = getchar();       // erste Eingabe lesen
        getchar();                    // nachfolgendes Newline entfernen
        if (answer == 'y' || answer == 'Y') {
            if (data_buffer_save_file(&controller.buffer, final_path) == 0) {
                unsaved_changes = 0; // Änderungen gespeichert
            }
        }
    }

    // Buffer freigeben
    data_buffer_deinit(&controller.buffer);
    memset(&controller, 0, sizeof(controller));

    // Abschließenden Status der Datei ermitteln
    const char *final_text;
    if (!ever_changed) {
        final_text = "NOTHING changed";           // Datei blieb unverändert
    } else if (unsaved_changes) {
        final_text = "Last changes DISCARDED";    // Änderungen verworfen
    } else {
        final_text = "Last changes SAVED";        // Änderungen gespeichert
    }

    // Status melden
    printf("Final status %s: %s\n", final_path, final_text);

    if (run_result != 0) {
        return 1; // Fehlercode weiterreichen
    }

    return 0; // Erfolg
}

//* ---------------------------------- Helfer Funktionen ----------------------------------

/**
 * @brief Fragt den Nutzer wiederholt nach einem gültigen Dateipfad.
 */
static void request_valid_path(char *path, size_t size) {
    while (1) {
        printf("Falscher oder fehlender Pfad. Bitte eingeben: ");

        /* Eingabe lesen; bei Fehler erneut versuchen */
        if (!fgets(path, size, stdin)) {
            clearerr(stdin);               /* Fehlerzustand zurücksetzen */
            continue;                     /* neue Eingabe anfordern */
        }

        size_t length = strlen(path);     /* Länge des Eingabestrings */
        if (length > 0 && path[length - 1] == '\n') {
            path[length - 1] = '\0';     /* Newline entfernen */
        }

        if (file_exists(path)) {          /* Existenz prüfen */
            break;                        /* gültiger Pfad gefunden */
        }
    }
}

//* ----------------------------------- Init und Deinit -----------------------------------
