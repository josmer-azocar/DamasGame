#include "MoveGenerator.h"
#include "Board.h"       // Para Board::BOARD_SIZE y metodos de Board
#include "CommonTypes.h" // Para PieceType, PlayerColor, Move

#include <vector>
#include <algorithm> 

// Constructor de MoveGenerator
MoveGenerator::MoveGenerator() {
	// No requiere inicializacion especial
}

// Retorna el color del jugador segun el tipo de pieza
PlayerColor MoveGenerator::GetPlayerFromPiece(PieceType piece) const {
	if (piece == PieceType::P1_MAN || piece == PieceType::P1_KING) return PlayerColor::PLAYER_1;
	if (piece == PieceType::P2_MAN || piece == PieceType::P2_KING) return PlayerColor::PLAYER_2;
	return PlayerColor::NONE;
}

// Busca movimientos simples (sin captura) para un peon
void MoveGenerator::FindSimplePawnMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const {
	if (piece != PieceType::P1_MAN && piece != PieceType::P2_MAN) return;

	int forwardDirection = (player == PlayerColor::PLAYER_1) ? -1 : 1;
	int dCols[] = { -1, 1 };

	for (int dc : dCols) {
		int endRow = r + forwardDirection;
		int endCol = c + dc;
		// Solo agrega el movimiento si la casilla destino esta vacia
		if (gameBoard.IsWithinBounds(endRow, endCol) && gameBoard.GetPieceAt(endRow, endCol) == PieceType::EMPTY) {
			moves.push_back({ r, c, endRow, endCol, piece, player, false });
		}
	}
}

