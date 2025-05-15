#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <string>
#include <vector>
#include "CommonTypes.h" // Para PlayerColor y MoveInput

// Para GetMenuChoice
#include <conio.h> // Para _getch() en Windows
#define KEY_UP 72    // Códigos de flecha para Windows
#define KEY_DOWN 80
#define KEY_ENTER 13

class InputHandler {
public:
    InputHandler();

    MoveInput GetPlayerMoveInput(PlayerColor currentPlayer);
    int GetMenuChoice(int currentSelection, int numOptions); // NUEVO

private:
    bool ParseCoordinate(const std::string& coordStr, int& rowIdx, int& colIdx);
};

#endif // INPUT_HANDLER_H