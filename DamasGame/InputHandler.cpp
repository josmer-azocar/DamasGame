#include "InputHandler.h" // Incluye la declaración de esta clase
#include "CommonTypes.h"  // Para PlayerColor, PlayerColorToString, y MoveInput

#include <iostream>       // Para std::cout, std::cin, std::getline
#include <string>         // Para std::string
#include <sstream>        // Para std::stringstream
#include <limits>         // Para std::numeric_limits

InputHandler::InputHandler() {
    // Constructor
}

MoveInput InputHandler::GetPlayerMoveInput(PlayerColor currentPlayer) {
    MoveInput inputResult; // MoveInput se define en CommonTypes.h

    // PlayerColorToString se define en CommonTypes.h
    std::string playerStr = PlayerColorToString(currentPlayer);

    std::cout << "Turno de " << playerStr
        << ". Introduce tu movimiento (filaIni colIni filaFin colFin, ej. 2 0 3 1) o 'salir': ";

    std::string lineInput;
    std::getline(std::cin >> std::ws, lineInput); // Lee la línea completa, std::ws consume espacios iniciales

    if (lineInput == "salir") {
        inputResult.wantsToExit = true;
        inputResult.isValidFormat = true; // Salir es un formato válido de "acción"
        return inputResult;
    }

    std::stringstream ss(lineInput);
    // Intenta leer 4 enteros.
    if (ss >> inputResult.startRow >> inputResult.startCol >> inputResult.endRow >> inputResult.endCol) {
        // Verifica si hay más datos en la línea después de los 4 números
        char remainingCharTest;
        if (ss >> remainingCharTest) { // Si se pudo leer algo más...
            inputResult.isValidFormat = false; // ...entonces el formato es incorrecto (demasiados datos)
        }
        else {
            inputResult.isValidFormat = true; // Se leyeron exactamente 4 números
        }
    }
    else {
        // No se pudieron leer 4 números
        inputResult.isValidFormat = false;
    }
    return inputResult;
}