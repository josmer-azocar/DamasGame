// ConsoleView.cpp

#include "ConsoleView.h"
#include "Board.h"       
#include "CommonTypes.h" 
#include "LocalizationManager.h"
#include <iostream>
#include <iomanip>    
#include <string>
#include <vector>
#include <cstdlib>    
#include <sstream>   

ConsoleView::ConsoleView(const LocalizationManager& i18n) :
	// Constructor
	m_i18n(i18n)
{
}

void ConsoleView::DisplayLanguageSelectionMenu(int selectedOption,
	const std::string& title,
	const std::string& opt1_text,
	const std::string& opt2_text,
	const std::string& instruction_text) const {
	// Asumimos que SetGameColorsAndClear() (fondo negro) ya fue llamado por LocalizationManager
	GoToXY(0, 0);

	int consoleWidth = 80;

	int titleX = (consoleWidth - static_cast<int>(title.length())) / 2;
	if (titleX < 0) titleX = 0;
	GoToXY(titleX, 3);
	// Título Amarillo sobre el fondo Negro actual
	DisplayMessage(title, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	GoToXY(titleX > 2 ? titleX - 2 : 0, 4); // Evitar X negativo
	// Crear una línea de separador que coincida con el largo del título
	std::string separator(title.length() + 4, '='); // +4 para un poco más de ancho
	if (titleX > 2) GoToXY(titleX - 2, 4); else GoToXY(0, 4);
	DisplayMessage(separator, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);


	std::vector<std::string> menuOptions = { opt1_text, opt2_text };

	int startY = 7;
	for (size_t i = 0; i < menuOptions.size(); ++i) {
		int optionX = (consoleWidth - static_cast<int>(menuOptions[i].length())) / 2;
		if (optionX < 0) optionX = 0;
		GoToXY(optionX, startY + static_cast<int>(i) * 2);
		if (static_cast<int>(i) + 1 == selectedOption) {
			// Opción seleccionada: Texto Negro sobre Amarillo
			DisplayMessage(menuOptions[i], false, CONSOLE_COLOR_BLACK, CONSOLE_COLOR_YELLOW);
		}
		else {
			// Opción no seleccionada: Texto Blanco/Gris Claro sobre Negro
			DisplayMessage(menuOptions[i], false, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
		}
	}
	std::cout << std::endl;

	int instructionY = startY + static_cast<int>(menuOptions.size()) * 2 + 1;
	int instructionX = (consoleWidth - static_cast<int>(instruction_text.length())) / 2;
	if (instructionX < 0) instructionX = 0;
	GoToXY(instructionX, instructionY);
	DisplayMessage(instruction_text, true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_BLACK);

	SetConsoleTextColor(CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK); // Color por defecto para el resto
}


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

void ConsoleView::SetMenuColorsAndClear() const {
	WORD attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	attributes |= (CONSOLE_COLOR_MAGENTA << 4);
	SetConsoleAttributesAndClear(attributes);
}

void ConsoleView::SetGameColorsAndClear() const {
	WORD attributes = CONSOLE_COLOR_LIGHT_GRAY;
	attributes |= (CONSOLE_COLOR_BLACK << 4);
	SetConsoleAttributesAndClear(attributes);
}

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

void ConsoleView::DisplayMainMenu(int selectedOption) const {
	GoToXY(0, 0);

	int consoleWidth = 80;
	// Ancho del arte ASCII de "DAMAS" con caracteres básicos
	int asciiArtWidth = 58;
	int titleX = (consoleWidth - asciiArtWidth) / 2;
	if (titleX < 0) titleX = 0;

	int currentY = 2;

	SetConsoleTextColor(CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_MAGENTA);

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
			DisplayMessage(menuOptions[i], false, CONSOLE_COLOR_BLACK, CONSOLE_COLOR_YELLOW);
		}
		else {
			DisplayMessage(menuOptions[i], false, CONSOLE_COLOR_WHITE, CONSOLE_COLOR_MAGENTA);
		}
	}
	
	int instructionY = startY + static_cast<int>(menuOptions.size()) * 2 + 1;
	std::string instructionText = m_i18n.GetString("menu_instruction");
	int instructionX = (consoleWidth - static_cast<int>(instructionText.length())) / 2;
	if (instructionX < 0) instructionX = 0;
	GoToXY(instructionX, instructionY);
	DisplayMessage(instructionText, true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_MAGENTA);

	SetConsoleTextColor(CONSOLE_COLOR_WHITE, CONSOLE_COLOR_MAGENTA);
}

void ConsoleView::DisplayBoard(const Board& gameBoard, int gameBgColor) const {
	int borderColor = (gameBgColor == CONSOLE_COLOR_BLACK) ? CONSOLE_COLOR_WHITE : CONSOLE_COLOR_BLACK;

	SetConsoleTextColor(borderColor, gameBgColor);

	std::cout << "     ";
	for (int c = 0; c < Board::BOARD_SIZE; ++c) {
		std::cout << (char)('A' + c);
		if (c < Board::BOARD_SIZE - 1) std::cout << "   ";
	}
	std::cout << std::endl;

	SetConsoleTextColor(borderColor, gameBgColor);
	std::cout << "   " << (char)218;
	for (int c = 0; c < Board::BOARD_SIZE; ++c) {
		std::cout << (char)196 << (char)196 << (char)196;
		if (c < Board::BOARD_SIZE - 1) std::cout << (char)194;
		else std::cout << (char)191;
	}
	std::cout << std::endl;

	for (int r = 0; r < Board::BOARD_SIZE; ++r) {
		SetConsoleTextColor(borderColor, gameBgColor);
		std::cout << std::setw(2) << (Board::BOARD_SIZE - r) << " ";
		std::cout << (char)179;

		for (int c_loop = 0; c_loop < Board::BOARD_SIZE; ++c_loop) {
			int squareBgColor = gameBoard.IsPlayableSquare(r, c_loop) ? CONSOLE_COLOR_DARK_GRAY : CONSOLE_COLOR_LIGHT_GRAY;
			PieceType piece = gameBoard.GetPieceAt(r, c_loop);
			char symbolToShow = ' ';
			int pieceFgColor = CONSOLE_COLOR_BLACK;

			if (piece == PieceType::P1_MAN) { // Jugador 1 (Blancas) Peón
				symbolToShow = 'w';
				pieceFgColor = CONSOLE_COLOR_BLUE; // O CONSOLE_COLOR_WHITE
			}
			else if (piece == PieceType::P1_KING) { // Jugador 1 (Blancas) Dama
				symbolToShow = 'W';
				pieceFgColor = CONSOLE_COLOR_LIGHT_BLUE; // O CONSOLE_COLOR_CYAN
			}
			else if (piece == PieceType::P2_MAN) { // Jugador 2 (Negras) Peón
				symbolToShow = 'b';
				pieceFgColor = CONSOLE_COLOR_RED;
			}
			else if (piece == PieceType::P2_KING) { // Jugador 2 (Negras) Dama
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