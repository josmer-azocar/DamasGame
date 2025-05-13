#include "InputHandler.h"
#include "CommonTypes.h" 

#include <iostream>       
#include <string>         
#include <sstream>        
#include <limits>         
#include <algorithm>      
#include <cctype>         

InputHandler::InputHandler() {
    // Constructor
}

bool InputHandler::ParseCoordinate(const std::string& coordStr, int& rowIdx, int& colIdx) {
    if (coordStr.length() != 2) {
        return false;
    }

    char colChar = static_cast<char>(std::tolower(coordStr[0]));
    char rowChar = coordStr[1];

    if (colChar >= 'a' && colChar <= 'h') {
        colIdx = colChar - 'a';
    }
    else {
        return false;
    }

    if (rowChar >= '1' && rowChar <= '8') {
        rowIdx = 8 - (rowChar - '0');
    }
    else {
        return false;
    }
    return true;
}

MoveInput InputHandler::GetPlayerMoveInput(PlayerColor currentPlayer) {
    MoveInput inputResult;
    std::string playerDisplayMessage;

    std::cout << "--------------------------------------------------" << std::endl;
    if (currentPlayer == PlayerColor::PLAYER_1) { // Negras (b)
        playerDisplayMessage = PlayerColorToString(currentPlayer) + " (Negras - arriba, en filas 8,7,6)";
        std::cout << "Turno de " << playerDisplayMessage << "." << std::endl;
        std::cout << "Comandos: 'Origen Destino' (ej: b6 a5), 'stats', o 'salir'." << std::endl;
        // Ejemplo válido desde la posición inicial: mover pieza Negra de B6 a A5
        std::cout << "Ejemplo para Negras (b): Mover de B6 a A5 -> Ingresa: b6 a5" << std::endl;
    }
    else { // PlayerColor::PLAYER_2 (Blancas w)
        playerDisplayMessage = PlayerColorToString(currentPlayer) + " (Blancas - abajo, en filas 1,2,3)";
        std::cout << "Turno de " << playerDisplayMessage << "." << std::endl;
        std::cout << "Comandos: 'Origen Destino' (ej: c3 b4), 'stats', o 'salir'." << std::endl;
        // Ejemplo válido desde la posición inicial: mover pieza Blanca de C3 a B4
        std::cout << "Ejemplo para Blancas (w): Mover de C3 a B4 -> Ingresa: c3 b4" << std::endl;
    }
    std::cout << "> ";

    std::string lineInput;
    std::getline(std::cin >> std::ws, lineInput);

    std::string originalInputForParsing = lineInput;
    std::transform(lineInput.begin(), lineInput.end(), lineInput.begin(),
        [](unsigned char c) { return std::tolower(c); });

    if (lineInput == "salir") {
        inputResult.wantsToExit = true;
        return inputResult;
    }
    if (lineInput == "stats" || lineInput == "estadisticas") {
        inputResult.wantsToShowStats = true;
        return inputResult;
    }

    std::stringstream ss(originalInputForParsing);
    std::string startCoordStr, endCoordStr;

    if (ss >> startCoordStr >> endCoordStr) {
        std::string remainingTest;
        if (ss >> remainingTest) {
            inputResult.isValidFormat = false;
        }
        else {
            if (ParseCoordinate(startCoordStr, inputResult.startRow, inputResult.startCol) &&
                ParseCoordinate(endCoordStr, inputResult.endRow, inputResult.endCol)) {
                inputResult.isValidFormat = true;
            }
            else {
                inputResult.isValidFormat = false;
            }
        }
    }
    else {
        inputResult.isValidFormat = false;
    }

    if (!inputResult.isValidFormat && !inputResult.wantsToExit && !inputResult.wantsToShowStats) {
        std::cout << "COMANDO/FORMATO INCORRECTO. Use 'Origen Destino' (ej: b6 a5), 'stats' o 'salir'." << std::endl;
    }

    return inputResult;
}