// DamasGame.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

// --- Includes Necesarios ---
#include <iostream> // Para std::cout, std::endl, std::cin
#include <limits>
#include "ConsoleView.h"
#include "Board.h"       // Para la clase Board
#include "CommonTypes.h" // Para PlayerColor, etc. (aunque Board.h lo incluye)

// --- Punto de Entrada Principal ---
int main() {
    // --- Inicialización ---
    std::cout << "Iniciando DamasGame...\n";

    // Crear el objeto Board
    Board gameBoard; // Llama al constructor Board()

    // Inicializar el tablero a la posición estándar
    gameBoard.InitializeBoard(); // Llama a InitializeBoard()

    std::cout << "Tablero inicializado.\n";

    // --- Visualización Inicial (usando método temporal en Board) ---
    std::cout << "Mostrando tablero inicial...\n";
    std::cout << "Presione Enter después de ver el tablero para salir.\n\n";

    // Pausa breve para leer los mensajes antes de limpiar y dibujar
    std::cin.get(); // Espera un Enter

    // Limpiar la pantalla (o al menos ir arriba) ANTES de dibujar el tablero
    // Si ClearScreen() está implementado en ConsoleView, lo usaríamos.
    // Por ahora, usamos GoToXY para dibujar sobre lo anterior.
    GoToXY(0, 0);

    // Llamar al método DisplayBoard (temporal) del objeto Board
    // Este método usa las funciones GoToXY, SetConsoleTextColor, etc.
    gameBoard.DisplayBoard();

    // --- Mensaje Final y Pausa ---
    // Posicionar cursor debajo del tablero (aproximadamente)
    GoToXY(0, Board::BOARD_SIZE + 3); // Ajusta este número si es necesario

    std::cout << "\nTablero mostrado. Presione Enter para finalizar...";
    std::cin.ignore(10000, '\n');
    std::cin.get(); // Espera el Enter final

    ResetConsoleTextColor(); // Buena práctica al salir

    return 0; // Termina el programa
}