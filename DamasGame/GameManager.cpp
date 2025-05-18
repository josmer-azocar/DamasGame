#include "GameManager.h"
#include "ConsoleView.h"
#include "InputHandler.h" 
#include "Board.h"
#include "CommonTypes.h"
#include "MoveGenerator.h"
#include "Player.h"         
#include "HumanPlayer.h"    
#include "ComputerPlayer.h" 

#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <cmath>     
#include <memory>    
#include <thread> 
#include <chrono>  
#include <algorithm>
#include <iomanip> 
#include <sstream> 

const int GAME_TITLE_LINES = 4;
const int BOARD_VISUAL_HEIGHT = 1 + 1 + (Board::BOARD_SIZE * 2 - 1) + 1;
const int CONSOLE_WIDTH_ASSUMED = 80;

GameManager::GameManager(Board& board, ConsoleView& view, InputHandler& inputHandler)
	: mGameBoard(board),
	mView(view),
	mInputHandler(inputHandler),
	mMoveGenerator(),
	mFileHandler(),
	mCurrentPlayerTurnColor(PlayerColor::PLAYER_1),
	mIsGameOver(false),
	mLastMove(),
	mInCaptureSequence(false),
	mForcedPieceRow(-1),
	mForcedPieceCol(-1),
	mCurrentGameMode(GameMode::NONE),
	m_player1(nullptr),
	m_player2(nullptr),
	m_currentPlayerObject(nullptr)
{
}

GameManager::~GameManager() {
}

std::string GameManager::getCurrentDateTime(const std::string& format_string) {
	auto now = std::chrono::system_clock::now();
	std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
	std::tm now_tm;

#ifdef _WIN32
	localtime_s(&now_tm, &now_time_t);
#else
	now_tm = *std::localtime(&now_time_t);
#endif

	std::ostringstream oss;
	oss << std::put_time(&now_tm, format_string.c_str());
	return oss.str();
}

void GameManager::InitializeApplication() { ShowMainMenu(); }
void GameManager::ShowMainMenu() {
	int selectedOption = 1;
	const int numMenuOptions = 5;
	bool exitMenu = false;
	while (!exitMenu) {
		mView.SetMenuColorsAndClear();
		mView.DisplayMainMenu(selectedOption);
		int choice = mInputHandler.GetMenuChoice(selectedOption, numMenuOptions);
		if (choice > 0) {
			selectedOption = choice;
			bool gameModeSelected = false;
			switch (selectedOption) {
			case 1: mCurrentGameMode = GameMode::PLAYER_VS_PLAYER; gameModeSelected = true; break;
			case 2: mCurrentGameMode = GameMode::PLAYER_VS_COMPUTER; gameModeSelected = true; break;
			case 3: mCurrentGameMode = GameMode::COMPUTER_VS_COMPUTER; gameModeSelected = true; break;
			case 4: ShowGlobalStats(); break;
			case 5: exitMenu = true; break;
			}
			if (gameModeSelected) {
				mView.SetGameColorsAndClear();
				StartNewGame(); RunGameLoop();
			}
		}
		else if (choice < 0) { selectedOption = -choice; }
	}
}
void GameManager::ShowGlobalStats() {
	mView.SetMenuColorsAndClear();
	mFileHandler.displayGameHistory();
	mView.DisplayMessage("\nPresione Enter para volver al menu principal...", true);
	if (std::cin.peek() == '\n') std::cin.ignore();
	std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}
