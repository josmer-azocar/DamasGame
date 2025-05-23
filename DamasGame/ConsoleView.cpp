#include "ConsoleView.h"
#include "Board.h"       // Para mostrar el tablero
#include "CommonTypes.h" // Tipos comunes del juego
#include "LocalizationManager.h"
#include <iostream>
#include <iomanip>    // Para manipulacion de salida (setw)
#include <string>
#include <vector>
#include <cstdlib>    // Para funciones de consola
#include <sstream>   

// Constructor de ConsoleView
// Inicializa la referencia al manejador de textos multilenguaje
ConsoleView::ConsoleView(const LocalizationManager& i18n) :
	m_i18n(i18n)
{
}

// Muestra el menu de seleccion de idioma en la consola
// selectedOption: opcion resaltada
// title: titulo del menu
// opt1_text: texto de la opcion 1
// opt2_text: texto de la opcion 2
// instruction_text: instrucciones para el usuario
void ConsoleView::DisplayLanguageSelectionMenu(int selectedOption,
	const std::string& title,
	const std::string& opt1_text,
	const std::string& opt2_text,
	const std::string& instruction_text) const {
	// Se asume que SetGameColorsAndClear() ya fue llamado
	GoToXY(0, 0);

	int consoleWidth = 80;

	// Centrar el titulo
	int titleX = (consoleWidth - static_cast<int>(title.length())) / 2;
	if (titleX < 0) titleX = 0;
	GoToXY(titleX, 3);
	// Titulo amarillo sobre fondo negro
	DisplayMessage(title, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	GoToXY(titleX > 2 ? titleX - 2 : 0, 4); // Evita X negativo
	// Separador debajo del titulo
	std::string separator(title.length() + 4, '=');
	if (titleX > 2) GoToXY(titleX - 2, 4); else GoToXY(0, 4);
	DisplayMessage(separator, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);

	// Opciones del menu
	std::vector<std::string> menuOptions = { opt1_text, opt2_text };

	int startY = 7;
	for (size_t i = 0; i < menuOptions.size(); ++i) {
		int optionX = (consoleWidth - static_cast<int>(menuOptions[i].length())) / 2;
		if (optionX < 0) optionX = 0;
		GoToXY(optionX, startY + static_cast<int>(i) * 2);
		if (static_cast<int>(i) + 1 == selectedOption) {
			// Opcion seleccionada: texto negro sobre amarillo
			DisplayMessage(menuOptions[i], false, CONSOLE_COLOR_BLACK, CONSOLE_COLOR_YELLOW);
		}
		else {
			// Opcion no seleccionada: texto gris claro sobre negro
			DisplayMessage(menuOptions[i], false, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
		}
	}
	std::cout << std::endl;

	// Mostrar instrucciones centradas
	int instructionY = startY + static_cast<int>(menuOptions.size()) * 2 + 1;
	int instructionX = (consoleWidth - static_cast<int>(instruction_text.length())) / 2;
	if (instructionX < 0) instructionX = 0;
	GoToXY(instructionX, instructionY);
	DisplayMessage(instruction_text, true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_BLACK);

	SetConsoleTextColor(CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK); // Restaura color por defecto
}

// Funcion auxiliar para establecer atributos de color y limpiar la pantalla
// attributes: combinacion de colores de texto y fondo
void SetConsoleAttributesAndClear(WORD attributes) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, attributes);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	DWORD count;
	DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
	COORD homeCoords = { 0, 0 };

	FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords, &count);
	FillConsoleOutputAttribute(hConsole, attributes, cellCount, homeCoords, &count);
	SetConsoleCursorPosition(hConsole, homeCoords);
}

// Establece colores y limpia la pantalla para el menu principal
void ConsoleView::SetMenuColorsAndClear() const {
	WORD attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	attributes |= (CONSOLE_COLOR_MAGENTA << 4);
	SetConsoleAttributesAndClear(attributes);
}

// Establece colores y limpia la pantalla para el juego
void ConsoleView::SetGameColorsAndClear() const {
	WORD attributes = CONSOLE_COLOR_LIGHT_GRAY;
	attributes |= (CONSOLE_COLOR_BLACK << 4);
	SetConsoleAttributesAndClear(attributes);
}

