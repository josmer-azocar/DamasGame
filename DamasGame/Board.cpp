#include "Board.h"       
#include "CommonTypes.h" // Para PieceType, PlayerColor

#include <stdexcept>     // Para std::out_of_range
#include <iostream>      // Para std::cout 

// --- Implementacion del Constructor ---
// Inicializa el tablero vacio y resetea los contadores de piezas
Board::Board() {
	ClearBoard();
	ResetPieceCounts();
}

// --- Implementacion de Metodos Publicos ---

// Inicializa el tablero con la posicion inicial de las piezas
void Board::InitializeBoard() {
	ClearBoard();
	ResetPieceCounts();

	// P1 (Blancas) empiezan abajo (filas de indice alto: 5, 6, 7 si 0 es arriba)
	// P2 (Negras) empiezan arriba (filas de indice bajo: 0, 1, 2 si 0 es arriba)
	for (int row = 0; row < BOARD_SIZE; ++row) {
		for (int col = 0; col < BOARD_SIZE; ++col) {
			if ((row % 2) != (col % 2)) { // Casillas jugables (oscuras)
				if (row < 3) { // Primeras 3 filas para Jugador 2 (Negras)
					SetPieceAt(row, col, PieceType::P2_MAN);
				}
				else if (row >= BOARD_SIZE - 3) { // Ultimas 3 filas para Jugador 1 (Blancas)
					SetPieceAt(row, col, PieceType::P1_MAN);
				}
			}
		}
	}
}

// Devuelve el tipo de pieza en la posicion indicada, o EMPTY si esta fuera de rango
PieceType Board::GetPieceAt(int row, int col) const {
	if (!IsWithinBounds(row, col)) {
		return PieceType::EMPTY;
	}
	return mGrid[row][col];
}

// Coloca una pieza en la posicion indicada y actualiza los contadores
void Board::SetPieceAt(int row, int col, PieceType pieceType) {
	if (!IsWithinBounds(row, col)) {
		throw std::out_of_range("Board::SetPieceAt: Coordenadas fuera de rango.");
	}
	PieceType oldPiece = mGrid[row][col];
	mGrid[row][col] = pieceType;
	UpdateCountsForSetPiece(row, col, oldPiece, pieceType);
}

// Verifica si la posicion esta dentro de los limites del tablero
bool Board::IsWithinBounds(int row, int col) const {
	return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

// Verifica si la casilla es jugable (oscura)
bool Board::IsPlayableSquare(int row, int col) const {
	if (!IsWithinBounds(row, col)) {
		return false;
	}
	// Si (0,0) es casilla clara (no jugable), entonces las jugables (oscuras) son (row + col) % 2 != 0
	// O equivalentemente, row % 2 != col % 2
	return (row % 2) != (col % 2);
}

// Devuelve la cantidad total de piezas (peones + damas) de un jugador
int Board::GetPieceCount(PlayerColor player) const {
	int playerIndex = (player == PlayerColor::PLAYER_1) ? 0 : ((player == PlayerColor::PLAYER_2) ? 1 : -1);
	if (playerIndex == -1) return 0;
	return mPieceCounts[playerIndex][0] + mPieceCounts[playerIndex][1];
}

// Devuelve la cantidad de damas (reyes) de un jugador
int Board::GetKingCount(PlayerColor player) const {
	int playerIndex = (player == PlayerColor::PLAYER_1) ? 0 : ((player == PlayerColor::PLAYER_2) ? 1 : -1);
	if (playerIndex == -1) return 0;
	return mPieceCounts[playerIndex][1];
}

// Promueve una pieza a dama si llega a la fila de coronacion
void Board::PromotePieceIfNecessary(int row, int col) {
	if (!IsWithinBounds(row, col)) {
		return;
	}
	PieceType currentPiece = GetPieceAt(row, col);

	// P1_MAN (Blancas) corona en la fila 0 (arriba)
	if (currentPiece == PieceType::P1_MAN && row == 0) {
		SetPieceAt(row, col, PieceType::P1_KING);
	}
	// P2_MAN (Negras) corona en la fila BOARD_SIZE - 1 (abajo)
	else if (currentPiece == PieceType::P2_MAN && row == BOARD_SIZE - 1) {
		SetPieceAt(row, col, PieceType::P2_KING);
	}
}

// --- Implementacion de Metodos Privados ---
// Limpia el tablero, dejando todas las casillas vacias
void Board::ClearBoard() {
	for (int r = 0; r < BOARD_SIZE; ++r) {
		for (int c = 0; c < BOARD_SIZE; ++c) {
			mGrid[r][c] = PieceType::EMPTY;
		}
	}
}

// Resetea los contadores de piezas de ambos jugadores
void Board::ResetPieceCounts() {
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			mPieceCounts[i][j] = 0;
		}
	}
}

// Actualiza los contadores de piezas al cambiar una pieza en el tablero
void Board::UpdateCountsForSetPiece(int r, int c, PieceType oldPiece, PieceType newPiece) {
	int playerIndexOld = -1, pieceTypeIndexOld = -1;
	if (oldPiece == PieceType::P1_MAN) { playerIndexOld = 0; pieceTypeIndexOld = 0; }
	else if (oldPiece == PieceType::P1_KING) { playerIndexOld = 0; pieceTypeIndexOld = 1; }
	else if (oldPiece == PieceType::P2_MAN) { playerIndexOld = 1; pieceTypeIndexOld = 0; }
	else if (oldPiece == PieceType::P2_KING) { playerIndexOld = 1; pieceTypeIndexOld = 1; }

	if (playerIndexOld != -1) {
		mPieceCounts[playerIndexOld][pieceTypeIndexOld]--;
	}

	int playerIndexNew = -1, pieceTypeIndexNew = -1;
	if (newPiece == PieceType::P1_MAN) { playerIndexNew = 0; pieceTypeIndexNew = 0; }
	else if (newPiece == PieceType::P1_KING) { playerIndexNew = 0; pieceTypeIndexNew = 1; }
	else if (newPiece == PieceType::P2_MAN) { playerIndexNew = 1; pieceTypeIndexNew = 0; }
	else if (newPiece == PieceType::P2_KING) { playerIndexNew = 1; pieceTypeIndexNew = 1; }

	if (playerIndexNew != -1) {
		mPieceCounts[playerIndexNew][pieceTypeIndexNew]++;
	}
}
