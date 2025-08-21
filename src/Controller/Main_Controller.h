#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

/**
 * @file Main_Controller.h
 * @brief Schnittstelle des Hauptcontrollers.
 *
 * Diese Datei definiert den zentralen Controller des Programms.
 * Er verbindet das Model, repräsentiert durch den `DataBuffer`, mit
 * den Darstellungskomponenten im `Main_Window`. Über die hier
 * deklarierten Funktionen werden Benutzereingaben verarbeitet, die
 * Hauptschleife gestartet und sämtliche Ressourcen verwaltet. Die
 * Schnittstelle wird vom Einstiegspunkt `main.c` genutzt, um den
 * Lebenszyklus der Anwendung zu steuern.
 */

#include "Data_Buffer.h"
#include "Main_Window.h"

//* ---------------------------- Globale Konstanten oder Defines --------------------------

//* ------------------------------------- Strukturen --------------------------------------
/**
 * @brief Bündelt alle Elemente des Controllers.
 */
typedef struct {
    char file_path[256]; /**< Pfad der zu bearbeitenden Datei */
    DataBuffer buffer;   /**< Eingelesene Dateidaten */
    Main_Window view;    /**< Hauptfenster der Anwendung */
} Main_Controller;

//* ------------------------------------- Funktionen --------------------------------------
/**
 * @brief Startet die Hauptschleife.
 * @param self Zeiger auf den Controller
 */
int main_controller_run(Main_Controller *self);

/**
 * @brief Verarbeitet eine Benutzereingabe.
 * @param self Zeiger auf den Controller
 * @param key  Eingelesene Taste
 */
void main_controller_handle_input(Main_Controller *self, int key);

//* ----------------------------------- Init und Deinit -----------------------------------
/**
 * @brief Initialisiert den Controller.
 * @param self      Zeiger auf den Controller
 * @param file_path Pfad zur Datei
 * @return 0 bei Erfolg, sonst -1
 */
int main_controller_init(Main_Controller *self, const char *file_path);

/**
 * @brief Gibt Ressourcen frei.
 * @param self Zeiger auf den Controller
 * @return 0 bei Erfolg
 */
int main_controller_deinit(Main_Controller *self);

#endif // MAIN_CONTROLLER_H
