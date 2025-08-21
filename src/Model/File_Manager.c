/**
 * @file File_Manager.c
 * @brief Dateioperationen für das Projekt.
 *
 * Dieses Modul enthält die Implementierungen der grundlegenden
 * Dateioperationen, die an mehreren Stellen des Programms benötigt
 * werden. Es abstrahiert das Öffnen, Lesen, Schreiben und Anhängen
 * von Dateien mithilfe der C-Standardbibliothek und liefert
 * einheitliche Rückgabewerte zur Fehlerbehandlung.
 */
//* -------------------------------------- Includes ---------------------------------------
#include "File_Manager.h"
#include <stdio.h>
#include <stdlib.h>

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Prüft, ob eine Datei existiert.
 * @param path Dateipfad
 * @return 1 wenn vorhanden, sonst 0
 */
int file_exists(const char *path) {
    FILE *file = fopen(path, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

/**
 * @brief Hängt Text an eine Datei an.
 * @param path Dateipfad
 * @param text anzuhängender Text
 * @return 0 bei Erfolg, sonst -1
 */
int appendLineToFile(const char *path, const char *text) {
    FILE *file = fopen(path, "a");
    if (!file) {
        return -1;
    }
    if (fputs(text, file) == EOF) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

/**
 * @brief Liest eine Datei komplett in einen Buffer.
 * @param path     Dateipfad
 * @param outBytes Ziel-Buffer
 * @param outSize  gelesene Größe
 * @return 0 bei Erfolg, sonst -1
 */
int readFileToBuffer(const char *path, unsigned char **outBytes, size_t *outSize) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        return -1; // Datei konnte nicht geöffnet werden
    }

    // Dateigröße ermitteln
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return -1; // Fehler beim Positionieren
    }
    long fileSize = ftell(file); // aktuelle Position entspricht Größe
    if (fileSize < 0) {
        fclose(file);
        return -1; // Größe konnte nicht bestimmt werden
    }
    rewind(file); // wieder an den Anfang setzen

    unsigned char *buffer = (unsigned char *)malloc(fileSize); // Speicher anfordern
    if (!buffer) {
        fclose(file);
        return -1; // kein Speicher
    }

    size_t readBytes = fread(buffer, 1, fileSize, file); // Datei lesen
    fclose(file);
    if (readBytes != (size_t)fileSize) {
        free(buffer); // unvollständig gelesen, Speicher freigeben
        return -1;
    }

    *outBytes = buffer;   // gelesene Bytes zurückgeben
    *outSize = readBytes; // gelesene Größe setzen
    return 0; // Erfolg
}

/**
 * @brief Schreibt einen Buffer in eine Datei.
 * @param path  Dateipfad
 * @param bytes Quell-Buffer
 * @param size  Anzahl der Bytes
 * @return 0 bei Erfolg, sonst -1
 */
int writeBufferToFile(const char *path, const unsigned char *bytes, size_t size) {
    FILE *file = fopen(path, "wb");
    if (!file) {
        return -1; // Datei konnte nicht geöffnet werden
    }
    size_t written = fwrite(bytes, 1, size, file); // Buffer in Datei schreiben
    fclose(file);
    if (written != size) {
        return -1; // nicht alle Bytes geschrieben
    }
    return 0; // Erfolg
}

//* ---------------------------------- Helfer Funktionen ----------------------------------

//* ----------------------------------- Init und Deinit -----------------------------------

