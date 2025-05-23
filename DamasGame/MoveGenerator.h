#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "CommonTypes.h" // Para Move, PlayerColor, PieceType, std::vector
#include "Board.h"       // Para const Board&

// Enum para definir el tipo de accion que es obligatoria para el jugador actual.
enum class MandatoryActionType {
	NONE,           // No hay capturas obligatorias; se permiten movimientos simples.
	KING_CAPTURE,   // Una captura realizada por una Dama (Rey) es obligatoria.
	PAWN_CAPTURE    // Una captura realizada por un Peon es obligatoria (esto ocurre si ninguna Dama puede capturar pero si un Peon).
};

// Clase responsable de generar y validar movimientos segun las reglas de las Damas.
class MoveGenerator {
public:
	MoveGenerator();

	std::vector<Move> GenerateMovesForPiece(const Board& gameBoard,
		int startRow, int startCol) const;

	// Genera SOLO los movimientos de salto (captura) posibles para una pieza espec�fica.
	std::vector<Move> GetPossibleJumpsForSpecificPiece(const Board& gameBoard,
		int pieceRow, int pieceCol) const;

	// Valida si un movimiento propuesto (de start a end) es legal para el jugador dado,
	bool IsValidMove(const Board& gameBoard,
		int startRow, int startCol,
		int endRow, int endCol,
		PlayerColor player,
		bool& wasCapture) const;

	// Verifica si el jugador dado tiene algun movimiento legal disponible,
	bool HasAnyValidMoves(const Board& gameBoard, PlayerColor player) const;

	// Determina el tipo de accion obligatoria (NINGUNA, CAPTURA_DAMA, CAPTURA_PEON)
	MandatoryActionType GetMandatoryActionType(const Board& gameBoard, PlayerColor player, std::vector<Move>& outMandatoryMoves) const;

	// Helper para obtener el PlayerColor de una PieceType 
	PlayerColor GetPlayerFromPiece(PieceType piece) const;

private:
	// Funciones helper para encontrar tipos especificos de movimientos para una pieza.
	void FindSimplePawnMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
	void FindPawnJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
	void FindSimpleKingMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
	void FindKingJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const;
};

#endif // MOVE_GENERATOR_H