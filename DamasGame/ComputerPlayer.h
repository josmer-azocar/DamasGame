#ifndef COMPUTER_PLAYER_H
#define COMPUTER_PLAYER_H

#include "Player.h"
#include "MoveGenerator.h" 
#include <random>          // Para std::mt19937 si se quiere aleatoriedad en desempates
#include <vector>          // Para std::vector
#include <limits>          // Para std::numeric_limits

// Clase que representa a un jugador controlado por la computadora (IA)
// Esta clase implementa la logica para que la computadora pueda tomar decisiones de movimiento
// utilizando un generador de movimientos y una funcion heuristica de evaluacion.
class ComputerPlayer : public Player {
public:
	// Constructor de ComputerPlayer
	// color: color asignado al jugador (blanco o negro)
	// moveGenerator: referencia al generador de movimientos para calcular posibles jugadas
	// difficulty: nivel de dificultad de la IA (por defecto 1)
	ComputerPlayer(PlayerColor color, const MoveGenerator& moveGenerator, int difficulty = 1);

	// Metodo que decide el movimiento de la computadora en su turno
	// board: estado actual del tablero
	// moveGenerator: referencia al generador de movimientos
	// isInCaptureSequence: indica si el jugador esta en una secuencia de capturas obligatorias
	// forcedRow, forcedCol: posicion desde donde debe continuar la captura (si aplica)
	// availableMandatoryJumps: lista de movimientos de captura obligatorios disponibles
	// Retorna un MoveInput con la jugada seleccionada por la IA
	MoveInput GetChosenMoveInput(
		const Board& board,
		const MoveGenerator& moveGenerator,
		bool isInCaptureSequence,
		int forcedRow,
		int forcedCol,
		const std::vector<Move>& availableMandatoryJumps
	) override;

private:
	int m_difficulty;                   // Nivel de dificultad de la IA (mayor valor implica mejor evaluacion y profundidad)
	const MoveGenerator& m_moveGeneratorRef; // Referencia constante al generador de movimientos para calcular jugadas

	// Generador de numeros aleatorios para desempates o para dar variedad a la IA
	std::mt19937 m_rng;

	// Funcion heuristica que evalua el estado del tablero desde la perspectiva de un color
	// currentBoard: tablero a evaluar
	// perspectiveColor: color para el cual se evalua la posicion
	// Retorna un valor numerico que representa la ventaja o desventaja
	int EvaluateBoardState(const Board& currentBoard, PlayerColor perspectiveColor) const;
};

#endif // COMPUTER_PLAYER_H