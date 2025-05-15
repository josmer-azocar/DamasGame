#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "CommonTypes.h" // Para Move, PlayerColor, PieceType, std::vector
#include "Board.h"       // Para const Board&

// Enum para definir el tipo de acción que es obligatoria para el jugador actual.
enum class MandatoryActionType {
    NONE,           // No hay capturas obligatorias; se permiten movimientos simples.
    KING_CAPTURE,   // Una captura realizada por una Dama (Rey) es obligatoria.
    PAWN_CAPTURE    // Una captura realizada por un Peón es obligatoria (esto ocurre si ninguna Dama puede capturar pero sí un Peón).
};

// Clase responsable de generar y validar movimientos según las reglas de las Damas.
class MoveGenerator {
public:
    MoveGenerator();

    // Genera todos los movimientos "elementales" para una pieza específica en (startRow, startCol).
    // Da prioridad a los saltos de ESA pieza sobre sus movimientos simples.
    // No considera (aún) las obligaciones globales de captura (como la prioridad de Dama).
    std::vector<Move> GenerateMovesForPiece(const Board& gameBoard,
        int startRow, int startCol) const;

    // Genera SOLO los movimientos de salto (captura) posibles para una pieza específica.
    std::vector<Move> GetPossibleJumpsForSpecificPiece(const Board& gameBoard,
        int pieceRow, int pieceCol) const;

    // Valida si un movimiento propuesto (de start a end) es legal para el jugador dado,
    // considerando todas las reglas, incluida la captura obligatoria y la prioridad de Dama.
    // 'wasCapture' se establece a true si el movimiento validado es una captura.
    bool IsValidMove(const Board& gameBoard,
        int startRow, int startCol,
        int endRow, int endCol,
        PlayerColor player,
        bool& wasCapture) const;

    // Verifica si el jugador dado tiene algún movimiento legal disponible,
    // considerando la captura obligatoria y la prioridad de Dama.
    bool HasAnyValidMoves(const Board& gameBoard, PlayerColor player) const;

    // Determina el tipo de acción obligatoria (NINGUNA, CAPTURA_DAMA, CAPTURA_PEON)
    // para el jugador actual y llena 'outMandatoryMoves' con la lista de
    // TODOS los movimientos que cumplen con esa obligación.
    MandatoryActionType GetMandatoryActionType(const Board& gameBoard, PlayerColor player, std::vector<Move>& outMandatoryMoves) const;

    // Helper para obtener el PlayerColor de una PieceType (movido a público para GameManager)
    PlayerColor GetPlayerFromPiece(PieceType piece) const;

private:
    // Funciones helper para encontrar tipos específicos de movimientos para una pieza.
    void FindSimplePawnMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
    void FindPawnJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
    void FindSimpleKingMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
    void FindKingJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
};

#endif // MOVE_GENERATOR_H