#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

/**
 * @file File_Manager.h
 * @brief Deklaration grundlegender Dateioperationen.
 *
 * Dieses Modul stellt einfache Funktionen für den Zugriff auf das
 * Dateisystem bereit. Dazu zählen das Prüfen der Existenz, das
 * Anhängen von Text sowie das Lesen und Schreiben ganzer Dateien in
 * Buffer. Die Routinen werden sowohl vom `Data_Buffer` als auch vom
 * Fehlerlogger genutzt und sind bewusst zustandslos implementiert,
 * um eine einfache Wiederverwendung zu ermöglichen.
 */

#include <stddef.h>

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Strukturen --------------------------------------

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Prüft, ob eine Datei existiert.
 */
int file_exists(const char *path);

/**
 * @brief Hängt Text an das Ende einer Datei an.
 */
int appendLineToFile(const char *path, const char *text);

/**
 * @brief Liest eine Datei in einen Buffer.
 */
int readFileToBuffer(const char *path, unsigned char **outBytes, size_t *outSize);

/**
 * @brief Schreibt einen Buffer in eine Datei.
 */
int writeBufferToFile(const char *path, const unsigned char *bytes, size_t size);

//* ----------------------------------- Init und Deinit -----------------------------------

#endif // FILE_MANAGER_H
