# src/CMakeLists.txt

# Quellcodedateien
set(SOURCES
    main.c
    Controller/Err_Log.c
    Controller/Main_Controller.c
    Controller/Utilities.c
    Model/Data_Buffer.c
    Model/File_Manager.c
    View/Bottom_Bar.c
    View/Editor.c
    View/editor_draw.c
    View/editor_input.c
    View/Main_Window.c
    View/Top_Bar.c
    View/Display_Strategy.c
)

# Executable anlegen
add_executable(TinyHexa ${SOURCES})

# Include-Verzeichnisse
target_include_directories(TinyHexa PRIVATE
    ${PROJECT_SOURCE_DIR}/src/Model
    ${PROJECT_SOURCE_DIR}/src/View
    ${PROJECT_SOURCE_DIR}/src/Controller
)

# Statische Kompilierung aktivieren
target_compile_definitions(TinyHexa PRIVATE NCURSES_STATIC)

# Win32-spezifisches statisches Linken für z.B. vscode
if (WIN32)
    # Erzwinge statisches Linken der GCC-Runtimes
    set_target_properties(TinyHexa PROPERTIES
        LINK_FLAGS "-static -static-libgcc -static-libstdc++"
    )
    target_link_libraries(TinyHexa ncurses)

# MSYS2-spezifisches statisches Linken
elseif (MSYS)
    message(STATUS ">> MSYS2 erkannt – statisches Linken wird erzwungen")

    # Bibliotheken statisch linken
    target_link_libraries(TinyHexa PRIVATE
        /ucrt64/lib/libncurses.a
        /ucrt64/lib/libwinpthread.a
    )

    # Linkoptionen setzen – wichtig: NICHT als Bibliothek behandeln!
    target_link_options(TinyHexa PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
        -Wl,-Bstatic
        -Wl,-Bdynamic
    )

    # Nur statisch suchen
    set_target_properties(TinyHexa PROPERTIES
        LINK_SEARCH_START_STATIC TRUE
        LINK_SEARCH_END_STATIC TRUE
    )
else()
    # Normales Linken für Linux
    target_link_libraries(TinyHexa PRIVATE ncurses)
endif()