// Busca movimientos de salto (captura) para un peon
void MoveGenerator::FindPawnJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const {
	if (piece != PieceType::P1_MAN && piece != PieceType::P2_MAN) return;

	PlayerColor opponentColor = (player == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
	PieceType opponentPawn = (opponentColor == PlayerColor::PLAYER_1) ? PieceType::P1_MAN : PieceType::P2_MAN;
	PieceType opponentKing = (opponentColor == PlayerColor::PLAYER_1) ? PieceType::P1_KING : PieceType::P2_KING;

	int forwardDirection = (player == PlayerColor::PLAYER_1) ? -1 : 1;
	int jumpOffsets[][2] = { {forwardDirection, -1}, {forwardDirection, 1} };

	for (const auto& offset : jumpOffsets) {
		int capturedRow = r + offset[0];
		int capturedCol = c + offset[1];
		int landingRow = r + 2 * offset[0];
		int landingCol = c + 2 * offset[1];

		if (gameBoard.IsWithinBounds(landingRow, landingCol) &&
			gameBoard.GetPieceAt(landingRow, landingCol) == PieceType::EMPTY) {
			if (gameBoard.IsWithinBounds(capturedRow, capturedCol)) {
				PieceType pieceAtCapturedPos = gameBoard.GetPieceAt(capturedRow, capturedCol);
				if (pieceAtCapturedPos == opponentPawn || pieceAtCapturedPos == opponentKing) {
					moves.push_back({ r, c, landingRow, landingCol, piece, player, true });
				}
			}
		}
	}
}

// Busca movimientos simples (sin captura) para una dama
void MoveGenerator::FindSimpleKingMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const {
	if (piece != PieceType::P1_KING && piece != PieceType::P2_KING) return;

	int dRows[] = { -1, -1,  1,  1 };
	int dCols[] = { -1,  1, -1,  1 };

	for (int i = 0; i < 4; ++i) {
		int endRow = r + dRows[i];
		int endCol = c + dCols[i];
		if (gameBoard.IsWithinBounds(endRow, endCol) && gameBoard.GetPieceAt(endRow, endCol) == PieceType::EMPTY) {
			moves.push_back({ r, c, endRow, endCol, piece, player, false });
		}
	}
}

// Busca movimientos de salto (captura) para una dama
void MoveGenerator::FindKingJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const {
	if (piece != PieceType::P1_KING && piece != PieceType::P2_KING) return;

	PlayerColor opponentColor = (player == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
	PieceType opponentPawn = (opponentColor == PlayerColor::PLAYER_1) ? PieceType::P1_MAN : PieceType::P2_MAN;
	PieceType opponentKing = (opponentColor == PlayerColor::PLAYER_1) ? PieceType::P1_KING : PieceType::P2_KING;

	int dRows[] = { -1, -1,  1,  1 };
	int dCols[] = { -1,  1, -1,  1 };

	for (int i = 0; i < 4; ++i) {
		int capturedRow = r + dRows[i];
		int capturedCol = c + dCols[i];
		int landingRow = r + 2 * dRows[i];
		int landingCol = c + 2 * dCols[i];

		if (gameBoard.IsWithinBounds(landingRow, landingCol) &&
			gameBoard.GetPieceAt(landingRow, landingCol) == PieceType::EMPTY) {
			if (gameBoard.IsWithinBounds(capturedRow, capturedCol)) {
				PieceType pieceAtCapturedPos = gameBoard.GetPieceAt(capturedRow, capturedCol);
				if (pieceAtCapturedPos == opponentPawn || pieceAtCapturedPos == opponentKing) {
					moves.push_back({ r, c, landingRow, landingCol, piece, player, true });
				}
			}
		}
	}
}

// Retorna todos los saltos posibles para una pieza especifica
std::vector<Move> MoveGenerator::GetPossibleJumpsForSpecificPiece(const Board& gameBoard, int pieceRow, int pieceCol) const {
	std::vector<Move> jumps;
	PieceType piece = gameBoard.GetPieceAt(pieceRow, pieceCol);
	PlayerColor player = GetPlayerFromPiece(piece);

	if (player == PlayerColor::NONE || piece == PieceType::EMPTY) {
		return jumps;
	}

	if (piece == PieceType::P1_MAN || piece == PieceType::P2_MAN) {
		FindPawnJumps(gameBoard, pieceRow, pieceCol, player, piece, jumps);
	}
	else if (piece == PieceType::P1_KING || piece == PieceType::P2_KING) {
		FindKingJumps(gameBoard, pieceRow, pieceCol, player, piece, jumps);
	}
	return jumps;
}

// Genera todos los movimientos posibles (simples y saltos) para una pieza
std::vector<Move> MoveGenerator::GenerateMovesForPiece(const Board& gameBoard, int startRow, int startCol) const {
	std::vector<Move> generatedMoves;
	PieceType piece = gameBoard.GetPieceAt(startRow, startCol);
	PlayerColor player = GetPlayerFromPiece(piece);

	if (player == PlayerColor::NONE || piece == PieceType::EMPTY) {
		return generatedMoves;
	}

	// Si la pieza puede saltar, solo se consideran los saltos
	if (piece == PieceType::P1_MAN || piece == PieceType::P2_MAN) {
		FindPawnJumps(gameBoard, startRow, startCol, player, piece, generatedMoves);
		if (generatedMoves.empty()) {
			FindSimplePawnMoves(gameBoard, startRow, startCol, player, piece, generatedMoves);
		}
	}
	else if (piece == PieceType::P1_KING || piece == PieceType::P2_KING) {
		FindKingJumps(gameBoard, startRow, startCol, player, piece, generatedMoves);
		if (generatedMoves.empty()) {
			FindSimpleKingMoves(gameBoard, startRow, startCol, player, piece, generatedMoves);
		}
	}
	return generatedMoves;
}

// Determina el tipo de accion obligatoria (ninguna, captura dama, captura peon) y llena la lista de movimientos obligatorios
MandatoryActionType MoveGenerator::GetMandatoryActionType(const Board& gameBoard, PlayerColor player, std::vector<Move>& outMandatoryMoves) const {
	outMandatoryMoves.clear();
	std::vector<Move> kingJumps;
	std::vector<Move> pawnJumps;

	for (int r = 0; r < gameBoard.GetBoardSize(); ++r) {
		for (int c = 0; c < gameBoard.GetBoardSize(); ++c) {
			PieceType piece = gameBoard.GetPieceAt(r, c);
			if (GetPlayerFromPiece(piece) == player) {
				std::vector<Move> jumpsForThisPiece;
				if (piece == PieceType::P1_KING || piece == PieceType::P2_KING) {
					FindKingJumps(gameBoard, r, c, player, piece, jumpsForThisPiece);
					kingJumps.insert(kingJumps.end(), jumpsForThisPiece.begin(), jumpsForThisPiece.end());
				}
				else if (piece == PieceType::P1_MAN || piece == PieceType::P2_MAN) {
					FindPawnJumps(gameBoard, r, c, player, piece, jumpsForThisPiece);
					pawnJumps.insert(pawnJumps.end(), jumpsForThisPiece.begin(), jumpsForThisPiece.end());
				}
			}
		}
	}

	if (!kingJumps.empty()) {
		outMandatoryMoves = kingJumps;
		return MandatoryActionType::KING_CAPTURE;
	}
	if (!pawnJumps.empty()) {
		outMandatoryMoves = pawnJumps;
		return MandatoryActionType::PAWN_CAPTURE;
	}

	return MandatoryActionType::NONE;
}

// Valida si un movimiento propuesto es legal segun las reglas y la obligatoriedad
bool MoveGenerator::IsValidMove(const Board& gameBoard, int startRow, int startCol, int endRow, int endCol, PlayerColor player, bool& wasCapture) const {
	wasCapture = false;

	if (!gameBoard.IsWithinBounds(startRow, startCol) || !gameBoard.IsWithinBounds(endRow, endCol)) return false;

	PieceType pieceAtStart = gameBoard.GetPieceAt(startRow, startCol);
	if (pieceAtStart == PieceType::EMPTY) return false;
	if (GetPlayerFromPiece(pieceAtStart) != player) return false;

	std::vector<Move> mandatoryGlobalMoves;
	MandatoryActionType requiredAction = GetMandatoryActionType(gameBoard, player, mandatoryGlobalMoves);

	// Busca si el movimiento propuesto es uno de los posibles para la pieza
	std::vector<Move> elementalMovesForPiece = GenerateMovesForPiece(gameBoard, startRow, startCol);

	Move concreteProposedMove;
	bool isElementalMove = false;
	for (const auto& elementalMove : elementalMovesForPiece) {
		if (elementalMove.endR_ == endRow && elementalMove.endC_ == endCol) {
			concreteProposedMove = elementalMove;
			isElementalMove = true;
			break;
		}
	}

	if (!isElementalMove) {
		return false;
	}

	// Aplica la logica de obligatoriedad global
	switch (requiredAction) {
	case MandatoryActionType::KING_CAPTURE:
		if (concreteProposedMove.isCapture_ &&
			(concreteProposedMove.pieceMoved_ == PieceType::P1_KING || concreteProposedMove.pieceMoved_ == PieceType::P2_KING)) {
			for (const auto& mandatoryKingJump : mandatoryGlobalMoves) {
				if (mandatoryKingJump.startR_ == startRow && mandatoryKingJump.startC_ == startCol &&
					mandatoryKingJump.endR_ == endRow && mandatoryKingJump.endC_ == endCol) {
					wasCapture = true;
					return true;
				}
			}
			return false;
		}
		return false;

	case MandatoryActionType::PAWN_CAPTURE:
		if (concreteProposedMove.isCapture_ &&
			(concreteProposedMove.pieceMoved_ == PieceType::P1_MAN || concreteProposedMove.pieceMoved_ == PieceType::P2_MAN)) {
			for (const auto& mandatoryPawnJump : mandatoryGlobalMoves) {
				if (mandatoryPawnJump.startR_ == startRow && mandatoryPawnJump.startC_ == startCol &&
					mandatoryPawnJump.endR_ == endRow && mandatoryPawnJump.endC_ == endCol) {
					wasCapture = true;
					return true;
				}
			}
			return false;
		}
		return false;

	case MandatoryActionType::NONE:
		if (concreteProposedMove.isCapture_) {
			return false;
		}
		wasCapture = false;
		return true;
	}
	return false;
}

// Verifica si el jugador tiene al menos un movimiento legal disponible
// Retorna true si existe al menos un movimiento permitido segun las reglas y la obligatoriedad
// Si hay capturas obligatorias, solo se consideran esos movimientos
// Si no hay capturas obligatorias, busca movimientos simples para cada pieza del jugador
bool MoveGenerator::HasAnyValidMoves(const Board& gameBoard, PlayerColor player) const {
	std::vector<Move> mandatoryMoves; // Almacena los movimientos obligatorios encontrados
	MandatoryActionType actionType = GetMandatoryActionType(gameBoard, player, mandatoryMoves);

	// Si hay capturas obligatorias (dama o peon), basta con que la lista no este vacia
	if (actionType == MandatoryActionType::KING_CAPTURE || actionType == MandatoryActionType::PAWN_CAPTURE) {
		return !mandatoryMoves.empty();
	}

	// Si no hay capturas obligatorias, buscar movimientos simples para cada pieza del jugador
	if (actionType == MandatoryActionType::NONE) {
		for (int r = 0; r < gameBoard.GetBoardSize(); ++r) {
			for (int c = 0; c < gameBoard.GetBoardSize(); ++c) {
				PieceType piece = gameBoard.GetPieceAt(r, c);
				// Solo considerar piezas del jugador actual
				if (GetPlayerFromPiece(piece) == player) {
					std::vector<Move> simpleMovesForThisPiece; // Movimientos simples para la pieza actual
					if (piece == PieceType::P1_MAN || piece == PieceType::P2_MAN) {
						// Buscar movimientos simples de peon
						FindSimplePawnMoves(gameBoard, r, c, player, piece, simpleMovesForThisPiece);
					}
					else if (piece == PieceType::P1_KING || piece == PieceType::P2_KING) {
						// Buscar movimientos simples de dama
						FindSimpleKingMoves(gameBoard, r, c, player, piece, simpleMovesForThisPiece);
					}
					// Si se encontro al menos un movimiento simple, el jugador puede mover
					if (!simpleMovesForThisPiece.empty()) {
						return true;
					}
				}
			}
		}
	}
	// Si no se encontro ningun movimiento valido, retorna false
	return false;
}