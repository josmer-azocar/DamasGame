#ifndef CONSOLE_VIEW_H
#define CONSOLE_VIEW_H

#include <windows.h> // Para la API de Windows (colores, gotoxy)
#include <string>
#include <vector>    // Para menús futuros
#include <iostream>  // Para std::ostream (aunque no se use directamente en .h)

// Declaración anticipada para evitar include circular si Board necesitara ConsoleView
class Board; // Necesitamos declarar Board porque DisplayBoard la usa
// struct GameStats; // Si tuvieras un parámetro GameStats en DisplayBoard

// --- CONSTANTES DE COLOR (Van aquí porque son para la vista) ---
const int CONSOLE_COLOR_BLACK = 0;
const int CONSOLE_COLOR_BLUE = 1;
const int CONSOLE_COLOR_GREEN = 2;
const int CONSOLE_COLOR_CYAN = 3;
const int CONSOLE_COLOR_RED = 4;
const int CONSOLE_COLOR_MAGENTA = 5;
const int CONSOLE_COLOR_BROWN = 6;
const int CONSOLE_COLOR_LIGHT_GRAY = 7;
const int CONSOLE_COLOR_DARK_GRAY = 8;
const int CONSOLE_COLOR_LIGHT_BLUE = 9;
const int CONSOLE_COLOR_LIGHT_GREEN = 10;
const int CONSOLE_COLOR_LIGHT_CYAN = 11;
const int CONSOLE_COLOR_LIGHT_RED = 12;
const int CONSOLE_COLOR_LIGHT_MAGENTA = 13;
const int CONSOLE_COLOR_YELLOW = 14;
const int CONSOLE_COLOR_WHITE = 15;

// --- FUNCIONES DE UTILIDAD DE CONSOLA ---
inline void SetConsoleTextColor(int foregroundColor, int backgroundColor = CONSOLE_COLOR_BLACK) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, foregroundColor | (backgroundColor << 4));
}

inline void ResetConsoleTextColor() {
    SetConsoleTextColor(CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
}

inline void GoToXY(int x, int y) {
    COORD cursorPosition;
    cursorPosition.X = static_cast<SHORT>(x);
    cursorPosition.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

// --- Declaración de la Clase ConsoleView ---
class ConsoleView {
public:
    ConsoleView(); // Constructor

    void DisplayBoard(const Board& gameBoard) const; // Dibuja el tablero
    void DisplayMessage(const std::string& message, bool newLine = true) const;
    void ClearScreen() const;
    // void EnableVTModeIfNeeded() const; // Si decidimos usar ANSI más adelante
};

#endif // CONSOLE_VIEW_H