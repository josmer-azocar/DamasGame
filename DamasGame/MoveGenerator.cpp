// -----------------------------------------------------------------------------
// MoveGenerator.cpp
// Implementación de MoveGenerator.
// -----------------------------------------------------------------------------
#include "MoveGenerator.h"
#include "Board.h"       // Necesario para interactuar con el tablero
#include "CommonTypes.h" // Para los tipos

#include <vector>
#include <cmath> // Para std::abs()

MoveGenerator::MoveGenerator() {
    // Constructor
}

// --- Implementación de Métodos Privados de Ayuda ---
void MoveGenerator::FindSimplePawnMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const {
    if (piece != PieceType::P1_MAN && piece != PieceType::P2_MAN) return;

    int forwardDirection = (player == PlayerColor::PLAYER_1) ? 1 : -1;
    int dCols[] = { -1, 1 };

    for (int dc : dCols) {
        int endRow = r + forwardDirection;
        int endCol = c + dc;

        if (gameBoard.IsWithinBounds(endRow, endCol) && gameBoard.GetPieceAt(endRow, endCol) == PieceType::EMPTY) {
            Move m;
            m.startR_ = r; m.startC_ = c; m.endR_ = endRow; m.endC_ = endCol;
            m.pieceMoved_ = piece; m.isCapture_ = false;
            moves.push_back(m);
        }
    }
}

void MoveGenerator::FindPawnJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const {
    if (piece != PieceType::P1_MAN && piece != PieceType::P2_MAN) return;

    PlayerColor opponentColor = (player == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
    PieceType opponentPawn = (opponentColor == PlayerColor::PLAYER_1) ? PieceType::P1_MAN : PieceType::P2_MAN;
    PieceType opponentKing = (opponentColor == PlayerColor::PLAYER_1) ? PieceType::P1_KING : PieceType::P2_KING;

    int dRows[] = { -1, -1, 1, 1 };
    int dCols[] = { -1, 1, -1, 1 };

    for (int i = 0; i < 4; ++i) {
        int forwardDirection = (player == PlayerColor::PLAYER_1) ? 1 : -1;
        if (dRows[i] != forwardDirection) {
            continue;
        }

        int capturedRow = r + dRows[i];
        int capturedCol = c + dCols[i];
        int landingRow = r + 2 * dRows[i];
        int landingCol = c + 2 * dCols[i];

        if (gameBoard.IsWithinBounds(landingRow, landingCol) &&
            gameBoard.GetPieceAt(landingRow, landingCol) == PieceType::EMPTY) {
            PieceType capturedPieceType = gameBoard.GetPieceAt(capturedRow, capturedCol);
            if (capturedPieceType == opponentPawn || capturedPieceType == opponentKing) {
                Move m;
                m.startR_ = r; m.startC_ = c; m.endR_ = landingRow; m.endC_ = landingCol;
                m.pieceMoved_ = piece; m.isCapture_ = true;
                moves.push_back(m);
            }
        }
    }
}

// --- Implementación de Métodos Públicos ---
std::vector<Move> MoveGenerator::GenerateMovesForPiece(const Board& gameBoard,
    int startRow, int startCol) {
    std::vector<Move> allValidMoves;
    PieceType piece = gameBoard.GetPieceAt(startRow, startCol);
    PlayerColor player = PlayerColor::NONE;

    if (piece == PieceType::P1_MAN || piece == PieceType::P1_KING) player = PlayerColor::PLAYER_1;
    else if (piece == PieceType::P2_MAN || piece == PieceType::P2_KING) player = PlayerColor::PLAYER_2;

    if (player == PlayerColor::NONE || piece == PieceType::EMPTY) return allValidMoves;

    if (piece == PieceType::P1_MAN || piece == PieceType::P2_MAN) {
        FindPawnJumps(gameBoard, startRow, startCol, player, piece, allValidMoves);
        if (allValidMoves.empty()) { // Solo buscar simples si no hay saltos (simplificación de captura obligatoria)
            FindSimplePawnMoves(gameBoard, startRow, startCol, player, piece, allValidMoves);
        }
    }
    // else if (piece == PieceType::P1_KING || piece == PieceType::P2_KING) { ... }

    return allValidMoves;
}

bool MoveGenerator::IsValidMove(const Board& gameBoard,
    int startRow, int startCol,
    int endRow, int endCol,
    PlayerColor player,
    bool& wasCapture) {
    wasCapture = false;

    PieceType pieceAtStart = gameBoard.GetPieceAt(startRow, startCol);
    bool playerOwnsPiece = false;
    if (player == PlayerColor::PLAYER_1 && (pieceAtStart == PieceType::P1_MAN || pieceAtStart == PieceType::P1_KING)) playerOwnsPiece = true;
    if (player == PlayerColor::PLAYER_2 && (pieceAtStart == PieceType::P2_MAN || pieceAtStart == PieceType::P2_KING)) playerOwnsPiece = true;

    if (!playerOwnsPiece) return false;

    std::vector<Move> possibleMoves = GenerateMovesForPiece(gameBoard, startRow, startCol);

    for (const Move& move : possibleMoves) {
        if (move.endR_ == endRow && move.endC_ == endCol) {
            wasCapture = move.isCapture_;
            return true;
        }
    }
    return false;
}

bool MoveGenerator::HasAnyValidMoves(const Board& gameBoard, PlayerColor player) {
    for (int r = 0; r < Board::BOARD_SIZE; ++r) {
        for (int c = 0; c < Board::BOARD_SIZE; ++c) {
            PieceType piece = gameBoard.GetPieceAt(r, c);
            PlayerColor pieceOwner = PlayerColor::NONE;

            if (piece == PieceType::P1_MAN || piece == PieceType::P1_KING) pieceOwner = PlayerColor::PLAYER_1;
            else if (piece == PieceType::P2_MAN || piece == PieceType::P2_KING) pieceOwner = PlayerColor::PLAYER_2;

            if (pieceOwner == player) {
                if (!GenerateMovesForPiece(gameBoard, r, c).empty()) {
                    return true;
                }
            }
        }
    }
    return false;
}