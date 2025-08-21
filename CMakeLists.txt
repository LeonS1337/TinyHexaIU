# Mindestversion von CMake festlegen
cmake_minimum_required(VERSION 3.15)

# Projektname und Sprache
# Das Programm soll als "TinyHexa" erzeugt werden
project(TinyHexa VERSION 1.0.0 LANGUAGES C)

# C-Standard festlegen (C17 für moderne Sprachfunktionen)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Unterverzeichnisse einbinden
# add_subdirectory(libs)      # um die libs dort zu organisieren
add_subdirectory(src)       # Quellcode
# add_subdirectory(tests)   # Tests

# Hauptprogramm erstellen 
# --> Siehe CMakeLists.txt in src/

# Header-Dateien einbinden
# --> Siehe CMakeLists.txt in libs/