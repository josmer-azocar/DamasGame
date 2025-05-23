#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <string>
#include <vector>
#include "CommonTypes.h" // Para PlayerColor y MoveInput

// Para GetMenuChoice
#include <conio.h> // Para _getch() en Windows
#define KEY_UP 72    // Codigos de flecha para Windows
#define KEY_DOWN 80
#define KEY_ENTER 13

// Clase encargada de manejar la entrada del usuario desde consola
class InputHandler {
public:
	// Constructor por defecto
	InputHandler();

	// Solicita y obtiene el movimiento del jugador actual
	MoveInput GetPlayerMoveInput(PlayerColor currentPlayer);

	// Permite seleccionar una opcion de menu usando las teclas de flecha y enter
	// currentSelection: opcion actualmente seleccionada
	// numOptions: cantidad total de opciones en el menu
	int GetMenuChoice(int currentSelection, int numOptions);

private:
	// Parsea una coordenada en formato string (ej: "A3") y la convierte a indices de fila y columna
	bool ParseCoordinate(const std::string& coordStr, int& rowIdx, int& colIdx);
};

#endif // INPUT_HANDLER_H