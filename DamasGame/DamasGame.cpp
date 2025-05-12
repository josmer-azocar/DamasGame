// -----------------------------------------------------------------------------
// DamasGame.cpp
// Punto de entrada principal del programa.
// Crea e inicia el GameManager.
// -----------------------------------------------------------------------------

#include <iostream>
#include <limits>

#include "ConsoleView.h"
#include "InputHandler.h"
#include "Board.h"
#include "GameManager.h" // <--- Asegúrate que este include esté
#include "CommonTypes.h"

int main() {
    Board gameBoard;
    ConsoleView view;
    InputHandler inputHandler;

    // Crear el GameManager, pasándole los componentes que necesita
    GameManager game(gameBoard, view, inputHandler);

    game.RunGameLoop(); // Iniciar el bucle principal del juego

    // El mensaje final ahora lo maneja RunGameLoop
    GoToXY(0, Board::BOARD_SIZE * 2 + 8); // Mover un poco más abajo
    ResetConsoleTextColor();
    // Ya no es necesario el cin.get() aquí porque RunGameLoop lo tiene

    return 0;
}