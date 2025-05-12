// -----------------------------------------------------------------------------
// Board.cpp
// Implementación de la clase Board, adaptando la lógica de Miguel.
// -----------------------------------------------------------------------------

#include "Board.h"       // Incluye nuestra declaración de la clase Board
#include "CommonTypes.h" // Para PieceType, PlayerColor

#include <stdexcept>     // Para std::out_of_range
#include <iostream>      // Para std::cout (si necesitamos depurar)
// NO necesitamos iomanip, sstream, string aquí si DisplayBoard se va.

// NO usaremos "using namespace std;" globalmente.

// --- Implementación del Constructor ---
Board::Board() {
    ClearBoard();
    ResetPieceCounts();
    // InitializeBoard(); // Decidimos llamarlo explícitamente desde main/GameManager
}

// --- Implementación de Métodos Públicos ---

// Adaptación de 'inicializarTablero' de Miguel
void Board::InitializeBoard() {
    ClearBoard();
    ResetPieceCounts();

    // Lógica de Miguel para colocar piezas, adaptada a nuestros PieceType
    // Asumimos que P1 (Negras) empieza arriba, P2 (Blancas) empieza abajo.
    for (int row = 0; row < BOARD_SIZE; ++row) { // 'fila' -> 'row'
        for (int col = 0; col < BOARD_SIZE; ++col) { // 'columna' -> 'col'
            // La condición (row % 2 == col % 2) para casillas claras
            // o (row % 2 != col % 2) para casillas oscuras es otra forma de verlo.
            // Si (0,0) es clara, jugables son (row+col)%2 != 0
            // o (row % 2 != col % 2)
            if ((row % 2) != (col % 2)) { // Casillas jugables (oscuras, si (0,0) es clara)
                // Esta es la forma estándar.
                // La lógica de Miguel (columna = (fila % 2 == 0)) es un poco más enredada
                // pero logra el mismo efecto de alternar. Usemos la estándar.
                if (row < 3) { // Primeras 3 filas jugables para Jugador 1 (Negras)
                    SetPieceAt(row, col, PieceType::P1_MAN);
                }
                else if (row >= BOARD_SIZE - 3) { // Últimas 3 filas jugables para Jugador 2 (Blancas)
                    SetPieceAt(row, col, PieceType::P2_MAN);
                }
            }
        }
    }
}

PieceType Board::GetPieceAt(int row, int col) const {
    if (!IsWithinBounds(row, col)) {
        return PieceType::EMPTY;
    }
    return mGrid[row][col];
}

void Board::SetPieceAt(int row, int col, PieceType pieceType) {
    if (!IsWithinBounds(row, col)) {
        throw std::out_of_range("Board::SetPieceAt: Coordenadas fuera de rango.");
    }
    PieceType oldPiece = mGrid[row][col];
    mGrid[row][col] = pieceType;
    UpdateCountsForSetPiece(row, col, oldPiece, pieceType);
}

bool Board::IsWithinBounds(int row, int col) const {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

bool Board::IsPlayableSquare(int row, int col) const {
    if (!IsWithinBounds(row, col)) {
        return false;
    }
    // Si (0,0) es casilla CLARA (no jugable), entonces las jugables (oscuras) son (row + col) % 2 != 0
    // O equivalentemente, row % 2 != col % 2
    return (row % 2) != (col % 2);
}

int Board::GetPieceCount(PlayerColor player) const {
    int playerIndex = (player == PlayerColor::PLAYER_1) ? 0 : ((player == PlayerColor::PLAYER_2) ? 1 : -1);
    if (playerIndex == -1) return 0;
    return mPieceCounts[playerIndex][0] + mPieceCounts[playerIndex][1];
}

int Board::GetKingCount(PlayerColor player) const {
    int playerIndex = (player == PlayerColor::PLAYER_1) ? 0 : ((player == PlayerColor::PLAYER_2) ? 1 : -1);
    if (playerIndex == -1) return 0;
    return mPieceCounts[playerIndex][1];
}

void Board::PromotePieceIfNecessary(int row, int col) {
    if (!IsWithinBounds(row, col)) {
        return;
    }
    PieceType currentPiece = GetPieceAt(row, col);

    // P1_MAN (Negras, arriba) corona en BOARD_SIZE - 1 (fila 7)
    if (currentPiece == PieceType::P1_MAN && row == BOARD_SIZE - 1) {
        SetPieceAt(row, col, PieceType::P1_KING);
    }
    // P2_MAN (Blancas, abajo) corona en 0
    else if (currentPiece == PieceType::P2_MAN && row == 0) {
        SetPieceAt(row, col, PieceType::P2_KING);
    }
}

// --- Implementación de Métodos Privados ---
void Board::ClearBoard() {
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            mGrid[r][c] = PieceType::EMPTY;
        }
    }
}

void Board::ResetPieceCounts() {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            mPieceCounts[i][j] = 0;
        }
    }
}

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

// La función DisplayBoard() YA NO ESTÁ AQUÍ. Se moverá a ConsoleView.cpp.
// --- Fin de la Implementación de Board ---