// Limpia toda la pantalla de la consola
void ConsoleView::ClearScreen() const {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	DWORD count;
	DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
	COORD homeCoords = { 0, 0 };
	FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords, &count);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count);
	SetConsoleCursorPosition(hConsole, homeCoords);
}

// Limpia una seccion de la pantalla desde la linea startY por numLines lineas
// consoleWidth: ancho de la consola
void ConsoleView::ClearLines(int startY, int numLines, int consoleWidth) const {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	COORD coord;
	DWORD count;
	for (int i = 0; i < numLines; ++i) {
		coord.X = 0;
		coord.Y = static_cast<SHORT>(startY + i);
		FillConsoleOutputCharacter(hConsole, (TCHAR)' ', consoleWidth, coord, &count);
		FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleWidth, coord, &count);
	}
}

// Muestra un mensaje en la consola con colores especificos
// message: texto a mostrar
// newLine: si es true, agrega salto de linea
// fgColor: color de texto (-1 para usar el color actual)
// bgColor: color de fondo (-1 para usar el color actual)
void ConsoleView::DisplayMessage(const std::string& message, bool newLine, int fgColor, int bgColor) const {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	int finalFgColor = (fgColor == -1) ? (csbi.wAttributes & 0x000F) : fgColor;
	int finalBgColor = (bgColor == -1) ? ((csbi.wAttributes & 0x00F0) >> 4) : bgColor;

	SetConsoleTextColor(finalFgColor, finalBgColor);
	std::cout << message;
	if (newLine) {
		std::cout << std::endl;
	}
}

// Muestra el menu principal con arte ASCII y opciones
// selectedOption: opcion resaltada
void ConsoleView::DisplayMainMenu(int selectedOption) const {
	GoToXY(0, 0);

	int consoleWidth = 80;
	// Ancho del arte ASCII de "DAMAS"
	int asciiArtWidth = 58;
	int titleX = (consoleWidth - asciiArtWidth) / 2;
	if (titleX < 0) titleX = 0;

	int currentY = 2;

	SetConsoleTextColor(CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_MAGENTA);

	// Arte ASCII para el titulo
	std::vector<std::string> artLines = {
		" ######     ###    ######  ######   ###     #####   ",
		" ##   ##   ## ##   ##  ##  ##  ##  ## ##   ##   ##  ",
		" ##   ##  ##   ##  ##  ##  ##  ## ##   ##  ##       ",
		" ##   ##  #######  ##  ##  ##  ## #######   ###### ",
		" ##   ##  ##   ##  ##   ####   ## ##   ##        ## ",
		" ##   ##  ##   ##  ##    ##    ## ##   ##  ##   ##  ",
		" ######   ##   ##  ##    ##    ## ##   ##   #####   "
	};

	for (const std::string& artLine : artLines) {
		GoToXY(titleX, currentY++);
		std::cout << artLine << std::endl;
	}

	// Opciones del menu principal
	std::vector<std::string> menuOptions = {
		m_i18n.GetString("menu_opt_pvp"),
		m_i18n.GetString("menu_opt_pvc"),
		m_i18n.GetString("menu_opt_cvc"),
		m_i18n.GetString("menu_opt_stats"),
		m_i18n.GetString("menu_opt_exit"),
	};

	int startY = currentY + 1;
	for (size_t i = 0; i < menuOptions.size(); ++i) {
		int optionX = (consoleWidth - static_cast<int>(menuOptions[i].length())) / 2;
		if (optionX < 0) optionX = 0;
		GoToXY(optionX, startY + static_cast<int>(i) * 2);

		if (static_cast<int>(i) + 1 == selectedOption) {
			// Opcion seleccionada: texto negro sobre amarillo
			DisplayMessage(menuOptions[i], false, CONSOLE_COLOR_BLACK, CONSOLE_COLOR_YELLOW);
		}
		else {
			// Opcion no seleccionada: texto blanco sobre magenta
			DisplayMessage(menuOptions[i], false, CONSOLE_COLOR_WHITE, CONSOLE_COLOR_MAGENTA);
		}
	}
	
	// Instruccion para el usuario
	int instructionY = startY + static_cast<int>(menuOptions.size()) * 2 + 1;
	std::string instructionText = m_i18n.GetString("menu_instruction");
	int instructionX = (consoleWidth - static_cast<int>(instructionText.length())) / 2;
	if (instructionX < 0) instructionX = 0;
	GoToXY(instructionX, instructionY);
	DisplayMessage(instructionText, true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_MAGENTA);

	SetConsoleTextColor(CONSOLE_COLOR_WHITE, CONSOLE_COLOR_MAGENTA);
}

