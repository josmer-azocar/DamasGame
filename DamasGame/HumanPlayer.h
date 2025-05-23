#ifndef HUMAN_PLAYER_H
#define HUMAN_PLAYER_H

#include "Player.h"
#include "InputHandler.h" 
#include "ConsoleView.h"  //para mensajes

// Clase que representa a un jugador humano
class HumanPlayer : public Player {
public:
	// Constructor: inicializa el jugador humano con su color, el manejador de entrada y la vista de consola
	HumanPlayer(PlayerColor color, InputHandler& inputHandler, const ConsoleView& view);

	// Implementacion del metodo para obtener la entrada de movimiento del humano
	// Devuelve un MoveInput con la informacion ingresada por el usuario
	MoveInput GetChosenMoveInput(
		const Board& board,
		const MoveGenerator& moveGenerator,
		bool isInCaptureSequence,
		int forcedRow,
		int forcedCol,
		const std::vector<Move>& availableMandatoryJumps
	) override;

private:
	InputHandler& m_inputHandler; // Referencia al manejador de entrada
	const ConsoleView& m_view;    // Referencia a la vista para mostrar mensajes
};

#endif // HUMAN_PLAYER_H
