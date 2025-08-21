/**
 * @file Err_Log.c
 * @brief Einfache Fehlerprotokollierung.
 *
 * Dieses Modul implementiert einen minimalen Logger, der
 * Fehlermeldungen mit Zeitstempel in eine Datei schreibt. Darüber
 * hinaus verwaltet es eine globale `fatal_error`-Routine, die
 * optional eine zuvor registrierte Aufräumfunktion ausführt und das
 * Programm mit aussagekräftiger Meldung beendet. Als Abhängigkeiten
 * kommen der `File_Manager` für Dateizugriffe sowie die Standard-
 * Bibliotheken zum Einsatz.
 */

//* -------------------------------------- Includes ---------------------------------------
#include "Err_Log.h"
#include "File_Manager.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//* ---------------------------- Globale Konstanten oder Defines --------------------------
// Globale Ressourcen für fatale Fehler
static ErrLog global_log;
static int global_log_initialized;
static void (*cleanup_function)(void);

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Schreibt eine Nachricht mit Zeitstempel in die Logdatei.
 * @param self    Zeiger auf den Logger
 * @param message Meldungstext
 */
void err_log_write(ErrLog *self, const char *message) {
    char time_buffer[32];  // Speicher für den Zeitstempel
    char line_buffer[512]; // Speicher für komplette Logzeile

    // Zeit ermitteln und formatiert ausgeben
    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);
    if (!time_info) {
        return;
    }
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", time_info);

    // Zeile zusammenbauen und schreiben
    snprintf(line_buffer, sizeof(line_buffer), "[%s] %s\n", time_buffer, message);
    appendLineToFile(self->log_file_path, line_buffer);
}

/**
 * @brief Registriert eine Aufräumfunktion.
 * @param cleanup Funktion, die vor Programmende ausgeführt wird
 */
void err_log_set_cleanup(void (*cleanup)(void)) {
    cleanup_function = cleanup;
}

/**
 * @brief Protokolliert einen fatalen Fehler und beendet das Programm.
 *
 * @param context  Ort des Fehlers
 * @param details  optionale Detailbeschreibung
 */
void fatal_error(const char *context, const char *details) {
    // Logger bei Bedarf initialisieren
    if (!global_log_initialized) {
        err_log_init(&global_log);
        global_log_initialized = 1;
    }

    // Nachricht vorbereiten
    char message[256];
    if (details) {
        snprintf(message, sizeof(message), "%s: %s", context, details);
    } else {
        snprintf(message, sizeof(message), "%s", context);
    }

    // Fehler protokollieren
    err_log_write(&global_log, message);

    // Optionales Aufräumen
    if (cleanup_function) {
        cleanup_function();
    }

    // Meldung ausgeben und beenden
    fprintf(stderr, "Fatal error in %s: %s\n", context, details ? details : "");
    exit(EXIT_FAILURE);
}

//* ---------------------------------- Helfer Funktionen ----------------------------------

//* ----------------------------------- Init und Deinit -----------------------------------
/**
 * @brief Initialisiert den Logger mit Standardpfad.
 * @param self Zeiger auf den Logger
 * @return 0 bei Erfolg
 */
int err_log_init(ErrLog *self) {
    memset(self, 0, sizeof(*self));
    self->log_file_path = "log.txt"; // Standardpfad
    return 0;
}

/**
 * @brief Gibt den Logger frei.
 * @param self Zeiger auf den Logger
 * @return 0 bei Erfolg
 */
int err_log_deinit(ErrLog *self) {
    memset(self, 0, sizeof(*self));
    return 0;
}