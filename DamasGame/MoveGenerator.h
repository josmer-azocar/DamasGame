#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "CommonTypes.h"
#include "Board.h"
#include <vector>

class MoveGenerator {
public:
    MoveGenerator();

    // Genera todos los movimientos (saltos tienen prioridad) para una pieza
    std::vector<Move> GenerateMovesForPiece(const Board& gameBoard,
        int startRow, int startCol) const;

    // NUEVA FUNCIÓN: Genera solo los saltos posibles para una pieza específica
    std::vector<Move> GetPossibleJumpsForSpecificPiece(const Board& gameBoard,
        int pieceRow, int pieceCol) const;

    bool IsValidMove(const Board& gameBoard,
        int startRow, int startCol,
        int endRow, int endCol,
        PlayerColor player,
        bool& wasCapture) const;

    bool HasAnyValidMoves(const Board& gameBoard, PlayerColor player) const;

public:
    // Estas funciones ahora toman PlayerColor y PieceType para ser más generales
    void FindSimplePawnMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
    void FindPawnJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
    void FindSimpleKingMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
    void FindKingJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
public:
    // Helper para determinar el jugador de una pieza
    PlayerColor GetPlayerFromPiece(PieceType piece) const;
};

#endif // MOVE_GENERATOR_H