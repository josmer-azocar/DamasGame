#ifndef BOARD_H
#define BOARD_H

#include "CommonTypes.h" // Para PieceType, PlayerColor
#include <array>
#include <vector>        // Para funciones futuras (como GetPossibleMoves)
#include <string>        // Para funciones futuras

class Board {
public:
    static constexpr int BOARD_SIZE = 8; // Equivalente a TAMANO_TABLERO

    // Constructor
    Board();

    // Inicializa el tablero con piezas (basado en la lógica de Miguel)
    // (Corresponds to Miguel's 'inicializarTablero')
    void InitializeBoard();

    // Obtiene el tipo de pieza en una coordenada
    // (Corresponds to accessing tablero[fila][columna] in Miguel's code)
    PieceType GetPieceAt(int row, int col) const;

    // Coloca una pieza en una coordenada y actualiza contadores
    void SetPieceAt(int row, int col, PieceType pieceType);

    // Verifica si las coordenadas están dentro del tablero
    bool IsWithinBounds(int row, int col) const;

    // Verifica si una casilla es jugable (oscura)
    // (Corresponds to Miguel's (fila + columna) % 2 != 0 logic, assuming 0,0 is light)
    bool IsPlayableSquare(int row, int col) const;

    // --- Métodos que ya teníamos y son útiles ---
    int GetPieceCount(PlayerColor player) const;
    int GetKingCount(PlayerColor player) const;
    int GetBoardSize() const { return BOARD_SIZE; }
    void PromotePieceIfNecessary(int row, int col); // Lo dejamos, será útil

    // IMPORTANTE: La función de dibujar (DisplayBoard) NO estará aquí.
    // Se moverá a ConsoleView.

private:
    std::array<std::array<PieceType, BOARD_SIZE>, BOARD_SIZE> mGrid;
    int mPieceCounts[2][2]; // [player_idx][type_idx: 0=MAN, 1=KING]

    void ClearBoard();
    void ResetPieceCounts();
    void UpdateCountsForSetPiece(int r, int c, PieceType oldPiece, PieceType newPiece);
};

#endif // BOARD_H