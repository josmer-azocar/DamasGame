#include "ConsoleView.h"
#include "Board.h"
#include "CommonTypes.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib> // Para system() si se usara (tratamos de evitarlo)

ConsoleView::ConsoleView() {
    // Constructor
}

// Helper para establecer colores globales y limpiar
void SetAndClear(WORD attributes) {
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
    WORD attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Texto blanco brillante
    attributes |= (CONSOLE_COLOR_MAGENTA << 4); // Fondo MAGENTA
    SetAndClear(attributes);
}

void ConsoleView::SetGameColorsAndClear() const {
    WORD attributes = CONSOLE_COLOR_LIGHT_GRAY; // Texto gris claro
    attributes |= (CONSOLE_COLOR_BLACK << 4);  // Fondo NEGRO
    SetAndClear(attributes);
}

void ConsoleView::ClearScreen() const {
    // Esta función simplemente borra con los colores *actualmente establecidos* en la consola.
    // Para cambiar el esquema de color + borrar, usar SetMenuColorsAndClear o SetGameColorsAndClear.
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi); // Obtener atributos actuales
    DWORD count;
    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    COORD homeCoords = { 0, 0 };
    FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords, &count);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count); // Usar atributos actuales
    SetConsoleCursorPosition(hConsole, homeCoords);
}

void ConsoleView::ClearLines(int startY, int numLines, int consoleWidth) const {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi); // Obtener atributos actuales (para el color de fondo del borrado)

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
    // No resetear aquí, la próxima llamada a SetConsoleTextColor o un reseteo global lo hará.
}

void ConsoleView::DisplayMainMenu(int selectedOption) const {
    // Se asume que SetMenuColorsAndClear() ya fue llamado por GameManager
    GoToXY(0, 0); // Asegurar que empezamos arriba para redibujar si es necesario

    std::string title = "M E N U   P R I N C I P A L";
    int consoleWidth = 80;
    // ... (código para obtener consoleWidth si se desea) ...

    int titleX = (consoleWidth - static_cast<int>(title.length())) / 2;
    GoToXY(titleX, 3);
    DisplayMessage(title, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_MAGENTA);
    GoToXY(titleX > 2 ? titleX - 2 : 0, 4);
    DisplayMessage("===============================", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_MAGENTA);

    std::vector<std::string> menuOptions = {
        "1. Jugador vs Jugador",
        "2. Jugador vs Computadora",
        "3. Computadora vs Computadora",
        "4. Estadisticas (Globales - No implementado)",
        "5. Salir"
    };

    int startY = 7;
    for (size_t i = 0; i < menuOptions.size(); ++i) {
        GoToXY((consoleWidth - static_cast<int>(menuOptions[i].length())) / 2, startY + static_cast<int>(i) * 2);
        if (static_cast<int>(i) + 1 == selectedOption) {
            DisplayMessage(menuOptions[i], false, CONSOLE_COLOR_BLACK, CONSOLE_COLOR_YELLOW);
        }
        else {
            DisplayMessage(menuOptions[i], false, CONSOLE_COLOR_WHITE, CONSOLE_COLOR_MAGENTA);
        }
    }
    // Nueva línea después de la última opción del menú para espaciar
    std::cout << std::endl;

    GoToXY((consoleWidth - 46) / 2, startY + static_cast<int>(menuOptions.size()) * 2 + 1);
    DisplayMessage("Use (W/ARRIBA), (S/ABAJO), (ENTER) para seleccionar.", true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_MAGENTA);
    // Reset general al color del menú al final, por si acaso
    SetConsoleTextColor(CONSOLE_COLOR_WHITE, CONSOLE_COLOR_MAGENTA);
}

void ConsoleView::DisplayBoard(const Board& gameBoard, int gameBgColor) const {
    // gameBgColor es el color de fondo general de la pantalla de juego (ej. CONSOLE_COLOR_BLACK)
    // Los bordes y coordenadas usarán un color de texto que contraste con gameBgColor.
    int borderColor = (gameBgColor == CONSOLE_COLOR_BLACK) ? CONSOLE_COLOR_LIGHT_GRAY : CONSOLE_COLOR_BLACK;

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

        for (int c = 0; c < Board::BOARD_SIZE; ++c) {
            // Colores de las casillas como antes (DARK_GRAY y LIGHT_GRAY)
            int squareBgColor = gameBoard.IsPlayableSquare(r, c) ? CONSOLE_COLOR_DARK_GRAY : CONSOLE_COLOR_LIGHT_GRAY;
            PieceType piece = gameBoard.GetPieceAt(r, c);
            char symbolToShow = ' ';
            // El color de la pieza debe contrastar con squareBgColor.
            int pieceFgColor = CONSOLE_COLOR_BLACK; // Default si no hay pieza, para el espacio.

            if (piece == PieceType::P1_MAN) { symbolToShow = 'b'; pieceFgColor = CONSOLE_COLOR_RED; }
            else if (piece == PieceType::P1_KING) { symbolToShow = 'B'; pieceFgColor = CONSOLE_COLOR_LIGHT_RED; }
            else if (piece == PieceType::P2_MAN) { symbolToShow = 'w'; pieceFgColor = CONSOLE_COLOR_LIGHT_BLUE; }
            else if (piece == PieceType::P2_KING) { symbolToShow = 'W'; pieceFgColor = CONSOLE_COLOR_CYAN; }
            else {
                // Si no hay pieza, el símbolo es ' '. El color del texto para ' ' no importa mucho,
                // pero para mantener la consistencia, podemos usar el mismo color que el fondo de la casilla.
                pieceFgColor = squareBgColor;
            }

            SetConsoleTextColor(pieceFgColor, squareBgColor);
            std::cout << " " << symbolToShow << " ";

            SetConsoleTextColor(borderColor, gameBgColor); // Borde vertical entre casillas
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
    // Al final de DisplayBoard, resetear al color de texto/fondo del juego.
    SetConsoleTextColor(borderColor, gameBgColor);
}