#include "InputHandler.h"
#include "CommonTypes.h" // Para PlayerColorToString si se usara (actualmente no)

#include <iostream>
#include <string>
#include <sstream>
#include <limits>    // Para std::numeric_limits
#include <algorithm> // Para std::transform
#include <cctype>    // Para std::tolower

#ifdef _WIN32 // Solo incluir conio.h en Windows
#include <conio.h>   // Para _getch()
#else
// Alternativa para _getch() en sistemas no Windows (ej. con ncurses)
// Si no usas ncurses, esta parte necesitaría una implementación diferente
// o GetMenuChoice tendría que usar std::cin
// #include <ncurses.h> // Ejemplo si usaras ncurses
int _getch() { // Implementación simple con std::cin para no Windows (menos ideal para menús)
    char c = std::cin.get();
    if (c == '\n' && std::cin.peek() == EOF) { // Evitar problemas con doble enter al final
        // No hacer nada especial aquí, solo consumir el char
    }
    // No maneja flechas directamente de esta forma simple
    return c;
}
// Definiciones de teclas para que compile en no-Windows, aunque la funcionalidad de flechas no será la misma
#define KEY_UP 'W'       // Usar W para arriba
#define KEY_DOWN 'S'     // Usar S para abajo
#define KEY_ENTER '\r'   // Enter (puede ser \n o \r según el sistema)
                         // '\r' es carriage return, a menudo lo que Enter envía
#endif


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

    // La conversión de fila asume que '1' es la fila inferior (índice 7)
    // y '8' es la fila superior (índice 0)
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
    // Los mensajes de "Turno de..." o "Continuar captura..." ahora se manejan en GameManager
    // para poder usar mView.DisplayMessage con posicionamiento y colores,
    // y para tener acceso al estado mInCaptureSequence.
    // InputHandler ahora solo se enfoca en el prompt ">" y leer la línea.

    std::cout << "> "; // Prompt simple

    std::string lineInput;
    // Si el input anterior fue un _getch() (del menú), puede haber un '\n' residual.
    // El std::ws en getline debería manejar espacios en blanco iniciales, incluyendo algunos newlines.
    std::getline(std::cin >> std::ws, lineInput);

    std::string originalInputForParsing = lineInput; // Conservar may/min para coordenadas
    std::transform(lineInput.begin(), lineInput.end(), lineInput.begin(),
        [](unsigned char c) { return std::tolower(c); }); // Convertir a minúsculas para comandos

    if (lineInput == "salir") {
        inputResult.wantsToExit = true;
        return inputResult;
    }
    if (lineInput == "stats" || lineInput == "estadisticas") {
        inputResult.wantsToShowStats = true;
        return inputResult;
    }

    std::stringstream ss(originalInputForParsing); // Usar la versión con may/min originales para parsear coordenadas
    std::string startCoordStr, endCoordStr;

    if (ss >> startCoordStr >> endCoordStr) {
        std::string remainingTest;
        if (ss >> remainingTest) { // Si hay más de dos "palabras"
            inputResult.isValidFormat = false;
        }
        else { // Exactamente dos "palabras"
            if (ParseCoordinate(startCoordStr, inputResult.startRow, inputResult.startCol) &&
                ParseCoordinate(endCoordStr, inputResult.endRow, inputResult.endCol)) {
                inputResult.isValidFormat = true;
            }
            else {
                inputResult.isValidFormat = false; // Alguna coordenada no es válida en formato A1-H8
            }
        }
    }
    else { // No se pudieron leer dos "palabras"
        inputResult.isValidFormat = false;
    }

    // El GameManager se encargará de mostrar el mensaje de error si !isValidFormat
    // y no es un comando especial.
    return inputResult;
}

int InputHandler::GetMenuChoice(int currentSelection, int numOptions) {
    int key = 0;
    int first_char = _getch();

#ifdef _WIN32
    if (first_char == 0 || first_char == 224) { // Tecla especial en Windows (0 o E0 para flechas)
        key = _getch(); // Leer el código real de la tecla
        if (key == KEY_UP) {
            currentSelection--;
            if (currentSelection < 1) {
                currentSelection = numOptions;
            }
        }
        else if (key == KEY_DOWN) {
            currentSelection++;
            if (currentSelection > numOptions) {
                currentSelection = 1;
            }
        }
        return -currentSelection; // Indica solo navegación
    }
    else if (first_char == KEY_ENTER) { // Enter
        return currentSelection; // Opción seleccionada
    }
    else if (std::tolower(first_char) == 'w') {
        currentSelection--;
        if (currentSelection < 1) currentSelection = numOptions;
        return -currentSelection;
    }
    else if (std::tolower(first_char) == 's') {
        currentSelection++;
        if (currentSelection > numOptions) currentSelection = 1;
        return -currentSelection;
    }
    else if (first_char >= '1' && first_char <= ('0' + numOptions)) { // Entrada numérica directa
        return first_char - '0'; // Devuelve el número de opción
    }
    else if (first_char == 27) { // Tecla ESC
        // Asumimos que la última opción es "Salir". Si `numOptions` es el índice de "Salir".
        return numOptions;
    }
#else // Lógica para no-Windows (simplificada, sin manejo de flechas directas con _getch simple)
    if (first_char == KEY_ENTER || first_char == '\n') { // Enter
        return currentSelection;
    }
    else if (std::tolower(first_char) == 'w' || first_char == KEY_UP) { // KEY_UP será 'W'
        currentSelection--;
        if (currentSelection < 1) currentSelection = numOptions;
    }
    else if (std::tolower(first_char) == 's' || first_char == KEY_DOWN) { // KEY_DOWN será 'S'
        currentSelection++;
        if (currentSelection > numOptions) currentSelection = 1;
    }
    else if (first_char >= '1' && first_char <= ('0' + numOptions)) {
        return first_char - '0';
    }
    else if (first_char == 27) { // ESC
        return numOptions; // Asumir que la última opción es Salir
    }
#endif

    // Si no fue Enter ni un número de opción válido, se asume navegación
    // (o una tecla no reconocida, en cuyo caso no se cambia la selección)
    // La lógica de arriba ya actualizó currentSelection para W/S/Flechas.
    // Para el caso de no-Windows, siempre devolvemos -currentSelection para redibujar.
    // Para Windows, también, si no fue Enter o número.
    return -currentSelection;
}