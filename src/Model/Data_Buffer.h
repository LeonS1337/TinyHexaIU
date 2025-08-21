#ifndef DATA_BUFFER_H // Schutz vor mehrfacher Einbindung
#define DATA_BUFFER_H // Start des Headerbereichs

/**
 * @file Data_Buffer.h
 * @brief Schnittstelle zum Daten-Buffer.
 *
 * Dieses Modul beschreibt die Datenhaltung der Anwendung. Es
 * speichert den Inhalt einer Datei im Speicher und verwaltet
 * Bearbeitungsflags, um geänderte oder unveränderte Zustände
 * zu erkennen. Die bereitgestellten Funktionen ermöglichen das
 * Laden, Lesen, Manipulieren und Speichern der Bytefolgen. Als
 * zentrales Element des Models wird der Buffer sowohl vom
 * Controller als auch von den View-Komponenten verwendet.
 */

#include <stddef.h> // stellt den Typ size_t bereit

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Strukturen --------------------------------------
/**
 * @brief Enthält geladene Datei und Bearbeitungsstatus.
 */
typedef struct {                                   // Struktur für den Daten-Buffer
    unsigned char *bytes; /**< Zeiger auf die gelesenen Bytes */
    size_t size;          /**< Größe der Datei in Bytes */
    int edited;           /**< Flag für ungespeicherte Änderungen */
    int ever_changed;     /**< Flag, ob die Datei jemals bearbeitet wurde */
} DataBuffer;                                      // Typname der Struktur

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Lädt eine Datei in den Buffer.
 */
int data_buffer_load_file(DataBuffer *self, const char *path); // Datei laden

/**
 * @brief Gibt ein Byte zurück.
 */
unsigned char data_buffer_get_byte(const DataBuffer *self, size_t index); // einzelnes Byte lesen

/**
 * @brief Setzt ein Byte an einer Position.
 */
void data_buffer_set_byte(DataBuffer *self, size_t index, unsigned char value); // Byte setzen

/**
 * @brief Kopiert einen Bereich in einen externen Buffer.
 */
void data_buffer_get_range(const DataBuffer *self, size_t start, size_t end,
                           unsigned char *dest); // Bereich kopieren

/**
 * @brief Speichert den Buffer in eine Datei.
 * @param self Zeiger auf den Buffer
 * @param path Dateipfad
 * @return 0 bei Erfolg
 */
int data_buffer_save_file(DataBuffer *self, const char *path); // Buffer speichern

//* ----------------------------------- Init und Deinit -----------------------------------
/**
 * @brief Initialisiert den Buffer.
 */
int data_buffer_init(DataBuffer *self); // Buffer initialisieren

/**
 * @brief Gibt Ressourcen frei.
 */
int data_buffer_deinit(DataBuffer *self); // Ressourcen freigeben

#endif // DATA_BUFFER_H // Ende des Headerbereichs
