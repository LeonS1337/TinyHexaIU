/**
 * @file Data_Buffer.c
 * @brief Verwaltung des Daten-Buffers.
 *
 * Dieses Modul implementiert die Funktionen zur Speicherung und
 * Manipulation von Dateiinhalt im Arbeitsspeicher. Es bietet
 * Routinen zum Laden von Bytes, zum Ändern einzelner Werte sowie zum
 * Speichern auf den Datenträger. Gleichzeitig überwacht es den
 * Bearbeitungsstatus, sodass andere Komponenten erkennen können,
 * ob Änderungen vorgenommen wurden. Fehlerfälle werden über den
 * `Err_Log` dokumentiert.
 */
//* -------------------------------------- Includes ---------------------------------------
#include "Data_Buffer.h"   // Schnittstelle dieses Moduls
#include <stdlib.h>         // Speicherfunktionen wie malloc und free
#include <string.h>         // Funktionen für Speicheroperationen
#include "File_Manager.h"  // Lesen und Schreiben von Dateien
#include "Err_Log.h"       // Protokollierung von Fehlern

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Lädt eine Datei in den Buffer.
 * @param self Zeiger auf den Buffer
 * @param path Dateipfad
 * @return 0 bei Erfolg
 */
int data_buffer_load_file(DataBuffer *self, const char *path) { // Datei in Buffer laden
    unsigned char *temp_bytes = NULL; // temporärer Zeiger für gelesene Daten
    size_t temp_size = 0;             // Größe der gelesenen Datei

    // Datei über den File Manager laden
    if (readFileToBuffer(path, &temp_bytes, &temp_size) != 0) { // Prüfen, ob Lesen klappt
        // Bei Fehler Programmabbruch und Logeintrag
        fatal_error("data_buffer_load_file", path); // Fehler melden und Programm beenden
    }

    // Vorherigen Inhalt freigeben und neue Daten übernehmen
    data_buffer_deinit(self); // alten Buffer leeren
    self->bytes = temp_bytes; // neuen Zeiger übernehmen
    self->size = temp_size;   // neue Größe setzen
    self->edited = 0;         // Bearbeitungsstatus zurücksetzen
    self->ever_changed = 0;   // bisherige Änderungen zurücksetzen
    return 0; // Erfolg melden
}

/**
 * @brief Liefert ein Byte aus dem Buffer.
 * @param self  Zeiger auf den Buffer
 * @param index Position des Bytes
 * @return Wert des Bytes oder 0 bei Fehler
 */
unsigned char data_buffer_get_byte(const DataBuffer *self, size_t index) {
    if (!self->bytes || index >= self->size) { // ungültigen Zugriff erkennen
        return 0;                              // Fehlerfall liefert 0
    }
    return self->bytes[index]; // angefordertes Byte zurückgeben
}

/**
 * @brief Schreibt ein Byte an eine Position.
 * @param self  Zeiger auf den Buffer
 * @param index Zielposition
 * @param value neuer Wert
 */
void data_buffer_set_byte(DataBuffer *self, size_t index, unsigned char value) {
    if (!self->bytes || index >= self->size) { // prüfen, ob der Zugriff zulässig ist
        return;                               // bei Fehler früh beenden
    }
    // Nur reagieren, wenn sich der Wert wirklich ändert
    if (self->bytes[index] == value) { // prüfen, ob neuer Wert identisch ist
        return;                        // keine Änderung nötig
    }
    self->bytes[index] = value; // neues Byte schreiben
    self->edited = 1;           // Buffer als geändert markieren
    self->ever_changed = 1;     // Merken, dass jemals etwas geändert wurde
}

/**
 * @brief Kopiert einen Bereich in einen externen Buffer.
 * @param self  Zeiger auf den Buffer
 * @param start Startindex
 * @param end   Endindex (exklusiv)
 * @param dest  Ziel-Buffer
 */
void data_buffer_get_range(const DataBuffer *self, size_t start, size_t end,
                           unsigned char *dest) {
    if (!self->bytes || start >= self->size || end > self->size || start >= end) { // Bereichsgrenzen prüfen
        return; // ungültiger Bereich
    }
    memcpy(dest, self->bytes + start, end - start); // Bereich kopieren
}

/**
 * @brief Speichert den Buffer in eine Datei.
 * @param self Zeiger auf den Buffer
 * @param path Dateipfad
 * @return 0 bei Erfolg
 */
int data_buffer_save_file(DataBuffer *self, const char *path) { // Buffer auf Platte sichern
    if (!self->bytes) { // prüfen, ob überhaupt Daten vorhanden sind
        // Kein Inhalt vorhanden, wird als schwerwiegender Fehler behandelt
        fatal_error("data_buffer_save_file", "no bytes"); // Hinweis auf fehlenden Inhalt
    }
    int result = writeBufferToFile(path, self->bytes, self->size); // auf Platte schreiben
    if (result != 0) { // prüfen, ob Schreiben fehlgeschlagen ist
        // Fehler protokollieren und Programm beenden
        fatal_error("data_buffer_save_file", path); // Fehler beim Schreiben
    }
    self->edited = 0; // Änderungen als gespeichert markieren
    return 0;         // Erfolg melden
}

//* ---------------------------------- Helfer Funktionen ----------------------------------

//* ----------------------------------- Init und Deinit -----------------------------------
/**
 * @brief Initialisiert den Buffer.
 * @param self Zeiger auf den Buffer
 * @return 0 bei Erfolg
 */
int data_buffer_init(DataBuffer *self) { // Struktur vorbereiten
    memset(self, 0, sizeof(*self)); // Struktur auf Null setzen
    self->edited = 0;               // keine Änderungen
    self->ever_changed = 0;         // bisher nichts verändert
    return 0;                       // Erfolg melden
}

/**
 * @brief Gibt den Buffer frei.
 * @param self Zeiger auf den Buffer
 * @return 0 bei Erfolg
 */
int data_buffer_deinit(DataBuffer *self) { // Ressourcen freigeben
    free(self->bytes);              // gespeicherte Bytes freigeben
    memset(self, 0, sizeof(*self)); // Struktur zurücksetzen
    return 0;                      // Erfolg melden
}


