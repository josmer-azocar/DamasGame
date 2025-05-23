#include "HumanPlayer.h"
#include "CommonTypes.h" // Para PlayerColorToString, ToAlgebraic
#include <iostream>      

// Constructor de HumanPlayer
// Inicializa el jugador humano con su color, el manejador de entrada y la vista de consola
HumanPlayer::HumanPlayer(PlayerColor color, InputHandler& inputHandler, const ConsoleView& view)
	: Player(color), m_inputHandler(inputHandler), m_view(view) {
}

// Solicita al usuario humano que ingrese su movimiento
// Devuelve un MoveInput con la informacion ingresada por el usuario
MoveInput HumanPlayer::GetChosenMoveInput(
	const Board& board,
	const MoveGenerator& moveGenerator,
	bool isInCaptureSequence,
	int forcedRow,
	int forcedCol,
	const std::vector<Move>& availableMandatoryJumps
) {
	// Solicita el movimiento al usuario segun el color del jugador
	MoveInput userInput = m_inputHandler.GetPlayerMoveInput(m_color);

	return userInput;
}