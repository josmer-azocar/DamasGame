#include "MoveGenerator.h"
#include "Board.h"       // Para Board::BOARD_SIZE y métodos de Board
#include "CommonTypes.h" // Para PieceType, PlayerColor, Move

#include <vector>
#include <algorithm> 

MoveGenerator::MoveGenerator() {
	
}

PlayerColor MoveGenerator::GetPlayerFromPiece(PieceType piece) const {
	if (piece == PieceType::P1_MAN || piece == PieceType::P1_KING) return PlayerColor::PLAYER_1;
	if (piece == PieceType::P2_MAN || piece == PieceType::P2_KING) return PlayerColor::PLAYER_2;
	return PlayerColor::NONE;
}

void MoveGenerator::FindSimplePawnMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const {
	if (piece != PieceType::P1_MAN && piece != PieceType::P2_MAN) return;

	int forwardDirection = (player == PlayerColor::PLAYER_1) ? -1 : 1;
	int dCols[] = { -1, 1 };

	for (int dc : dCols) {
		int endRow = r + forwardDirection;
		int endCol = c + dc;
		if (gameBoard.IsWithinBounds(endRow, endCol) && gameBoard.GetPieceAt(endRow, endCol) == PieceType::EMPTY) {
			moves.push_back({ r, c, endRow, endCol, piece, player, false });
		}
	}
}

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

std::vector<Move> MoveGenerator::GenerateMovesForPiece(const Board& gameBoard, int startRow, int startCol) const {
	std::vector<Move> generatedMoves; // No usar allValidMoves para evitar confusión con validación global
	PieceType piece = gameBoard.GetPieceAt(startRow, startCol);
	PlayerColor player = GetPlayerFromPiece(piece);

	if (player == PlayerColor::NONE || piece == PieceType::EMPTY) {
		return generatedMoves;
	}

	// Una pieza específica siempre debe saltar si puede, antes de hacer un movimiento simple.
	if (piece == PieceType::P1_MAN || piece == PieceType::P2_MAN) {
		FindPawnJumps(gameBoard, startRow, startCol, player, piece, generatedMoves);
		if (generatedMoves.empty()) { // Solo si no hay saltos para ESTA pieza, buscar simples
			FindSimplePawnMoves(gameBoard, startRow, startCol, player, piece, generatedMoves);
		}
	}
	else if (piece == PieceType::P1_KING || piece == PieceType::P2_KING) {
		FindKingJumps(gameBoard, startRow, startCol, player, piece, generatedMoves);
		if (generatedMoves.empty()) { // Solo si no hay saltos para ESTA pieza, buscar simples
			FindSimpleKingMoves(gameBoard, startRow, startCol, player, piece, generatedMoves);
		}
	}
	return generatedMoves;
}

