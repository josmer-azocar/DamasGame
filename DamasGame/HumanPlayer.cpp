#include "HumanPlayer.h"
#include "CommonTypes.h" // Para PlayerColorToString, ToAlgebraic
#include <iostream>      

HumanPlayer::HumanPlayer(PlayerColor color, InputHandler& inputHandler, const ConsoleView& view)
	: Player(color), m_inputHandler(inputHandler), m_view(view) {
}

MoveInput HumanPlayer::GetChosenMoveInput(
	const Board& board,
	const MoveGenerator& moveGenerator,
	bool isInCaptureSequence,
	int forcedRow,
	int forcedCol,
	const std::vector<Move>& availableMandatoryJumps
) {

	MoveInput userInput = m_inputHandler.GetPlayerMoveInput(m_color);

	return userInput;
}