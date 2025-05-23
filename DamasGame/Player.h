// Definicion de la clase base abstracta Player para el juego de Damas
// Esta clase representa un jugador generico (humano o computadora)
#ifndef PLAYER_H
#define PLAYER_H

#include "CommonTypes.h" // Tipos comunes como PlayerColor, Move, MoveInput
#include "Board.h"       // Para acceder al estado del tablero
#include "MoveGenerator.h" // Para generar movimientos posibles

// Clase base abstracta para representar un jugador
class Player {
public:
	// Constructor que asigna el color al jugador
	// color: color de las piezas de este jugador
	Player(PlayerColor color);
	// Destructor virtual por defecto
	virtual ~Player() = default;

	// Retorna el color de las piezas de este jugador
	PlayerColor GetColor() const;

	// Metodo abstracto para obtener la intencion de movimiento del jugador
	// Debe ser implementado por clases derivadas (humano o computadora)
	// board: estado actual del tablero
	// moveGenerator: referencia al generador de movimientos
	// isInCaptureSequence: indica si el jugador esta en una secuencia de capturas
	// forcedRow, forcedCol: posicion desde donde debe continuar la captura (si aplica)
	// availableMandatoryJumps: lista de movimientos de captura obligatorios
	// Retorna un MoveInput con la jugada elegida
	virtual MoveInput GetChosenMoveInput(
		const Board& board,
		const MoveGenerator& moveGenerator,
		bool isInCaptureSequence,
		int forcedRow,
		int forcedCol,
		const std::vector<Move>& availableMandatoryJumps
	) = 0;

protected:
	PlayerColor m_color; // Color de las piezas de este jugador
};

#endif // PLAYER_H