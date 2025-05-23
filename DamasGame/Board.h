#ifndef BOARD_H
#define BOARD_H

#include "CommonTypes.h" // Para PieceType, PlayerColor
#include <array>
#include <vector>     
#include <string>       

class Board {
public:
	static constexpr int BOARD_SIZE = 8; // Tamano del tablero (8x8)

	// Constructor: inicializa el tablero y los contadores
	Board();

	// Inicializa el tablero con la posicion inicial de las piezas
	void InitializeBoard();

	// Devuelve el tipo de pieza en una coordenada
	PieceType GetPieceAt(int row, int col) const;

	// Coloca una pieza en una coordenada y actualiza los contadores
	void SetPieceAt(int row, int col, PieceType pieceType);

	// Verifica si las coordenadas estan dentro del tablero
	bool IsWithinBounds(int row, int col) const;

	// Verifica si una casilla es jugable (oscura)
	bool IsPlayableSquare(int row, int col) const;

	// Devuelve la cantidad total de piezas (peones + damas) de un jugador
	int GetPieceCount(PlayerColor player) const;
	// Devuelve la cantidad de damas (reyes) de un jugador
	int GetKingCount(PlayerColor player) const;
	// Devuelve el tamano del tablero
	int GetBoardSize() const { return BOARD_SIZE; }
	// Promueve una pieza a dama si llega a la fila de coronacion
	void PromotePieceIfNecessary(int row, int col);

private:
	// Matriz que representa el tablero y el tipo de pieza en cada casilla
	std::array<std::array<PieceType, BOARD_SIZE>, BOARD_SIZE> mGrid;
	// Contadores de piezas: [jugador][tipo] -> [0=peon, 1=dama]
	int mPieceCounts[2][2];

	// Limpia el tablero, dejando todas las casillas vacias
	void ClearBoard();
	// Resetea los contadores de piezas de ambos jugadores
	void ResetPieceCounts();
	// Actualiza los contadores de piezas al cambiar una pieza en el tablero
	void UpdateCountsForSetPiece(int r, int c, PieceType oldPiece, PieceType newPiece);
};

#endif // BOARD_H