void GameManager::StartNewGame() {
	mGameBoard.InitializeBoard();
	mCurrentPlayerTurnColor = PlayerColor::PLAYER_1;
	mIsGameOver = false; mGameStats = GameStats{}; mLastMove = Move{};
	mInCaptureSequence = false; mForcedPieceRow = -1; mForcedPieceCol = -1;
	switch (mCurrentGameMode) {
	case GameMode::PLAYER_VS_PLAYER:
		m_player1 = std::make_unique<HumanPlayer>(PlayerColor::PLAYER_1, mInputHandler, mView);
		m_player2 = std::make_unique<HumanPlayer>(PlayerColor::PLAYER_2, mInputHandler, mView); break;
	case GameMode::PLAYER_VS_COMPUTER:
		m_player1 = std::make_unique<HumanPlayer>(PlayerColor::PLAYER_1, mInputHandler, mView);
		m_player2 = std::make_unique<ComputerPlayer>(PlayerColor::PLAYER_2, mMoveGenerator); break;
	case GameMode::COMPUTER_VS_COMPUTER:
		m_player1 = std::make_unique<ComputerPlayer>(PlayerColor::PLAYER_1, mMoveGenerator);
		m_player2 = std::make_unique<ComputerPlayer>(PlayerColor::PLAYER_2, mMoveGenerator); break;
	default:
		mView.DisplayMessage("Error: Modo de juego invalido. Iniciando Jugador vs Jugador.", true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
		if (std::cin.peek() == '\n') std::cin.ignore();
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		mCurrentGameMode = GameMode::PLAYER_VS_PLAYER;
		m_player1 = std::make_unique<HumanPlayer>(PlayerColor::PLAYER_1, mInputHandler, mView);
		m_player2 = std::make_unique<HumanPlayer>(PlayerColor::PLAYER_2, mInputHandler, mView); break;
	}
	m_currentPlayerObject = m_player1.get();
}
void GameManager::RunGameLoop() {
	if (mCurrentGameMode == GameMode::NONE || !m_player1 || !m_player2) {
		mView.DisplayMessage("Error: Juego no inicializado correctamente. Volviendo al menu.", true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
		std::cin.get(); return;
	}
	GoToXY(0, 0); mView.ClearLines(0, GAME_TITLE_LINES, CONSOLE_WIDTH_ASSUMED); GoToXY(0, 0);
	std::string modeTitle = "=== JUEGO DE DAMAS: ";
	if (mCurrentGameMode == GameMode::PLAYER_VS_PLAYER) modeTitle += "JUGADOR VS JUGADOR";
	else if (mCurrentGameMode == GameMode::PLAYER_VS_COMPUTER) modeTitle += "JUGADOR VS COMPUTADORA";
	else if (mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER) modeTitle += "COMPUTADORA VS COMPUTADORA";
	modeTitle += " ===";
	mView.DisplayMessage(modeTitle, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage("Reglas: Capturas multiples. Dama prioridad. Blancas (w) empiezan.", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage("Comandos: 'Origen Destino' (ej: a3 b4), 'stats', 'salir'.", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage("--------------------------------------------------", true, CONSOLE_COLOR_WHITE, CONSOLE_COLOR_BLACK);
	mIsGameOver = false;
	while (!mIsGameOver) { ProcessPlayerTurn(); } AnnounceResult();
}
void GameManager::DisplayCurrentStats() {
	int statsY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 7;
	GoToXY(0, statsY); mView.ClearLines(statsY, 6, CONSOLE_WIDTH_ASSUMED); GoToXY(0, statsY);
	mView.DisplayMessage("--- Estadisticas Actuales ---", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage("Turno Actual Nro: " + std::to_string(mGameStats.currentTurnNumber), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	int p1p = mGameBoard.GetPieceCount(PlayerColor::PLAYER_1), p1k = mGameBoard.GetKingCount(PlayerColor::PLAYER_1), p1m = p1p - p1k;
	int p2p = mGameBoard.GetPieceCount(PlayerColor::PLAYER_2), p2k = mGameBoard.GetKingCount(PlayerColor::PLAYER_2), p2m = p2p - p2k;
	mView.DisplayMessage(PlayerColorToString(PlayerColor::PLAYER_1) + ": " + std::to_string(p1p) + " (" + std::to_string(p1m) + "p, " + std::to_string(p1k) + "D). Capt: " + std::to_string(mGameStats.player1CapturedCount), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage(PlayerColorToString(PlayerColor::PLAYER_2) + ": " + std::to_string(p2p) + " (" + std::to_string(p2m) + "p, " + std::to_string(p2k) + "D). Capt: " + std::to_string(mGameStats.player2CapturedCount), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage("-----------------------------", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
}
void GameManager::DisplayLastMove() {
	int lastMoveY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT;
	GoToXY(0, lastMoveY); mView.ClearLines(lastMoveY, 1, CONSOLE_WIDTH_ASSUMED); GoToXY(0, lastMoveY);
	if (!mLastMove.IsNull()) { mView.DisplayMessage("Ultimo movimiento: " + mLastMove.ToNotation(), true, CONSOLE_COLOR_WHITE, CONSOLE_COLOR_BLACK); }
}

void GameManager::ProcessPlayerTurn() {
	bool turnActionSuccessfullyCompleted = false;
	while (!turnActionSuccessfullyCompleted && !mIsGameOver) {
		GoToXY(0, GAME_TITLE_LINES); mView.ClearLines(GAME_TITLE_LINES, BOARD_VISUAL_HEIGHT + 15, CONSOLE_WIDTH_ASSUMED); GoToXY(0, GAME_TITLE_LINES);
		mView.DisplayBoard(mGameBoard, CONSOLE_COLOR_BLACK); DisplayLastMove();
		int turnMessageY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 1; GoToXY(0, turnMessageY);
		std::vector<Move> mandatoryJumpsForCurrentPlayer; // Va a ser llenada por GetMandatoryActionType si hay saltos obligatorios
		MandatoryActionType currentMandatoryAction; // Va a ser determinada por GetMandatoryActionType

		if (!mInCaptureSequence) {
			currentMandatoryAction = mMoveGenerator.GetMandatoryActionType(mGameBoard, mCurrentPlayerTurnColor, mandatoryJumpsForCurrentPlayer);
		}
		else {
			mandatoryJumpsForCurrentPlayer = mMoveGenerator.GetPossibleJumpsForSpecificPiece(mGameBoard, mForcedPieceRow, mForcedPieceCol);
			PieceType forcedPiece = mGameBoard.GetPieceAt(mForcedPieceRow, mForcedPieceCol);
			currentMandatoryAction = (forcedPiece == PieceType::P1_KING || forcedPiece == PieceType::P2_KING) ? MandatoryActionType::KING_CAPTURE : MandatoryActionType::PAWN_CAPTURE;
		}

		if (!mInCaptureSequence && !mMoveGenerator.HasAnyValidMoves(mGameBoard, mCurrentPlayerTurnColor)) {
			mView.DisplayMessage("El jugador " + PlayerColorToString(mCurrentPlayerTurnColor) + " no tiene movimientos validos.", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
			mGameStats.winner = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
			mGameStats.reason = GameOverReason::NO_MOVES; mIsGameOver = true; turnActionSuccessfullyCompleted = true; continue;
		}
		std::string turnMsg = "Turno de " + PlayerColorToString(mCurrentPlayerTurnColor) + ". ";
		if (mInCaptureSequence) { turnMsg += "CONTINUAR CAPTURA con la pieza en " + ToAlgebraic(mForcedPieceRow, mForcedPieceCol) + "."; }
		mView.DisplayMessage(turnMsg, true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_BLACK);

		if (mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER && dynamic_cast<ComputerPlayer*>(m_currentPlayerObject)) {
			GoToXY(0, turnMessageY + 1); mView.ClearLines(turnMessageY + 1, 2, CONSOLE_WIDTH_ASSUMED); GoToXY(0, turnMessageY + 1);
			mView.DisplayMessage("IA (" + PlayerColorToString(mCurrentPlayerTurnColor) + ") va a mover. Presione ENTER o ingrese 'stats'/'salir':", false, CONSOLE_COLOR_DARK_GRAY, CONSOLE_COLOR_BLACK);
			std::cout << std::endl << "> "; std::string cvc_cmd; if (std::cin.peek() == '\n') std::cin.ignore(); std::getline(std::cin, cvc_cmd);
			std::transform(cvc_cmd.begin(), cvc_cmd.end(), cvc_cmd.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			if (cvc_cmd == "salir") {
				mView.DisplayMessage("Saliendo de la partida CvC por comando del observador...", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
				mIsGameOver = true; mGameStats.reason = GameOverReason::PLAYER_EXIT; mGameStats.winner = PlayerColor::NONE;
				turnActionSuccessfullyCompleted = true; continue;
			}
			else if (cvc_cmd == "stats") {
				DisplayCurrentStats(); int cvcY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 7 + 6 + 1; GoToXY(0, cvcY);
				mView.ClearLines(cvcY, 1, CONSOLE_WIDTH_ASSUMED); GoToXY(0, cvcY);
				mView.DisplayMessage("Estadisticas mostradas. Presione Enter para continuar...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
				if (std::cin.peek() == '\n') std::cin.ignore(); std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); continue;
			}
			GoToXY(0, turnMessageY + 1); mView.ClearLines(turnMessageY + 1, 2, CONSOLE_WIDTH_ASSUMED); GoToXY(0, turnMessageY + 1);
		}
		if (!m_currentPlayerObject) { mView.DisplayMessage("Error crítico: Jugador actual no definido.", true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK); mIsGameOver = true; continue; }
		MoveInput userInput = m_currentPlayerObject->GetChosenMoveInput(mGameBoard, mMoveGenerator, mInCaptureSequence, mForcedPieceRow, mForcedPieceCol, mandatoryJumpsForCurrentPlayer);
		int feedbackY = turnMessageY + ((mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER && dynamic_cast<ComputerPlayer*>(m_currentPlayerObject)) ? 1 : 2);
		GoToXY(0, feedbackY); mView.ClearLines(feedbackY, 8, CONSOLE_WIDTH_ASSUMED); GoToXY(0, feedbackY);

		if (userInput.wantsToExit) {
			mView.DisplayMessage("Jugador " + PlayerColorToString(mCurrentPlayerTurnColor) + " elige salir...", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
			mIsGameOver = true; mGameStats.reason = GameOverReason::PLAYER_EXIT;
			mGameStats.winner = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
			turnActionSuccessfullyCompleted = true; mInCaptureSequence = false;
		}
		else if (userInput.wantsToShowStats && dynamic_cast<HumanPlayer*>(m_currentPlayerObject)) {
			DisplayCurrentStats(); int pressY = (GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 7) + 6; GoToXY(0, pressY);
			mView.ClearLines(pressY, 1, CONSOLE_WIDTH_ASSUMED); GoToXY(0, pressY);
			mView.DisplayMessage("Presione Enter para continuar su turno...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
			if (std::cin.peek() == '\n') std::cin.ignore(); std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); continue;
		}
		else if (!userInput.isValidFormat) {
			if (dynamic_cast<HumanPlayer*>(m_currentPlayerObject)) {
				mView.DisplayMessage("FORMATO INCORRECTO. Use 'Origen Destino' (ej: a3 b4).", true, CONSOLE_COLOR_LIGHT_RED, CONSOLE_COLOR_BLACK);
				mView.DisplayMessage("Presione Enter para reintentar...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
				if (std::cin.peek() == '\n') std::cin.ignore(); std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			}
			else {
				mView.DisplayMessage("IA (" + PlayerColorToString(mCurrentPlayerTurnColor) + ") no pudo generar movimiento.", true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
				mGameStats.winner = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
				mGameStats.reason = GameOverReason::NO_MOVES; mIsGameOver = true; turnActionSuccessfullyCompleted = true;
			}
		}
		else {
			int sR = userInput.startRow, sC = userInput.startCol, eR = userInput.endRow, eC = userInput.endCol;
			bool moveWasCapture = false;
			// --- CORRECCIÓN: Llamada a IsValidMove ---
			// Tu IsValidMove no toma los últimos 3 argumentos que yo había puesto.
			// Tampoco devuelve un string de error, así que la lógica de specificErrorMessage se simplifica.
			if (!mMoveGenerator.IsValidMove(mGameBoard, sR, sC, eR, eC, mCurrentPlayerTurnColor, moveWasCapture)) {
				// Como IsValidMove no devuelve el mensaje de error, damos uno genérico o intentamos deducir.
				// Por ahora, un mensaje genérico. Podrías mejorar esto si IsValidMove
				// pudiera setear un código de error o si rehaces la lógica de validación aquí.
				mView.DisplayMessage("MOVIMIENTO INVALIDO.", true, CONSOLE_COLOR_LIGHT_RED, CONSOLE_COLOR_BLACK);
				if (dynamic_cast<HumanPlayer*>(m_currentPlayerObject)) {
					mView.DisplayMessage("Presione Enter para reintentar...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
					if (std::cin.peek() == '\n') std::cin.ignore(); std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				}
				else if (mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER) {
					mView.DisplayMessage("Presione Enter (IA fallo movimiento)...", true, CONSOLE_COLOR_DARK_GRAY, CONSOLE_COLOR_BLACK);
					if (std::cin.peek() == '\n') std::cin.ignore(); std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				}
				continue;
			}

			Move currentMove;
			currentMove.startR_ = sR; currentMove.startC_ = sC; currentMove.endR_ = eR; currentMove.endC_ = eC;
			currentMove.pieceMoved_ = mGameBoard.GetPieceAt(sR, sC);
			currentMove.playerColor_ = mCurrentPlayerTurnColor;
			currentMove.isCapture_ = moveWasCapture; // Usar el valor que IsValidMove determinó

			// --- CORRECCIÓN: Reemplazar MakeMoveObject ---
			PieceType pieceToMove = mGameBoard.GetPieceAt(sR, sC);
			mGameBoard.SetPieceAt(eR, eC, pieceToMove); // Mover la pieza a la nueva casilla
			mGameBoard.SetPieceAt(sR, sC, PieceType::EMPTY); // Vaciar la casilla original

			if (moveWasCapture) {
				int capturedRow = sR + (eR - sR) / 2; // Fila de la pieza capturada
				int capturedCol = sC + (eC - sC) / 2; // Columna de la pieza capturada
				mGameBoard.SetPieceAt(capturedRow, capturedCol, PieceType::EMPTY); // Eliminar pieza capturada
			}
			mGameBoard.PromotePieceIfNecessary(eR, eC); // Promocionar si es necesario
			// --- FIN CORRECCIÓN MakeMoveObject ---

			if (mCurrentGameMode != GameMode::COMPUTER_VS_COMPUTER || !dynamic_cast<ComputerPlayer*>(m_currentPlayerObject)) {
				mView.DisplayMessage("Movimiento realizado: " + currentMove.ToNotation(), true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
			}
			else {
				mView.DisplayMessage("IA (" + PlayerColorToString(mCurrentPlayerTurnColor) + ") realizo: " + currentMove.ToNotation(), true, CONSOLE_COLOR_CYAN, CONSOLE_COLOR_BLACK);
				std::this_thread::sleep_for(std::chrono::milliseconds(750));
			}
			mLastMove = currentMove;

			if (moveWasCapture) {
				if (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) mGameStats.player1CapturedCount++;
				else mGameStats.player2CapturedCount++;
			}

			if (moveWasCapture) {
				std::vector<Move> furtherJumps = mMoveGenerator.GetPossibleJumpsForSpecificPiece(mGameBoard, eR, eC);
				if (!furtherJumps.empty()) {
					mInCaptureSequence = true; mForcedPieceRow = eR; mForcedPieceCol = eC;
					if (dynamic_cast<HumanPlayer*>(m_currentPlayerObject) && std::cin.rdbuf()->in_avail() > 0) { std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); }
				}
				else { mInCaptureSequence = false; turnActionSuccessfullyCompleted = true; }
			}
			else { mInCaptureSequence = false; turnActionSuccessfullyCompleted = true; }

			if (turnActionSuccessfullyCompleted) {
				PlayerColor opponent = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
				if (mGameBoard.GetPieceCount(opponent) == 0) {
					mView.DisplayMessage("FIN DEL JUEGO!", true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
					mGameStats.winner = mCurrentPlayerTurnColor; mGameStats.reason = GameOverReason::NO_PIECES; mIsGameOver = true;
				}
				else if (!mIsGameOver) {
					if (!mMoveGenerator.HasAnyValidMoves(mGameBoard, opponent)) {
						mView.DisplayMessage("FIN DEL JUEGO!", true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
						mView.DisplayMessage("El jugador " + PlayerColorToString(opponent) + " no tiene movimientos.", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
						mGameStats.winner = mCurrentPlayerTurnColor; mGameStats.reason = GameOverReason::NO_MOVES; mIsGameOver = true;
					}
					else { SwitchPlayer(); mGameStats.currentTurnNumber++; }
				}
			}
		}
	}
}
void GameManager::SwitchPlayer() {
	if (!mIsGameOver) {
		mCurrentPlayerTurnColor = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
		m_currentPlayerObject = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? m_player1.get() : m_player2.get();
	}
}
void GameManager::AnnounceResult() {
	int finalMessageStartY = GAME_TITLE_LINES; GoToXY(0, finalMessageStartY);
	mView.ClearLines(finalMessageStartY, BOARD_VISUAL_HEIGHT + 20, CONSOLE_WIDTH_ASSUMED); GoToXY(0, finalMessageStartY);
	mView.DisplayMessage("--- PARTIDA FINALIZADA ---", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	GameResult gameResultData;
	gameResultData.date = getCurrentDateTime("%Y-%m-%d"); gameResultData.time = getCurrentDateTime("%H:%M:%S");
	std::string p1TypeStr = "Desconocido", p2TypeStr = "Desconocido";
	switch (mCurrentGameMode) {
	case GameMode::PLAYER_VS_PLAYER: p1TypeStr = "Humano"; p2TypeStr = "Humano"; break;
	case GameMode::PLAYER_VS_COMPUTER: p1TypeStr = "Humano"; p2TypeStr = "Computadora"; break;
	case GameMode::COMPUTER_VS_COMPUTER: p1TypeStr = "Computadora"; p2TypeStr = "Computadora"; break;
	default: break;
	}
	gameResultData.playerTypes = p1TypeStr + " vs " + p2TypeStr;
	PlayerColor winner = mGameStats.winner;
	if (winner != PlayerColor::NONE) gameResultData.winner = PlayerColorToString(winner);
	else if (mGameStats.reason == GameOverReason::STALEMATE_BY_RULES) gameResultData.winner = "Empate";
	else if (mGameStats.reason == GameOverReason::PLAYER_EXIT && mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER) gameResultData.winner = "N/A (Salida Observador)";
	else gameResultData.winner = "N/A";
	std::string reasonDisplayStr = ""; PlayerColor loser = (winner != PlayerColor::NONE) ? ((winner == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1) : PlayerColor::NONE;
	switch (mGameStats.reason) {
	case GameOverReason::NO_PIECES: reasonDisplayStr = PlayerColorToString(loser) + " se quedo sin piezas."; break;
	case GameOverReason::NO_MOVES: reasonDisplayStr = PlayerColorToString(loser) + " no tiene movimientos (bloqueado)."; break;
	case GameOverReason::PLAYER_EXIT: reasonDisplayStr = (winner != PlayerColor::NONE) ? (PlayerColorToString(loser) + " salio de la partida.") : "El observador salio de la partida."; break;
	case GameOverReason::STALEMATE_BY_RULES: reasonDisplayStr = "Empate por reglas del juego."; break;
	default: reasonDisplayStr = (winner != PlayerColor::NONE) ? ((mGameBoard.GetPieceCount(loser) == 0) ? PlayerColorToString(loser) + " se quedo sin piezas." : PlayerColorToString(loser) + " perdio por condicion no especificada.") : "Juego terminado por condicion desconocida."; break;
	}
	gameResultData.reason = reasonDisplayStr;
	gameResultData.totalTurns = mGameStats.currentTurnNumber;
	if (winner == PlayerColor::PLAYER_1) { gameResultData.winnerCaptures = mGameStats.player1CapturedCount; gameResultData.loserCaptures = mGameStats.player2CapturedCount; }
	else if (winner == PlayerColor::PLAYER_2) { gameResultData.winnerCaptures = mGameStats.player2CapturedCount; gameResultData.loserCaptures = mGameStats.player1CapturedCount; }
	else { gameResultData.winnerCaptures = mGameStats.player1CapturedCount; gameResultData.loserCaptures = mGameStats.player2CapturedCount; }

	if (winner != PlayerColor::NONE) {
		mView.DisplayMessage("GANADOR: " + PlayerColorToString(winner) + "!", true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
		mView.DisplayMessage("Razon: " + gameResultData.reason, true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	}
	else if (gameResultData.winner == "Empate") {
		mView.DisplayMessage("EMPATE. Razon: " + gameResultData.reason, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	}
	else { mView.DisplayMessage("Juego terminado. " + gameResultData.reason, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK); }

	// --- CORRECCIÓN: Usar CONSOLE_COLOR_GREEN ---
	if (mFileHandler.saveGameResult(gameResultData)) {
		mView.DisplayMessage("Resultado de la partida guardado exitosamente.", true, CONSOLE_COLOR_GREEN, CONSOLE_COLOR_BLACK);
	}
	else { mView.DisplayMessage("Error al guardar el resultado de la partida.", true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK); }

	int statsY = finalMessageStartY + ((winner != PlayerColor::NONE) ? 4 : 3) + 1; // +1 por el mensaje de guardado
	GoToXY(0, statsY); DisplayCurrentStats();
	int pressY = statsY + 6; GoToXY(0, pressY); mView.ClearLines(pressY, 1, CONSOLE_WIDTH_ASSUMED); GoToXY(0, pressY);
	mView.DisplayMessage("Presione Enter para volver al menu principal...", true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_BLACK);
	if (std::cin.rdbuf()->in_avail() > 0) std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
	std::cin.get();
}