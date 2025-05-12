#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <string>
#include <vector>        // Aunque no usemos vector directamente en los métodos públicos por ahora
#include "CommonTypes.h" // Para PlayerColor y MUY IMPORTANTE: para la struct MoveInput

// La struct MoveInput debe estar definida en CommonTypes.h para que este archivo la conozca.
// class Board; // No es necesario si GetPlayerMoveInput no toma un Board& por ahora.

class InputHandler {
public:
    InputHandler(); // Constructor

    // Este método usa MoveInput y PlayerColor, que vienen de CommonTypes.h
    MoveInput GetPlayerMoveInput(PlayerColor currentPlayer);

private:
    // Métodos privados de ayuda si son necesarios en el futuro
};

#endif // INPUT_HANDLER_H