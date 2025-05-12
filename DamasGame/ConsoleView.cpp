// -----------------------------------------------------------------------------
// ConsoleView.cpp
// Implementación de la clase ConsoleView para mostrar el juego en consola.
// -----------------------------------------------------------------------------

#include "ConsoleView.h" // Incluye la declaración de la clase ConsoleView
#include "Board.h"       // Necesita la definición de Board para el parámetro de DisplayBoard
#include "CommonTypes.h" // Para PieceType, PlayerColor, y las constantes CONSOLE_COLOR_...

#include <iostream>      // Para std::cout, std::endl
#include <iomanip>       // Para std::setw
#include <string>        // Para std::string
#include <vector>        // Para el parámetro de DisplayMainMenu (si lo usamos)
#include <cstdlib>       // Para system("cls") o system("clear")

ConsoleView::ConsoleView() {
    // Constructor.
}

void ConsoleView::ClearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void ConsoleView::DisplayMessage(const std::string& message, bool newLine) const {
    std::cout << message;
    if (newLine) {
        std::cout << std::endl;
    }
}

// Implementación de DisplayBoard con alineación (esperemos que esta sea la definitiva!)
void ConsoleView::DisplayBoard(const Board& gameBoard) const {
    GoToXY(0, 0);
    ResetConsoleTextColor();

    // --- Imprimir Coordenadas de Columna (A-H) Alineadas (INTENTO CON SETW) ---
    ResetConsoleTextColor();
    std::cout << "   "; // 3 espacios para la columna de números de fila (ej: " 8 ")

    // Cada "celda de letra" debe ocupar lo mismo que el contenido + un borde = 4 caracteres
    // " L  " (Espacio, Letra, Espacio, Espacio)
    for (int c = 0; c < Board::BOARD_SIZE; ++c) {
        std::cout << " " << (char)('A' + c) << "  ";
    }
    std::cout << std::endl;

    // --- Imprimir Línea Superior del Tablero ---
    ResetConsoleTextColor();
    std::cout << "  " << (char)218; // TOP_LEFT_CORNER_S (2 espacios para números + esquina)
    for (int c = 0; c < Board::BOARD_SIZE; ++c) {
        for (int i = 0; i < 3; ++i) std::cout << (char)196; // HORIZONTAL_LINE_S (para el contenido " S ")
        if (c < Board::BOARD_SIZE - 1) {
            std::cout << (char)194; // T_DOWN_S
        }
        else {
            std::cout << (char)191; // TOP_RIGHT_CORNER_S
        }
    }
    std::cout << std::endl;

    // --- Imprimir Filas del Tablero ---
    for (int r = 0; r < Board::BOARD_SIZE; ++r) {
        ResetConsoleTextColor();
        std::cout << std::setw(2) << (Board::BOARD_SIZE - r) << " "; // Número de fila (ocupa 3 chars)
        std::cout << (char)179; // VERTICAL_LINE_S (borde izquierdo de la fila)

        for (int c = 0; c < Board::BOARD_SIZE; ++c) {
            int squareBgColor = gameBoard.IsPlayableSquare(r, c) ? CONSOLE_COLOR_DARK_GRAY : CONSOLE_COLOR_LIGHT_GRAY;
            PieceType piece = gameBoard.GetPieceAt(r, c);
            char symbolToShow = ' ';
            int pieceFgColor = CONSOLE_COLOR_BLACK;

            if (piece == PieceType::P1_MAN) { symbolToShow = 'o'; pieceFgColor = CONSOLE_COLOR_RED; }
            else if (piece == PieceType::P1_KING) { symbolToShow = 'O'; pieceFgColor = CONSOLE_COLOR_LIGHT_RED; }
            else if (piece == PieceType::P2_MAN) { symbolToShow = 'x'; pieceFgColor = CONSOLE_COLOR_BLUE; }
            else if (piece == PieceType::P2_KING) { symbolToShow = 'X'; pieceFgColor = CONSOLE_COLOR_LIGHT_BLUE; }
            else { pieceFgColor = squareBgColor; } // Casilla vacía

            SetConsoleTextColor(pieceFgColor, squareBgColor);
            std::cout << " " << symbolToShow << " "; // Contenido de 3 caracteres (" Símbolo ")

            ResetConsoleTextColor();
            std::cout << (char)179; // VERTICAL_LINE_S (borde derecho de esta casilla / izquierdo de la siguiente)
        }
        std::cout << std::endl;

        // --- Imprimir Borde Horizontal entre Filas (o Borde Inferior) ---
        ResetConsoleTextColor();
        std::cout << "   "; // 3 espacios para alinear con números de fila
        if (r < Board::BOARD_SIZE - 1) {
            std::cout << (char)195; // T_RIGHT_S
            for (int c = 0; c < Board::BOARD_SIZE; ++c) {
                for (int i = 0; i < 3; ++i) std::cout << (char)196; // HORIZONTAL_LINE_S
                if (c < Board::BOARD_SIZE - 1) {
                    std::cout << (char)197; // CROSS_S
                }
                else {
                    std::cout << (char)180; // T_LEFT_S
                }
            }
        }
        else { // Última fila, dibujar borde inferior completo
            std::cout << (char)192; // BOTTOM_LEFT_CORNER_S
            for (int c = 0; c < Board::BOARD_SIZE; ++c) {
                for (int i = 0; i < 3; ++i) std::cout << (char)196; // HORIZONTAL_LINE_S
                if (c < Board::BOARD_SIZE - 1) {
                    std::cout << (char)193; // T_UP_S
                }
                else {
                    std::cout << (char)217; // BOTTOM_RIGHT_CORNER_S
                }
            }
        }
        std::cout << std::endl;
    }
    ResetConsoleTextColor(); // Asegurar reseteo final
}