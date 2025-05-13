#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "CommonTypes.h" 
#include "Board.h"       
#include <vector>

class MoveGenerator {
public:
    MoveGenerator();

    std::vector<Move> GenerateMovesForPiece(const Board& gameBoard,
        int startRow, int startCol) const; // Añadido const

    bool IsValidMove(const Board& gameBoard,
        int startRow, int startCol,
        int endRow, int endCol,
        PlayerColor player,
        bool& wasCapture) const; // Añadido const

    bool HasAnyValidMoves(const Board& gameBoard, PlayerColor player) const; // Añadido const

private:
    void FindSimplePawnMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
    void FindPawnJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
    void FindSimpleKingMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
    void FindKingJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
};

#endif // MOVE_GENERATOR_H