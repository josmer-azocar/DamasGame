#include "InputHandler.h"
#include "CommonTypes.h" 

#include <iostream>
#include <string>
#include <sstream>
#include <limits>    // Para std::numeric_limits
#include <algorithm> // Para std::transform
#include <cctype>    // Para std::tolower

// Implementacion de InputHandler
// Incluye metodos para manejar la entrada del usuario desde consola, tanto para movimientos como para navegacion de menus

#ifdef _WIN32 
#include <conio.h>   // Para _getch() en Windows
#else
// Alternativa para _getch() en sistemas no Windows (ej. con ncurses)
// Si no usas ncurses, esta parte necesitara una implementacion diferente
// o GetMenuChoice tendria que usar std::cin

// Implementacion simple de _getch para sistemas no Windows
int _getch() { 
	char c = std::cin.get();
	if (c == '\n' && std::cin.peek() == EOF) {
		// No hace nada especial aqui
	}
	// No maneja flechas directamente de esta forma simple
	return c;
}
// Definiciones de teclas para que compile en no-Windows, aunque la funcionalidad de flechas no sera la misma
#define KEY_UP 'W'       
#define KEY_DOWN 'S'     
#define KEY_ENTER '\r'  
#endif

// Constructor por defecto
InputHandler::InputHandler() {
	// No requiere inicializacion especial
}

// Parsea una coordenada en formato string (ej: "A3") y la convierte a indices de fila y columna
// Devuelve true si la conversion fue exitosa, false si el formato es invalido
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

// Solicita y obtiene el movimiento del jugador actual desde la consola
// Permite comandos especiales como "salir" o "stats"
MoveInput InputHandler::GetPlayerMoveInput(PlayerColor currentPlayer) {
	MoveInput inputResult;

	std::cout << "> ";

	std::string lineInput;
	// Si el input anterior fue un _getch() (del menu), puede haber un '\n' residual.
	std::getline(std::cin >> std::ws, lineInput);

	std::string originalInputForParsing = lineInput; // Conservar may/min para coordenadas
	std::transform(lineInput.begin(), lineInput.end(), lineInput.begin(),
		[](unsigned char c) { return std::tolower(c); }); // Convertir a minusculas para comandos

	if (lineInput == "salir" || lineInput == "exit") {
		inputResult.wantsToExit = true;
		return inputResult;
	}
	if (lineInput == "stats" || lineInput == "estadisticas") {
		inputResult.wantsToShowStats = true;
		return inputResult;
	}

	std::stringstream ss(originalInputForParsing); // Usar la version con may/min originales para parsear coordenadas
	std::string startCoordStr, endCoordStr;

	if (ss >> startCoordStr >> endCoordStr) {
		std::string remainingTest;
		if (ss >> remainingTest) { // Si hay mas de dos "palabras"
			inputResult.isValidFormat = false;
		}
		else { // Exactamente dos "palabras"
			if (ParseCoordinate(startCoordStr, inputResult.startRow, inputResult.startCol) &&
				ParseCoordinate(endCoordStr, inputResult.endRow, inputResult.endCol)) {
				inputResult.isValidFormat = true;
			}
			else {
				inputResult.isValidFormat = false; // Alguna coordenada no es valida en formato A1-H8
			}
		}
	}
	else { // No se pudieron leer dos "palabras"
		inputResult.isValidFormat = false;
	}

	// El GameManager se encargara de mostrar el mensaje de error si !isValidFormat
	// y no es un comando especial.
	return inputResult;
}

// Permite seleccionar una opcion de menu usando las teclas de flecha, enter, o numeros
// Devuelve el numero de la opcion seleccionada o un valor negativo para navegacion
int InputHandler::GetMenuChoice(int currentSelection, int numOptions) {
	int key = 0;
	int first_char = _getch();

#ifdef _WIN32
	if (first_char == 0 || first_char == 224) { // Tecla especial en Windows (0 o E0 para flechas)
		key = _getch(); // Leer el codigo real de la tecla
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
		return -currentSelection; // Indica solo navegacion
	}
	else if (first_char == KEY_ENTER) { // Enter
		return currentSelection; // Opcion seleccionada
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
	else if (first_char >= '1' && first_char <= ('0' + numOptions)) { // Entrada numerica directa
		return first_char - '0'; // Devuelve el numero de opcion
	}
	else if (first_char == 27) { // Tecla ESC
		// Asumimos que la ultima opcion es "Salir". Si `numOptions` es el indice de "Salir".
		return numOptions;
	}
#else // Logica para no-Windows (simplificada, sin manejo de flechas directas con _getch simple)
	if (first_char == KEY_ENTER || first_char == '\n') { // Enter
		return currentSelection;
	}
	else if (std::tolower(first_char) == 'w' || first_char == KEY_UP) { // KEY_UP sera 'W'
		currentSelection--;
		if (currentSelection < 1) currentSelection = numOptions;
	}
	else if (std::tolower(first_char) == 's' || first_char == KEY_DOWN) { // KEY_DOWN sera 'S'
		currentSelection++;
		if (currentSelection > numOptions) currentSelection = 1;
	}
	else if (first_char >= '1' && first_char <= ('0' + numOptions)) {
		return first_char - '0';
	}
	else if (first_char == 27) { // ESC
		return numOptions; // Asumir que la ultima opcion es Salir
	}
#endif

	// Si no fue Enter ni un numero de opcion valido, se asume navegacion
	// (o una tecla no reconocida, en cuyo caso no se cambia la seleccion)
	return -currentSelection;
}