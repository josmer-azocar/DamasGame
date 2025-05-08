#pragma once
#include <windows.h> // Asegúrate que estos includes estén
#include <iostream>
#include <string>

// --- DEFINICIONES DE CONSTANTES DE COLOR (Color Constant Definitions) ---
const int CONSOLE_COLOR_BLACK = 0;  // NEGRO
const int CONSOLE_COLOR_BLUE = 1;  // AZUL
const int CONSOLE_COLOR_GREEN = 2;  // VERDE
const int CONSOLE_COLOR_CYAN = 3;  // CIAN
const int CONSOLE_COLOR_RED = 4;  // ROJO
const int CONSOLE_COLOR_MAGENTA = 5;  // MAGENTA
const int CONSOLE_COLOR_BROWN = 6;  // MARRÓN (en consola suele verse como amarillo oscuro)
const int CONSOLE_COLOR_LIGHT_GRAY = 7;  // GRIS_CLARO  <--- ¡Esta es la que faltaba!
const int CONSOLE_COLOR_DARK_GRAY = 8;  // GRIS_OSCURO
const int CONSOLE_COLOR_LIGHT_BLUE = 9;  // AZUL_CLARO
const int CONSOLE_COLOR_LIGHT_GREEN = 10; // VERDE_CLARO
const int CONSOLE_COLOR_LIGHT_CYAN = 11; // CIAN_CLARO
const int CONSOLE_COLOR_LIGHT_RED = 12; // ROJO_CLARO
const int CONSOLE_COLOR_LIGHT_MAGENTA = 13; // MAGENTA_CLARO
const int CONSOLE_COLOR_YELLOW = 14; // AMARILLO
const int CONSOLE_COLOR_WHITE = 15; // BLANCO

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

// --- AQUÍ VENDRÍA LA DECLARACIÓN DE LA CLASE CONSOLEVIEW ---
// class ConsoleView { ... };