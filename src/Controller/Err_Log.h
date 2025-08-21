#ifndef ERR_LOG_H
#define ERR_LOG_H

/**
 * @file Err_Log.h
 * @brief Schnittstelle für das Fehlerlog.
 *
 * Diese Header-Datei deklariert einen einfachen Logger, der
 * Fehlermeldungen mit Zeitstempel in eine Textdatei schreibt.
 * Zusätzlich können Module eine Aufräumfunktion registrieren, die
 * bei fatalen Fehlern automatisch aufgerufen wird. Die hier
 * bereitgestellte `fatal_error`-Funktion wird controllerweit genutzt,
 * um bei schweren Fehlern einen geordneten Programmabbruch
 * sicherzustellen.
 */

//* -------------------------------------- Includes ---------------------------------------

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Strukturen --------------------------------------
/**
 * @brief Struktur zur Speicherung des Logdateipfads.
 */
typedef struct {
    const char *log_file_path; /**< Pfad zur Logdatei */
} ErrLog;

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Schreibt eine Fehlermeldung.
 * @param self    Zeiger auf den Logger
 * @param message Meldungstext
 */
void err_log_write(ErrLog *self, const char *message);

/**
 * @brief Merkt sich eine Aufräumfunktion.
 * @param cleanup Funktion, die vor Programmende ausgeführt wird
 */
void err_log_set_cleanup(void (*cleanup)(void));

/**
 * @brief Behandelt einen fatalen Fehler und beendet das Programm.
 * @param context Ort des Fehlers
 * @param details optionale Details
 */
void fatal_error(const char *context, const char *details);

//* ----------------------------------- Init und Deinit -----------------------------------
/**
 * @brief Initialisiert den Logger.
 * @param self Zeiger auf den Logger
 * @return 0 bei Erfolg
 */
int err_log_init(ErrLog *self);

/**
 * @brief Gibt den Logger frei.
 * @param self Zeiger auf den Logger
 * @return 0 bei Erfolg
 */
int err_log_deinit(ErrLog *self);

#endif // ERR_LOG_H

