#ifndef BOARD_H
#define BOARD_H

#include "CommonTypes.h" // Para PieceType, PlayerColor
#include <array>
#include <vector>     
#include <string>       

class Board {
public:
	static constexpr int BOARD_SIZE = 8; // Equivalente a TAMANO_TABLERO

	// Constructor
	Board();

	void InitializeBoard();

	// Obtiene el tipo de pieza en una coordenada
	PieceType GetPieceAt(int row, int col) const;

	// Coloca una pieza en una coordenada y actualiza contadores
	void SetPieceAt(int row, int col, PieceType pieceType);

	// Verifica si las coordenadas están dentro del tablero
	bool IsWithinBounds(int row, int col) const;

	// Verifica si una casilla es jugable (oscura)
	bool IsPlayableSquare(int row, int col) const;

	//Métodos 
	int GetPieceCount(PlayerColor player) const;
	int GetKingCount(PlayerColor player) const;
	int GetBoardSize() const { return BOARD_SIZE; }
	void PromotePieceIfNecessary(int row, int col);

private:
	std::array<std::array<PieceType, BOARD_SIZE>, BOARD_SIZE> mGrid;
	int mPieceCounts[2][2];

	void ClearBoard();
	void ResetPieceCounts();
	void UpdateCountsForSetPiece(int r, int c, PieceType oldPiece, PieceType newPiece);
};

#endif // BOARD_H