MandatoryActionType MoveGenerator::GetMandatoryActionType(const Board& gameBoard, PlayerColor player, std::vector<Move>& outMandatoryMoves) const {
	outMandatoryMoves.clear();
	std::vector<Move> kingJumps;
	std::vector<Move> pawnJumps;

	for (int r = 0; r < gameBoard.GetBoardSize(); ++r) {
		for (int c = 0; c < gameBoard.GetBoardSize(); ++c) {
			PieceType piece = gameBoard.GetPieceAt(r, c);
			if (GetPlayerFromPiece(piece) == player) {
				std::vector<Move> jumpsForThisPiece; // Temporal para los saltos de la pieza actual
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

bool MoveGenerator::IsValidMove(const Board& gameBoard, int startRow, int startCol, int endRow, int endCol, PlayerColor player, bool& wasCapture) const {
	wasCapture = false;

	if (!gameBoard.IsWithinBounds(startRow, startCol) || !gameBoard.IsWithinBounds(endRow, endCol)) return false;

	PieceType pieceAtStart = gameBoard.GetPieceAt(startRow, startCol);
	if (pieceAtStart == PieceType::EMPTY) return false;
	if (GetPlayerFromPiece(pieceAtStart) != player) return false;

	std::vector<Move> mandatoryGlobalMoves; // Contendrá los movimientos obligatorios según la prioridad
	MandatoryActionType requiredAction = GetMandatoryActionType(gameBoard, player, mandatoryGlobalMoves);

	// El movimiento propuesto (startRow, startCol) -> (endRow, endCol)
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
		return false; // El movimiento ni siquiera es uno que la pieza pueda hacer elementalmente.
	}

	// Ahora, aplicar la lógica de obligatoriedad global
	switch (requiredAction) {
	case MandatoryActionType::KING_CAPTURE:
		// Si se requiere captura de Dama, el movimiento propuesto DEBE ser una captura de Dama.
		// Y DEBE ser uno de los que están en mandatoryGlobalMoves.
		if (concreteProposedMove.isCapture_ &&
			(concreteProposedMove.pieceMoved_ == PieceType::P1_KING || concreteProposedMove.pieceMoved_ == PieceType::P2_KING)) {
			for (const auto& mandatoryKingJump : mandatoryGlobalMoves) {
				if (mandatoryKingJump.startR_ == startRow && mandatoryKingJump.startC_ == startCol &&
					mandatoryKingJump.endR_ == endRow && mandatoryKingJump.endC_ == endCol) {
					wasCapture = true;
					return true;
				}
			}
			return false; // Es una captura de Dama, pero no una de las obligatorias listadas (no debería pasar si mandatoryGlobalMoves es correcto)
		}
		return false; // No es una captura de Dama, o no es captura.

	case MandatoryActionType::PAWN_CAPTURE:
		// Si se requiere captura de Peón (porque ninguna Dama puede), el movimiento propuesto DEBE ser una captura de Peón.
		// Y DEBE ser uno de los que están en mandatoryGlobalMoves.
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
			return false; // No puedes capturar si no hay capturas obligatorias (según GetMandatoryActionType)
		}
		wasCapture = false; // Es un movimiento simple
		return true;        // Y es elementalmente válido para la pieza.
	}
	return false; // Default (no debería alcanzarse)
}

bool MoveGenerator::HasAnyValidMoves(const Board& gameBoard, PlayerColor player) const {
	std::vector<Move> mandatoryMoves; // Para almacenar los movimientos que cumplen la obligación
	MandatoryActionType actionType = GetMandatoryActionType(gameBoard, player, mandatoryMoves);

	if (actionType == MandatoryActionType::KING_CAPTURE || actionType == MandatoryActionType::PAWN_CAPTURE) {
		// Si hay una acción de captura obligatoria (Dama o Peón),
		// y la lista de mandatoryMoves (que GetMandatoryActionType llena) no está vacía,
		// entonces el jugador SÍ tiene movimientos válidos.
		return !mandatoryMoves.empty();
	}

	// Si actionType es NONE (no hay capturas obligatorias de Dama ni de Peón),
	if (actionType == MandatoryActionType::NONE) {
		for (int r = 0; r < gameBoard.GetBoardSize(); ++r) {
			for (int c = 0; c < gameBoard.GetBoardSize(); ++c) {
				PieceType piece = gameBoard.GetPieceAt(r, c);
				if (GetPlayerFromPiece(piece) == player) {
					// Generar movimientos simples para esta pieza
					std::vector<Move> simpleMovesForThisPiece;
					if (piece == PieceType::P1_MAN || piece == PieceType::P2_MAN) {
						FindSimplePawnMoves(gameBoard, r, c, player, piece, simpleMovesForThisPiece);
					}
					else if (piece == PieceType::P1_KING || piece == PieceType::P2_KING) {
						FindSimpleKingMoves(gameBoard, r, c, player, piece, simpleMovesForThisPiece);
					}
					if (!simpleMovesForThisPiece.empty()) {
						return true; // Se encontró al menos un movimiento simple.
					}
				}
			}
		}
	}
	return false; // No se encontraron movimientos válidos según la prioridad.
}