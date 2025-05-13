// -----------------------------------------------------------------------------
// ConsoleView.cpp
// Implementación de la clase ConsoleView para mostrar el juego en consola.
// -----------------------------------------------------------------------------

#include "ConsoleView.h" 
#include "Board.h"       
#include "CommonTypes.h" 

#include <iostream>      
#include <iomanip>       // Para std::setw
#include <string>        
#include <vector>        
#include <cstdlib>       

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

// Implementación de DisplayBoard con el último intento de ajuste de alineación (PASO UI.2.3)
void ConsoleView::DisplayBoard(const Board& gameBoard) const {
    GoToXY(0, 0);
    ResetConsoleTextColor();

    // --- Imprimir Coordenadas de Columna (A-H) ---
    std::cout << "     "; // 5 espacios antes de la primera letra 'A'

    for (int c = 0; c < Board::BOARD_SIZE; ++c) {
        std::cout << (char)('A' + c);
        if (c < Board::BOARD_SIZE - 1) {
            std::cout << "   ";
        }
    }
    std::cout << std::endl;

    // --- Imprimir Línea Superior del Tablero ---
    ResetConsoleTextColor();
    std::cout << "   " << (char)218; // 3 espacios + Esquina superior izquierda ┌

    for (int c = 0; c < Board::BOARD_SIZE; ++c) {
        std::cout << (char)196 << (char)196 << (char)196; // --- (3 caracteres de contenido)
        if (c < Board::BOARD_SIZE - 1) {
            std::cout << (char)194; // T hacia abajo ┬
        }
        else {
            std::cout << (char)191; // Esquina superior derecha ┐
        }
    }
    std::cout << std::endl;

    // --- Imprimir Filas del Tablero ---
    for (int r = 0; r < Board::BOARD_SIZE; ++r) {
        ResetConsoleTextColor();
        std::cout << std::setw(2) << (Board::BOARD_SIZE - r) << " ";
        std::cout << (char)179; // Línea vertical │ (borde izquierdo de la fila)

        for (int c = 0; c < Board::BOARD_SIZE; ++c) {
            int squareBgColor = gameBoard.IsPlayableSquare(r, c) ? CONSOLE_COLOR_DARK_GRAY : CONSOLE_COLOR_LIGHT_GRAY;
            PieceType piece = gameBoard.GetPieceAt(r, c);
            char symbolToShow = ' ';
            int pieceFgColor = CONSOLE_COLOR_BLACK;

            if (piece == PieceType::P1_MAN) { symbolToShow = 'b'; pieceFgColor = CONSOLE_COLOR_RED; }
            else if (piece == PieceType::P1_KING) { symbolToShow = 'B'; pieceFgColor = CONSOLE_COLOR_LIGHT_RED; }
            else if (piece == PieceType::P2_MAN) { symbolToShow = 'w'; pieceFgColor = CONSOLE_COLOR_BLUE; }
            else if (piece == PieceType::P2_KING) { symbolToShow = 'W'; pieceFgColor = CONSOLE_COLOR_LIGHT_BLUE; }
            else { pieceFgColor = squareBgColor; }

            SetConsoleTextColor(pieceFgColor, squareBgColor);
            std::cout << " " << symbolToShow << " ";

            ResetConsoleTextColor();
            std::cout << (char)179;
        }
        std::cout << std::endl;

        // --- Imprimir Borde Horizontal entre Filas (o Borde Inferior) ---
        ResetConsoleTextColor();
        std::cout << "   ";
        if (r < Board::BOARD_SIZE - 1) {
            std::cout << (char)195;
            for (int c = 0; c < Board::BOARD_SIZE; ++c) {
                std::cout << (char)196 << (char)196 << (char)196;
                if (c < Board::BOARD_SIZE - 1) {
                    std::cout << (char)197;
                }
                else {
                    std::cout << (char)180;
                }
            }
        }
        else {
            std::cout << (char)192;
            for (int c = 0; c < Board::BOARD_SIZE; ++c) {
                std::cout << (char)196 << (char)196 << (char)196;
                if (c < Board::BOARD_SIZE - 1) {
                    std::cout << (char)193;
                }
                else {
                    std::cout << (char)217;
                }
            }
        }
        std::cout << std::endl;
    }
    ResetConsoleTextColor();
}