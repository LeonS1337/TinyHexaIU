# TinyHexa

## Kompilieren mit CMake

Für reproduzierbare Builds unter Linux und Windows eignet sich CMake besonders gut. Die ncurses-Bibliothek wird unter windows durch ncurses statisch gelinkt, sodass sie vollständig in die ausführbare Datei eingebettet ist. Auch ein Build unter Windows – mit Visual Studio Code, dem UCRT64-GCC-Compiler und der CMake-Erweiterung ist möglich.

### Unter Windows (getestet auf Windows 11 24H2)

Installation MSYS2  
`https://github.com/msys2/msys2-installer/releases/download/2024-12-08/msys2-x86_64-20241208.exe`

Dem Installationsassistenten folgen, anschließend MSYS2 starten.

In MSYS2 die mingw-64 Toolchain installieren.

```bash
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
```

In das Verzeichnis `C:\msys2\` wechseln und `ucrt64.exe` starten.

```bash
pacman -S cmake
pacman -S ninja
```

Ncurses ist bereits Teil der Toolchain und benötigt keine zusätzliche Installation.

Zum Kompilieren:

```bash
cd /c/TinyHexaProjektFolder/
cmake -G Ninja CMakeLists.txt
ninja
```

Das erzeugte Binary liegt danach unter `\src\TinyHexa.exe`.

### Unter Linux (getestet auf Debian 13.0.0)

Installation der Build-Tools (GCC, Make etc.)

```bash
sudo apt update
sudo apt install -y build-essential
```

Installation von CMake und Ninja

```bash
sudo apt install -y cmake
sudo apt install -y ninja-build
```

Installation der ncurses-Bibliothek

```bash
sudo apt install -y libncurses-dev
```

CMake mit Ninja als Build-System verwenden

```bash
cmake -G Ninja CMakeLists.txt
```

Build-Prozess mit Ninja starten

```bash
ninja
```

Das erzeugte Binary liegt danach unter `/src/TinyHexa`.

## Ausführung

Unter Linux

```bash
./tinyhexa <Datei>
```

Unter Windows

```bash
.\tinyhexa.exe <Datei>
```

Wird keine Datei angegeben, fragt TinyHexa interaktiv nach einem Pfad.

## Kurzanleitung zum Testen

1. Mit einer Beispiel-Datei starten, z.B. `./src/tinyhexa Makefile`.
2. Navigation per Pfeiltasten, Wechsel zwischen Daten- und ASCII-Spalte mit `Tab`.
3. `F1` schaltet zwischen Hex- und Binärdarstellung um.
4. Änderungen mit `F2` speichern, `F3` lädt die Datei erneut.
5. Beenden über `F12`.