// Muestra el tablero de juego en la consola
// gameBoard: referencia al tablero actual
// gameBgColor: color de fondo del tablero
void ConsoleView::DisplayBoard(const Board& gameBoard, int gameBgColor) const {
	int borderColor = (gameBgColor == CONSOLE_COLOR_BLACK) ? CONSOLE_COLOR_WHITE : CONSOLE_COLOR_BLACK;

	SetConsoleTextColor(borderColor, gameBgColor);

	// Imprime letras de columnas
	std::cout << "     ";
	for (int c = 0; c < Board::BOARD_SIZE; ++c) {
		std::cout << (char)('A' + c);
		if (c < Board::BOARD_SIZE - 1) std::cout << "   ";
	}
	std::cout << std::endl;

	SetConsoleTextColor(borderColor, gameBgColor);
	// Imprime borde superior
	std::cout << "   " << (char)218;
	for (int c = 0; c < Board::BOARD_SIZE; ++c) {
		std::cout << (char)196 << (char)196 << (char)196;
		if (c < Board::BOARD_SIZE - 1) std::cout << (char)194;
		else std::cout << (char)191;
	}
	std::cout << std::endl;

	// Imprime filas del tablero
	for (int r = 0; r < Board::BOARD_SIZE; ++r) {
		SetConsoleTextColor(borderColor, gameBgColor);
		std::cout << std::setw(2) << (Board::BOARD_SIZE - r) << " ";
		std::cout << (char)179;

		for (int c_loop = 0; c_loop < Board::BOARD_SIZE; ++c_loop) {
			// Determina color de fondo de la casilla
			int squareBgColor = gameBoard.IsPlayableSquare(r, c_loop) ? CONSOLE_COLOR_DARK_GRAY : CONSOLE_COLOR_LIGHT_GRAY;
			PieceType piece = gameBoard.GetPieceAt(r, c_loop);
			char symbolToShow = ' ';
			int pieceFgColor = CONSOLE_COLOR_BLACK;

			// Determina simbolo y color segun la pieza
			if (piece == PieceType::P1_MAN) {
				symbolToShow = 'w';
				pieceFgColor = CONSOLE_COLOR_BLUE;
			}
			else if (piece == PieceType::P1_KING) {
				symbolToShow = 'W';
				pieceFgColor = CONSOLE_COLOR_LIGHT_BLUE;
			}
			else if (piece == PieceType::P2_MAN) {
				symbolToShow = 'b';
				pieceFgColor = CONSOLE_COLOR_RED;
			}
			else if (piece == PieceType::P2_KING) {
				symbolToShow = 'B';
				pieceFgColor = CONSOLE_COLOR_LIGHT_RED;
			}
			else {
				pieceFgColor = squareBgColor;
			}

			SetConsoleTextColor(pieceFgColor, squareBgColor);
			std::cout << " " << symbolToShow << " ";

			SetConsoleTextColor(borderColor, gameBgColor);
			std::cout << (char)179;
		}
		std::cout << std::endl;

		SetConsoleTextColor(borderColor, gameBgColor);
		// Imprime borde intermedio o inferior
		std::cout << "   ";
		if (r < Board::BOARD_SIZE - 1) {
			std::cout << (char)195;
			for (int c_b = 0; c_b < Board::BOARD_SIZE; ++c_b) {
				std::cout << (char)196 << (char)196 << (char)196;
				if (c_b < Board::BOARD_SIZE - 1) std::cout << (char)197;
				else std::cout << (char)180;
			}
		}
		else {
			std::cout << (char)192;
			for (int c_b = 0; c_b < Board::BOARD_SIZE; ++c_b) {
				std::cout << (char)196 << (char)196 << (char)196;
				if (c_b < Board::BOARD_SIZE - 1) std::cout << (char)193;
				else std::cout << (char)217;
			}
		}
		std::cout << std::endl;
	}
	SetConsoleTextColor(borderColor, gameBgColor);
}