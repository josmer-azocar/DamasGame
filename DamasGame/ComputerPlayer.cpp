#include "ComputerPlayer.h"
#include "CommonTypes.h" 
#include "Board.h"       


#include <vector>
#include <algorithm> 
#include <chrono>    
#include <thread>    


// Constructor de ComputerPlayer
// Inicializa el jugador IA con un color, una referencia al generador de movimientos y la dificultad.
// Tambien inicializa el generador de numeros aleatorios con una semilla basada en el tiempo actual.
ComputerPlayer::ComputerPlayer(PlayerColor color, const MoveGenerator& moveGenerator, int difficulty)
	: Player(color),
	m_difficulty(difficulty),
	m_moveGeneratorRef(moveGenerator) { // Guardar referencia a MoveGenerator
	// Sembrar el generador de numeros aleatorios con la hora actual
	unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
	m_rng.seed(seed);
}

// Funcion de Evaluacion Heuristica
// Devuelve una puntuacion para el tablero desde la perspectiva de 'perspectiveColor'.
// Una puntuacion mas alta es mejor para 'perspectiveColor'.
// Considera el valor de las piezas, su posicion y penalizaciones/bonificaciones simples.
int ComputerPlayer::EvaluateBoardState(const Board& currentBoard, PlayerColor perspectiveColor) const {
	int score = 0;
	PlayerColor opponentColor = (perspectiveColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;

	const int pawnValue = 100; // Valor base de un peon
	const int kingValue = 250; // Valor de una Dama (Rey)
	const int advancementBonusBase = 5; // Bonificacion por cada fila avanzada
	const int edgePenalty = -10; // Pequena penalizacion por estar en el borde (menos movilidad)

	// Recorre todas las casillas del tablero
	for (int r = 0; r < currentBoard.GetBoardSize(); ++r) {
		for (int c = 0; c < currentBoard.GetBoardSize(); ++c) {
			PieceType piece = currentBoard.GetPieceAt(r, c);
			if (piece == PieceType::EMPTY) continue; // Ignora casillas vacias

			// Determina el dueno de la pieza usando el generador de movimientos
			PlayerColor pieceOwner = m_moveGeneratorRef.GetPlayerFromPiece(piece);
			int pieceBaseValue = 0;
			int positionalBonus = 0;

			if (piece == PieceType::P1_MAN || piece == PieceType::P2_MAN) {
				pieceBaseValue = pawnValue;
				// Bonificacion por peones avanzados
				if (pieceOwner == PlayerColor::PLAYER_1) { // Negras, avanzan hacia filas mayores
					positionalBonus += advancementBonusBase * r;
				}
				else { // PlayerColor::PLAYER_2 (Blancas), avanzan hacia filas menores
					positionalBonus += advancementBonusBase * (currentBoard.GetBoardSize() - 1 - r);
				}
			}
			else { // Es Dama (Rey)
				pieceBaseValue = kingValue;
				// Las damas controlan mas el centro, pero es mas complejo de evaluar simplemente
			}

			// Penalizacion si esta en un borde lateral (columnas 0 o ultima)
			if (c == 0 || c == (currentBoard.GetBoardSize() - 1)) {
				positionalBonus += edgePenalty;
			}
			// Se pueden anadir mas bonificaciones/penalizaciones aqui

			if (pieceOwner == perspectiveColor) { // Pieza de la IA (o del jugador cuya perspectiva evaluamos)
				score += pieceBaseValue + positionalBonus;
			}
			else if (pieceOwner == opponentColor) { // Pieza del oponente
				score -= (pieceBaseValue + positionalBonus);
			}
		}
	}
	return score;
}

// Selecciona el movimiento que realizara la IA en su turno.
// Considera movimientos obligatorios, secuencias de captura y evalua cada posible movimiento usando la heuristica.
// Devuelve un MoveInput con el movimiento elegido o invalido si no hay movimientos posibles.
MoveInput ComputerPlayer::GetChosenMoveInput(
	const Board& board,
	const MoveGenerator& moveGenerator, // Parametro de la interfaz, aunque ya tenemos m_moveGeneratorRef
	bool isInCaptureSequence,
	int forcedRow,
	int forcedCol,
	const std::vector<Move>& availableMandatoryJumpsFromGameManager // Estos ya estan priorizados
) {
	MoveInput chosenAiMove;
	chosenAiMove.isValidFormat = false; // Por defecto, si no se encuentra nada
	chosenAiMove.wantsToExit = false;
	chosenAiMove.wantsToShowStats = false;

	std::vector<Move> candidateMoves;

	if (isInCaptureSequence) {
		// Si esta en una secuencia de captura, solo puede mover la pieza obligada
		candidateMoves = availableMandatoryJumpsFromGameManager;
	}
	else {
		// No esta en secuencia, availableMandatoryJumpsFromGameManager contendra
		// TODOS los saltos obligatorios (Dama > Peon). Si esta vacio, no hay capturas obligatorias.
		if (!availableMandatoryJumpsFromGameManager.empty()) {
			candidateMoves = availableMandatoryJumpsFromGameManager;
		}
		else {
			// No hay capturas obligatorias, generar todos los movimientos simples posibles.
			for (int r = 0; r < board.GetBoardSize(); ++r) {
				for (int c = 0; c < board.GetBoardSize(); ++c) {
					if (m_moveGeneratorRef.GetPlayerFromPiece(board.GetPieceAt(r, c)) == m_color) {
						// Genera movimientos para la pieza si no hay saltos obligatorios
						std::vector<Move> movesForPiece = m_moveGeneratorRef.GenerateMovesForPiece(board, r, c);
						for (const auto& move : movesForPiece) {
							if (!move.isCapture_) { // Solo movimientos simples
								candidateMoves.push_back(move);
							}
						}
					}
				}
			}
		}
	}

	if (candidateMoves.empty()) {
		// No hay movimientos validos (la IA esta bloqueada o es un error si HasAnyValidMoves dijo que si habia)
		return chosenAiMove; // Devuelve isValidFormat = false
	}

	// --- Evaluacion de Movimientos y Seleccion ---
	Move bestMove; // Se inicializara en el primer bucle o si solo hay un candidato
	int bestScore = std::numeric_limits<int>::min(); // El valor mas bajo posible para int

	bool firstMoveEvaluated = false;

	// Evalua cada movimiento candidato simulando el resultado y usando la heuristica
	for (const Move& currentMove : candidateMoves) {
		Board tempBoard = board;

		// Simula el movimiento en un tablero temporal
		PieceType pieceMoved = tempBoard.GetPieceAt(currentMove.startR_, currentMove.startC_);
		tempBoard.SetPieceAt(currentMove.endR_, currentMove.endC_, pieceMoved);
		tempBoard.SetPieceAt(currentMove.startR_, currentMove.startC_, PieceType::EMPTY);
		if (currentMove.isCapture_) {
			// Elimina la pieza capturada
			int capturedR = currentMove.startR_ + (currentMove.endR_ - currentMove.startR_) / 2;
			int capturedC = currentMove.startC_ + (currentMove.endC_ - currentMove.startC_) / 2;
			tempBoard.SetPieceAt(capturedR, capturedC, PieceType::EMPTY);
		}
		tempBoard.PromotePieceIfNecessary(currentMove.endR_, currentMove.endC_);

		int score = EvaluateBoardState(tempBoard, m_color);

		// Desempate aleatorio simple: si dos movimientos tienen la misma mejor puntuacion,
		// hay una probabilidad de elegir el nuevo.
		if (!firstMoveEvaluated || score > bestScore) {
			bestScore = score;
			bestMove = currentMove;
			firstMoveEvaluated = true;
		}
		else if (score == bestScore) {
			// Si las puntuaciones son iguales, elegir al azar (50% de probabilidad de cambiar)
			std::uniform_int_distribution<int> dist(0, 1);
			if (dist(m_rng) == 0) {
				bestMove = currentMove; // Sobrescribir con el nuevo movimiento
			}
		}
	}

	if (!firstMoveEvaluated && !candidateMoves.empty()) {
		// Esto podria pasar si todos los movimientos llevan a un estado con puntuacion min_int,
		// o si hubo un error. Como fallback, tomar el primer movimiento candidato si existe.
		bestMove = candidateMoves[0];
		firstMoveEvaluated = true; // Para marcar que se ha elegido un bestMove
	}

	if (firstMoveEvaluated) { // Si se encontro un mejor movimiento
		chosenAiMove.startRow = bestMove.startR_;
		chosenAiMove.startCol = bestMove.startC_;
		chosenAiMove.endRow = bestMove.endR_;
		chosenAiMove.endCol = bestMove.endC_;
		chosenAiMove.isValidFormat = true;
	}
	else {
		// Si despues de todo, no se pudo determinar un bestMove 
		chosenAiMove.isValidFormat = false;
	}

	// Simula un pequeno retraso para que la IA no sea instantanea
	if (chosenAiMove.isValidFormat) { // Solo retrasar si la IA va a hacer un movimiento
		std::uniform_int_distribution<int> delay_dist(200, 800); // Milisegundos
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_dist(m_rng)));
	}

	return chosenAiMove;
}