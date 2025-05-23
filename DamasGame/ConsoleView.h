// Vista de consola para el juego de Damas
// Esta clase y utilidades permiten mostrar el tablero, menus y mensajes en la consola de Windows
// Los comentarios no contienen tildes para evitar problemas de codificacion

#ifndef CONSOLE_VIEW_H
#define CONSOLE_VIEW_H

#include <windows.h>
#include <string>
#include <vector>
#include <iostream>

class Board; // Declaracion adelantada de la clase Board
class LocalizationManager; // Declaracion adelantada para manejo de textos multilenguaje

// --- CONSTANTES DE COLOR ---
// Definicion de constantes para los colores de la consola
const int CONSOLE_COLOR_BLACK = 0;           // Negro
const int CONSOLE_COLOR_BLUE = 1;            // Azul
const int CONSOLE_COLOR_GREEN = 2;           // Verde
const int CONSOLE_COLOR_CYAN = 3;            // Cyan
const int CONSOLE_COLOR_RED = 4;             // Rojo
const int CONSOLE_COLOR_MAGENTA = 5;         // Magenta
const int CONSOLE_COLOR_BROWN = 6;           // Marron
const int CONSOLE_COLOR_LIGHT_GRAY = 7;      // Gris claro
const int CONSOLE_COLOR_DARK_GRAY = 8;       // Gris oscuro
const int CONSOLE_COLOR_LIGHT_BLUE = 9;      // Azul claro
const int CONSOLE_COLOR_LIGHT_GREEN = 10;    // Verde claro
const int CONSOLE_COLOR_LIGHT_CYAN = 11;     // Cyan claro
const int CONSOLE_COLOR_LIGHT_RED = 12;      // Rojo claro
const int CONSOLE_COLOR_LIGHT_MAGENTA = 13;  // Magenta claro
const int CONSOLE_COLOR_YELLOW = 14;         // Amarillo
const int CONSOLE_COLOR_WHITE = 15;          // Blanco

// --- FUNCIONES DE UTILIDAD DE CONSOLA ---
// Mueve el cursor de la consola a la posicion (x, y)
inline void GoToXY(int x, int y) {
	COORD cursorPosition;
	cursorPosition.X = static_cast<SHORT>(x);
	cursorPosition.Y = static_cast<SHORT>(y);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

// Cambia el color del texto y fondo de la consola
// foregroundColor: color del texto
// backgroundColor: color de fondo
// Si se desea el fondo actual, se debe pasar explicitamente
inline void SetConsoleTextColor(int foregroundColor, int backgroundColor) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, foregroundColor | (backgroundColor << 4));
}

// Restaura los colores por defecto del juego (texto gris claro, fondo negro)
inline void ResetConsoleColorsToGameDefault() {
	SetConsoleTextColor(CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
}

// Restaura los colores por defecto del menu (texto blanco, fondo magenta)
inline void ResetConsoleColorsToMenuDefault() {
	SetConsoleTextColor(CONSOLE_COLOR_WHITE, CONSOLE_COLOR_MAGENTA);
}

// Clase que gestiona la visualizacion en consola del juego
class ConsoleView {
public:
	// Constructor que recibe el manejador de textos multilenguaje
	ConsoleView(const LocalizationManager& i18n);

	// Establece colores y limpia la pantalla para el menu principal (fondo magenta)
	void SetMenuColorsAndClear() const;
	// Establece colores y limpia la pantalla para el juego (fondo negro)
	void SetGameColorsAndClear() const;

	// Muestra el menu principal y resalta la opcion seleccionada
	void DisplayMainMenu(int selectedOption) const;
	// Muestra el tablero de juego en la consola
	// gameBoard: referencia al tablero actual
	// gameBgColor: color de fondo del tablero (por defecto negro)
	void DisplayBoard(const Board& gameBoard, int gameBgColor = CONSOLE_COLOR_BLACK) const;

	// Muestra un mensaje en la consola con colores especificos
	// message: texto a mostrar
	// newLine: si es true, agrega salto de linea
	// fgColor: color de texto (-1 para usar el color actual)
	// bgColor: color de fondo (-1 para usar el color actual)
	void DisplayMessage(const std::string& message, bool newLine = true,
		int fgColor = -1, int bgColor = -1) const;

	// Limpia toda la pantalla de la consola
	void ClearScreen() const;
	// Limpia una seccion de la pantalla desde la linea startY por numLines lineas
	// consoleWidth: ancho de la consola (por defecto 80)
	void ClearLines(int startY, int numLines, int consoleWidth = 80) const;

	// Muestra el menu de seleccion de idioma
	// selectedOption: opcion resaltada
	// title: titulo del menu
	// opt1_text: texto de la opcion 1
	// opt2_text: texto de la opcion 2
	// instruction_text: instrucciones para el usuario
	void DisplayLanguageSelectionMenu(int selectedOption,
		const std::string& title,
		const std::string& opt1_text,
		const std::string& opt2_text,
		const std::string& instruction_text) const;
private:
	const LocalizationManager& m_i18n; // Referencia al manejador de textos multilenguaje
};

#endif // CONSOLE_VIEW_H