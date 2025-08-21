#ifndef NCURSES_CHECK_H
#define NCURSES_CHECK_H

/**
 * @file Ncurses_Check.h
 * @brief Fehlerprüfung für ncurses-Aufrufe.
 *
 * Dieses Makro vereinfacht die Fehlerbehandlung. Jeder ncurses-Aufruf
 * wird geprüft und im Fehlerfall mit einer aussagekräftigen Meldung
 * beendet. So bleibt der Code übersichtlich und Fehler werden sofort
 * erkannt.
 */

#include "Err_Log.h" // stellt fatal_error bereit

/**
 * @brief Führt den ncurses-Aufruf aus und prüft das Ergebnis.
 *
 * Bei einem Rückgabewert von ERR wird der aktuelle Funktionsname sowie
 * der fehlerhafte Aufruf ausgegeben und das Programm kontrolliert beendet.
 */
#define NCURSES_CHECK(call) \
    do { \
        if ((call) == ERR) { \
            fatal_error(__func__, #call); \
        } \
    } while (0)

#endif // NCURSES_CHECK_H
