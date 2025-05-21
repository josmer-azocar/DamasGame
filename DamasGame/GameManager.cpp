// GameManager.cpp
#include "LocalizationManager.h"
#include "GameManager.h"
#include "ConsoleView.h"
#include "InputHandler.h" 
#include "Board.h"
#include "CommonTypes.h"
#include "MoveGenerator.h"
#include "Player.h"         
#include "HumanPlayer.h"    
#include "ComputerPlayer.h" 
#include "FileHandler.h" 

#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <cmath>     
#include <memory>    
#include <thread> 
#include <chrono>  
#include <algorithm> // Para std::transform
#include <iomanip>   // Para std::put_time
#include <sstream>   // Para std::ostringstream

const int GAME_TITLE_LINES = 4;
const int BOARD_VISUAL_HEIGHT = 1 + 1 + (Board::BOARD_SIZE * 2 - 1) + 1;
const int CONSOLE_WIDTH_ASSUMED = 80;

GameManager::GameManager(Board& board, InputHandler& inputHandler)
	: mGameBoard(board),
	m_i18n(), 
	mView(m_i18n), 
	mInputHandler(inputHandler),
	mMoveGenerator(),
	mFileHandler(m_i18n),
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
	std::tm now_tm{}; // Inicializar a ceros

#ifdef _WIN32
	localtime_s(&now_tm, &now_time_t);
#else
	// localtime es sensible al hilo en algunos sistemas, localtime_r es más seguro si está disponible
	// pero para este caso, un localtime simple debería estar bien.
	std::tm* temp_tm = std::localtime(&now_time_t);
	if (temp_tm != nullptr) {
		now_tm = *temp_tm;
	}
	else {
		// Manejar error si localtime devuelve nullptr
		// Podrías lanzar una excepción o devolver una cadena de error.
		return "Error:localtime";
	}
#endif

	std::ostringstream oss;
	oss << std::put_time(&now_tm, format_string.c_str());
	return oss.str();
}

void GameManager::InitializeApplication() { 
	m_i18n.SelectLanguageUI(mView, mInputHandler); //Menu de seleccion de idiomas
	
	ShowMainMenu(); }

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
	// Asegurar que GoToXY está disponible o ConsoleView lo maneja internamente
	GoToXY(0, 1); // Posicionar un poco más abajo del inicio para que el título no quede pegado arriba
	mFileHandler.displayGameHistory(); // Asume que displayGameHistory maneja su propio GoToXY si es necesario para el contenido

	// Posicionar el mensaje "Presione Enter" de forma consistente
	int pressEnterY = 20; // O calcular basado en la altura del historial
	GoToXY(0, pressEnterY);
	mView.ClearLines(pressEnterY, 1, CONSOLE_WIDTH_ASSUMED);
	GoToXY(0, pressEnterY);
	mView.DisplayMessage(m_i18n.GetString("press_enter_to_menu"), true, CONSOLE_COLOR_LIGHT_CYAN); // Añadido color
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
		mView.DisplayMessage(m_i18n.GetString("error_invalid_mode"), true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
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
		mView.DisplayMessage(m_i18n.GetString("error_game_not_init"), true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
		std::cin.get(); return;
	}
	GoToXY(0, 0); mView.ClearLines(0, GAME_TITLE_LINES, CONSOLE_WIDTH_ASSUMED); GoToXY(0, 0);
	std::string modeTitle;
	if (mCurrentGameMode == GameMode::PLAYER_VS_PLAYER) modeTitle = m_i18n.GetString("game_title_pvp");
	else if (mCurrentGameMode == GameMode::PLAYER_VS_COMPUTER) modeTitle = m_i18n.GetString("game_title_pvc");
	else if (mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER) modeTitle = m_i18n.GetString("game_title_cvc");

	mView.DisplayMessage(modeTitle, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage(m_i18n.GetString("game_rules_line1"), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage(m_i18n.GetString("game_rules_line2"), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage("--------------------------------------------------", true, CONSOLE_COLOR_WHITE, CONSOLE_COLOR_BLACK);
	mIsGameOver = false;
	while (!mIsGameOver) { ProcessPlayerTurn(); } AnnounceResult();
}
void GameManager::DisplayCurrentStats() {
	int statsY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 7;
	GoToXY(0, statsY); mView.ClearLines(statsY, 6, CONSOLE_WIDTH_ASSUMED); GoToXY(0, statsY);
	mView.DisplayMessage(m_i18n.GetString("game_current_stats_title"), true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage(m_i18n.GetString("game_current_turn") + std::to_string(mGameStats.currentTurnNumber), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	int p1p = mGameBoard.GetPieceCount(PlayerColor::PLAYER_1), p1k = mGameBoard.GetKingCount(PlayerColor::PLAYER_1), p1m = p1p - p1k;
	int p2p = mGameBoard.GetPieceCount(PlayerColor::PLAYER_2), p2k = mGameBoard.GetKingCount(PlayerColor::PLAYER_2), p2m = p2p - p2k;
	mView.DisplayMessage(PlayerColorToString(PlayerColor::PLAYER_1) + ": " + std::to_string(p1p) + " (" + std::to_string(p1m) + "p, " + std::to_string(p1k) + "D). Capt: " + std::to_string(mGameStats.player1CapturedCount), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage(PlayerColorToString(PlayerColor::PLAYER_2) + ": " + std::to_string(p2p) + " (" + std::to_string(p2m) + "p, " + std::to_string(p2k) + "D). Capt: " + std::to_string(mGameStats.player2CapturedCount), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage("-----------------------------", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
}
void GameManager::DisplayLastMove() {
	int lastMoveY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT;
	GoToXY(0, lastMoveY); mView.ClearLines(lastMoveY, 1, CONSOLE_WIDTH_ASSUMED); GoToXY(0, lastMoveY);
	if (!mLastMove.IsNull()) { mView.DisplayMessage(m_i18n.GetString("last_move") + mLastMove.ToNotation(), true, CONSOLE_COLOR_WHITE, CONSOLE_COLOR_BLACK); }
}

void GameManager::ProcessPlayerTurn() {
	bool turnActionSuccessfullyCompleted = false;
	while (!turnActionSuccessfullyCompleted && !mIsGameOver) {
		GoToXY(0, GAME_TITLE_LINES); mView.ClearLines(GAME_TITLE_LINES, BOARD_VISUAL_HEIGHT + 15, CONSOLE_WIDTH_ASSUMED); GoToXY(0, GAME_TITLE_LINES);
		mView.DisplayBoard(mGameBoard, CONSOLE_COLOR_BLACK); DisplayLastMove();
		int turnMessageY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 1; GoToXY(0, turnMessageY);
		std::vector<Move> mandatoryJumpsForCurrentPlayer;
		MandatoryActionType currentMandatoryAction = MandatoryActionType::NONE;

		if (!mInCaptureSequence) {
			currentMandatoryAction = mMoveGenerator.GetMandatoryActionType(mGameBoard, mCurrentPlayerTurnColor, mandatoryJumpsForCurrentPlayer);
		}
		else {
			mandatoryJumpsForCurrentPlayer = mMoveGenerator.GetPossibleJumpsForSpecificPiece(mGameBoard, mForcedPieceRow, mForcedPieceCol);
			PieceType forcedPiece = mGameBoard.GetPieceAt(mForcedPieceRow, mForcedPieceCol);
			currentMandatoryAction = (forcedPiece == PieceType::P1_KING || forcedPiece == PieceType::P2_KING) ? MandatoryActionType::KING_CAPTURE : MandatoryActionType::PAWN_CAPTURE;
		}

		if (!mInCaptureSequence && !mMoveGenerator.HasAnyValidMoves(mGameBoard, mCurrentPlayerTurnColor)) {
			mView.DisplayMessage(m_i18n.GetString("player_mention") + PlayerColorToString(mCurrentPlayerTurnColor) + m_i18n.GetString("reason_no_moves"), true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
			mGameStats.winner = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
			mGameStats.reason = GameOverReason::NO_MOVES; mIsGameOver = true; turnActionSuccessfullyCompleted = true; continue;
		}
		std::string turnMsg = m_i18n.GetString("turn_of_player") + PlayerColorToString(mCurrentPlayerTurnColor) + ". ";
		if (mInCaptureSequence) { turnMsg += m_i18n.GetString("turn_continue_capture") + ToAlgebraic(mForcedPieceRow, mForcedPieceCol) + "."; }
		mView.DisplayMessage(turnMsg, true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_BLACK);

		// --- INICIO MODIFICACIÓN PAUSA CvC ---
		if (mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER && dynamic_cast<ComputerPlayer*>(m_currentPlayerObject)) {
			GoToXY(0, turnMessageY + 1);
			mView.ClearLines(turnMessageY + 1, 2, CONSOLE_WIDTH_ASSUMED); // Limpiar para mensaje y prompt
			GoToXY(0, turnMessageY + 1);
			mView.DisplayMessage(m_i18n.GetString("artificial_inteligence") + "(" + PlayerColorToString(mCurrentPlayerTurnColor) + ")" + m_i18n.GetString("cvc_ia_turn_prompt"), false, CONSOLE_COLOR_DARK_GRAY, CONSOLE_COLOR_BLACK);
			std::cout << std::endl << "> "; // Mover el prompt ">" a la siguiente línea

			std::string cvc_command_line;
			if (std::cin.peek() == '\n') { // Consumir newline si está en el buffer
				
			}
			std::getline(std::cin, cvc_command_line);
			std::string command_lower = cvc_command_line;
			std::transform(command_lower.begin(), command_lower.end(), command_lower.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

			if (command_lower == "salir" || command_lower == "exit") {
				mView.DisplayMessage(m_i18n.GetString("cvc_observer_exit"), true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
				mIsGameOver = true;
				mGameStats.reason = GameOverReason::PLAYER_EXIT;
				mGameStats.winner = PlayerColor::NONE;
				turnActionSuccessfullyCompleted = true;
				continue; // Salir del bucle while de ProcessPlayerTurn
			}
			else if (command_lower == "stats") {
				DisplayCurrentStats();
				int pressEnterCvCY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 7 + 6 + 1; // Debajo de stats
				GoToXY(0, pressEnterCvCY);
				mView.ClearLines(pressEnterCvCY, 1, CONSOLE_WIDTH_ASSUMED);
				GoToXY(0, pressEnterCvCY);
				mView.DisplayMessage(m_i18n.GetString("cvc_stats_continue_prompt"), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
				if (std::cin.peek() == '\n') std::cin.ignore();
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				continue; // Volver al inicio de ProcessPlayerTurn para redibujar y volver a pausar.
			}
			// Si no fue "salir" ni "stats", se asume Enter o cualquier otra cosa, el juego continúa con el turno de la IA.
			GoToXY(0, turnMessageY + 1);
			mView.ClearLines(turnMessageY + 1, 2, CONSOLE_WIDTH_ASSUMED); // Limpiar mensaje
			GoToXY(0, turnMessageY + 1); // Reposicionar por si acaso
		}

		if (!m_currentPlayerObject) { mView.DisplayMessage(m_i18n.GetString("critical_error_player_undefinedt"), true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK); mIsGameOver = true; continue; }
		MoveInput userInput = m_currentPlayerObject->GetChosenMoveInput(mGameBoard, mMoveGenerator, mInCaptureSequence, mForcedPieceRow, mForcedPieceCol, mandatoryJumpsForCurrentPlayer);

		int feedbackY = turnMessageY + 2;
		// Ajustar feedbackY si estábamos en pausa de CvC
		if (mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER && dynamic_cast<ComputerPlayer*>(m_currentPlayerObject)) {
			feedbackY = turnMessageY + 1; 
		}
		GoToXY(0, feedbackY); mView.ClearLines(feedbackY, 8, CONSOLE_WIDTH_ASSUMED); GoToXY(0, feedbackY);

		if (userInput.wantsToExit) {
			mView.DisplayMessage(m_i18n.GetString("player_mention") + PlayerColorToString(mCurrentPlayerTurnColor) + m_i18n.GetString("player_wants_to_exit"), true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
			mIsGameOver = true; mGameStats.reason = GameOverReason::PLAYER_EXIT;
			mGameStats.winner = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
			turnActionSuccessfullyCompleted = true; mInCaptureSequence = false;
		}
		else if (userInput.wantsToShowStats && dynamic_cast<HumanPlayer*>(m_currentPlayerObject)) {
			DisplayCurrentStats(); int pressY = (GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 7) + 6; GoToXY(0, pressY);
			mView.ClearLines(pressY, 1, CONSOLE_WIDTH_ASSUMED); GoToXY(0, pressY);
			mView.DisplayMessage(m_i18n.GetString("stats_show_continue_prompt"), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
			if (std::cin.peek() == '\n') std::cin.ignore(); std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); continue;
		}
		else if (!userInput.isValidFormat) {
			if (dynamic_cast<HumanPlayer*>(m_currentPlayerObject)) {
				mView.DisplayMessage(m_i18n.GetString("error_invalid_format"), true, CONSOLE_COLOR_LIGHT_RED, CONSOLE_COLOR_BLACK);
				mView.DisplayMessage(m_i18n.GetString("retry_prompt"), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
				if (std::cin.peek() == '\n') std::cin.ignore(); std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			}
			else {
				mView.DisplayMessage(m_i18n.GetString("artificial_inteligence") + "(" + PlayerColorToString(mCurrentPlayerTurnColor) + ")"+ m_i18n.GetString("error_ai_no_move"), true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
				mGameStats.winner = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
				mGameStats.reason = GameOverReason::NO_MOVES; mIsGameOver = true; turnActionSuccessfullyCompleted = true;
			}
		}
		else {
			int sR = userInput.startRow, sC = userInput.startCol, eR = userInput.endRow, eC = userInput.endCol;
			bool moveWasCapture = false;
			std::string specificErrorMessage = ""; // Para mensajes de error más detallados
			bool isValidAttemptGeneral = true;     // Para controlar el flujo de validación

			if (mInCaptureSequence) {
				
				if (sR != mForcedPieceRow || sC != mForcedPieceCol) {
					specificErrorMessage = m_i18n.GetString("error_must_move_forced_piece1") + ToAlgebraic(mForcedPieceRow, mForcedPieceCol) + m_i18n.GetString("error_must_move_forced_piece2");
					isValidAttemptGeneral = false;
				}
				else {
					bool isValidContinuationJump = false;
					for (const auto& jump : mandatoryJumpsForCurrentPlayer) {
						if (jump.endR_ == eR && jump.endC_ == eC) {
							isValidContinuationJump = true;
							moveWasCapture = true; // Un salto de continuación siempre es captura
							break;
						}
					}
					if (!isValidContinuationJump) {
						specificErrorMessage = m_i18n.GetString("error_invalid_jump_sequence1") + ToAlgebraic(mForcedPieceRow, mForcedPieceCol) +
							m_i18n.GetString("error_invalid_jump_sequence2") + ToAlgebraic(eR, eC) + m_i18n.GetString("error_invalid_jump_sequence1") + "\n";
						specificErrorMessage += m_i18n.GetString("error_invalid_jump_sequence4");
						if (mandatoryJumpsForCurrentPlayer.empty()) {
							specificErrorMessage += m_i18n.GetString("error_no_jumps_left_sequence");
							mInCaptureSequence = false;
						}
						else {
							for (size_t i = 0; i < mandatoryJumpsForCurrentPlayer.size(); ++i) {
								specificErrorMessage += ToAlgebraic(mandatoryJumpsForCurrentPlayer[i].endR_, mandatoryJumpsForCurrentPlayer[i].endC_) + (i == mandatoryJumpsForCurrentPlayer.size() - 1 ? "" : ", ");
							}
						}
						isValidAttemptGeneral = false;
					}
				}
			}
			else {
				// (Lógica de validación de movimiento normal, usando currentMandatoryAction y mandatoryJumpsForCurrentPlayer
				
				if (!mMoveGenerator.IsValidMove(mGameBoard, sR, sC, eR, eC, mCurrentPlayerTurnColor, moveWasCapture)) {
					isValidAttemptGeneral = false;
					PieceType attemptedMovePieceType = mGameBoard.GetPieceAt(sR, sC);

					if (currentMandatoryAction == MandatoryActionType::KING_CAPTURE) {
						specificErrorMessage = m_i18n.GetString("error_must_capture_king1") + "\n";
						if (!mandatoryJumpsForCurrentPlayer.empty()) {
							specificErrorMessage += m_i18n.GetString("error_must_capture_king2") + "\n";
							for (const auto& jump_move : mandatoryJumpsForCurrentPlayer) {
								specificErrorMessage += "  -> " + jump_move.ToNotation() + "\n";
							}
						}
						else {
							specificErrorMessage += m_i18n.GetString("error_internal_no_mandatory_moves") + "\n";
						}
					}
					else if (currentMandatoryAction == MandatoryActionType::PAWN_CAPTURE) {
						if (attemptedMovePieceType == PieceType::P1_KING || attemptedMovePieceType == PieceType::P2_KING) {
							specificErrorMessage = m_i18n.GetString("error_king_move_when_pawn_capture1") + "\n";
							specificErrorMessage += m_i18n.GetString("error_king_move_when_pawn_capture2") + "\n";
						}
						else {
							specificErrorMessage += m_i18n.GetString("error_must_capture_pawn1") + "\n";
						}
						if (!mandatoryJumpsForCurrentPlayer.empty()) {
							specificErrorMessage += m_i18n.GetString("error_must_capture_pawn2") + "\n";

							for (const auto& jump_move : mandatoryJumpsForCurrentPlayer) {
								specificErrorMessage += "  -> " + jump_move.ToNotation() + "\n";
							}
						}
						else {
							specificErrorMessage += m_i18n.GetString("error_internal_no_mandatory_moves") + "\n";
						}
					}
					else if (currentMandatoryAction == MandatoryActionType::NONE) {
						bool proposedMoveWasIntendedAsCapture = (std::abs(sR - eR) == 2 && std::abs(sC - eC) == 2);
						if (proposedMoveWasIntendedAsCapture) {
							specificErrorMessage = m_i18n.GetString("error_no_mandatory_capture_attempt");
						}
						else if (mGameBoard.GetPieceAt(eR, eC) != PieceType::EMPTY) {
							specificErrorMessage = m_i18n.GetString("error_destination_occupied1") + "(" + ToAlgebraic(eR, eC) + ") " + m_i18n.GetString("error_destination_occupied2");
						}
						else {
							specificErrorMessage = m_i18n.GetString("error_move_not_legal1") +
								ToAlgebraic(sR, sC) + m_i18n.GetString("error_move_not_legal2") +
								ToAlgebraic(eR, eC) + m_i18n.GetString("error_move_not_legal3");
						}
					}
					else {
						specificErrorMessage = m_i18n.GetString("error_unknown_move_rule");
					}
				}
			}


			if (!isValidAttemptGeneral) { // Si el movimiento es inválido por CUALQUIER razón
				mView.DisplayMessage(specificErrorMessage, true, CONSOLE_COLOR_LIGHT_RED, CONSOLE_COLOR_BLACK);
				if (dynamic_cast<HumanPlayer*>(m_currentPlayerObject)) {
					mView.DisplayMessage(m_i18n.GetString("retry_prompt"), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
					if (std::cin.peek() == '\n') std::cin.ignore(); std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				}
				else if (mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER) { // Si es IA en CvC y falló
					mView.DisplayMessage(m_i18n.GetString("ai_failed"), true, CONSOLE_COLOR_DARK_GRAY, CONSOLE_COLOR_BLACK);
					if (std::cin.peek() == '\n') std::cin.ignore(); std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				}
				continue; // Vuelve al inicio del bucle ProcessPlayerTurn
			}

			// Si llegamos aquí, el movimiento es válido.
			Move currentMove;
			currentMove.startR_ = sR; currentMove.startC_ = sC; currentMove.endR_ = eR; currentMove.endC_ = eC;
			currentMove.pieceMoved_ = mGameBoard.GetPieceAt(sR, sC);
			currentMove.playerColor_ = mCurrentPlayerTurnColor;
			currentMove.isCapture_ = moveWasCapture;

			PieceType pieceToMove = mGameBoard.GetPieceAt(sR, sC);
			mGameBoard.SetPieceAt(eR, eC, pieceToMove);
			mGameBoard.SetPieceAt(sR, sC, PieceType::EMPTY);

			if (moveWasCapture) {
				int capturedRow = sR + (eR - sR) / 2;
				int capturedCol = sC + (eC - sC) / 2;
				mGameBoard.SetPieceAt(capturedRow, capturedCol, PieceType::EMPTY);
				if (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) mGameStats.player1CapturedCount++;
				else mGameStats.player2CapturedCount++;
			}
			mGameBoard.PromotePieceIfNecessary(eR, eC);

			// Mensaje de movimiento realizado
			if (mCurrentGameMode != GameMode::COMPUTER_VS_COMPUTER || !dynamic_cast<ComputerPlayer*>(m_currentPlayerObject)) {
				mView.DisplayMessage(m_i18n.GetString("move_successful") + currentMove.ToNotation(), true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
			}
			else {
				mView.DisplayMessage(m_i18n.GetString("artificial_inteligence") + " (" + PlayerColorToString(mCurrentPlayerTurnColor) + ") "+ m_i18n.GetString("done") + currentMove.ToNotation(), true, CONSOLE_COLOR_CYAN, CONSOLE_COLOR_BLACK);
				// No hay pausa de thread aquí, la pausa de CvC es ANTES de obtener el movimiento.
			}
			mLastMove = currentMove;


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
					mView.DisplayMessage(m_i18n.GetString("game_over_no_pieces_opponent"), true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
					mGameStats.winner = mCurrentPlayerTurnColor; mGameStats.reason = GameOverReason::NO_PIECES; mIsGameOver = true;
				}
				else if (!mIsGameOver) {
					// Verificar si el oponente tiene movimientos ANTES de cambiar de jugador
					// Pero solo si el juego no ha terminado ya por otra razón.
					if (!mMoveGenerator.HasAnyValidMoves(mGameBoard, opponent)) {
						mView.DisplayMessage(m_i18n.GetString("game_over_no_pieces_opponent"), true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
						mView.DisplayMessage(m_i18n.GetString("player_mention") + PlayerColorToString(opponent) + m_i18n.GetString("reason_no_moves"), true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
						mGameStats.winner = mCurrentPlayerTurnColor; // Gana el jugador actual
						mGameStats.reason = GameOverReason::NO_MOVES;
						mIsGameOver = true;
					}
					else {
						// Solo incrementar turno y cambiar jugador si el juego no ha terminado
						mGameStats.currentTurnNumber++;
						SwitchPlayer();
					}
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
	mView.DisplayMessage(m_i18n.GetString("announce_game_over_title"), true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	GameResult gameResultData; // Asumo que GameResult está definido en CommonTypes.h o FileHandler.h
	gameResultData.date = getCurrentDateTime("%Y-%m-%d"); gameResultData.time = getCurrentDateTime("%H:%M:%S");
	std::string p1TypeStr = m_i18n.GetString("unknown"), p2TypeStr = m_i18n.GetString("unknown");
	switch (mCurrentGameMode) {
	case GameMode::PLAYER_VS_PLAYER: p1TypeStr = m_i18n.GetString("human"); p2TypeStr = m_i18n.GetString("human"); break;
	case GameMode::PLAYER_VS_COMPUTER:
		// Necesitamos saber quién fue humano y quién IA
		if (dynamic_cast<HumanPlayer*>(m_player1.get())) p1TypeStr = m_i18n.GetString("human"); else p1TypeStr = m_i18n.GetString("computer");
		if (dynamic_cast<HumanPlayer*>(m_player2.get())) p2TypeStr = m_i18n.GetString("human"); else p2TypeStr = m_i18n.GetString("computer");
		break;
	case GameMode::COMPUTER_VS_COMPUTER: p1TypeStr = m_i18n.GetString("computer"); p2TypeStr = m_i18n.GetString("computer"); break;
	default: break;
	}
	gameResultData.playerTypes = p1TypeStr + " vs " + p2TypeStr;

	PlayerColor winner = mGameStats.winner;
	if (winner != PlayerColor::NONE) gameResultData.winner = PlayerColorToString(winner);
	else if (mGameStats.reason == GameOverReason::STALEMATE_BY_RULES) gameResultData.winner = m_i18n.GetString("announce_draw");
	else if (mGameStats.reason == GameOverReason::PLAYER_EXIT && mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER) gameResultData.winner = "N/A";
	else gameResultData.winner = "N/A"; // O "Indeterminado"

	std::string reasonDisplayStr = "";
	PlayerColor loser = PlayerColor::NONE;
	if (winner != PlayerColor::NONE) { // Solo hay perdedor si hay un ganador
		loser = (winner == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
	}

	switch (mGameStats.reason) {
	case GameOverReason::NO_PIECES:
		if (loser != PlayerColor::NONE) reasonDisplayStr = PlayerColorToString(loser) + m_i18n.GetString("reason_no_pieces");
		else reasonDisplayStr = m_i18n.GetString("reason_player_no_pieces");
		break;
	case GameOverReason::NO_MOVES:
		if (loser != PlayerColor::NONE) reasonDisplayStr = PlayerColorToString(loser) + m_i18n.GetString("reason_no_moves");
		else reasonDisplayStr = m_i18n.GetString("reason_player_no_moves");
		break;
	case GameOverReason::PLAYER_EXIT:
		if (winner != PlayerColor::NONE && loser != PlayerColor::NONE)
			reasonDisplayStr = PlayerColorToString(loser) + m_i18n.GetString("reason_exit");
		else if (mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER)
			reasonDisplayStr = m_i18n.GetString("watcher_out");
		else
			reasonDisplayStr = m_i18n.GetString("reason_player_exit");
		break;
	case GameOverReason::STALEMATE_BY_RULES: reasonDisplayStr = m_i18n.GetString("reason_stalemate_rules"); break;
	default:
		if (winner != PlayerColor::NONE && loser != PlayerColor::NONE) {
			reasonDisplayStr = (mGameBoard.GetPieceCount(loser) == 0) ?
				PlayerColorToString(loser) + m_i18n.GetString("reason_no_pieces") :
				PlayerColorToString(loser) + m_i18n.GetString("reason_ended_unspecified");
		}
		else {
			reasonDisplayStr = m_i18n.GetString("reason_ended_unspecified");
		}
		break;
	}
	gameResultData.reason = reasonDisplayStr;
	gameResultData.totalTurns = mGameStats.currentTurnNumber;

	if (winner == PlayerColor::PLAYER_1) { gameResultData.winnerCaptures = mGameStats.player1CapturedCount; gameResultData.loserCaptures = mGameStats.player2CapturedCount; }
	else if (winner == PlayerColor::PLAYER_2) { gameResultData.winnerCaptures = mGameStats.player2CapturedCount; gameResultData.loserCaptures = mGameStats.player1CapturedCount; }
	else { // Empate o salida sin ganador claro de piezas
		gameResultData.winnerCaptures = mGameStats.player1CapturedCount; // O mostrar ambos sin etiqueta "ganador"
		gameResultData.loserCaptures = mGameStats.player2CapturedCount;
	}

	if (winner != PlayerColor::NONE) {
		mView.DisplayMessage(m_i18n.GetString("announce_winner") + PlayerColorToString(winner) + "!", true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
		mView.DisplayMessage(m_i18n.GetString("announce_reason") + gameResultData.reason, true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	}
	else if (gameResultData.winner == m_i18n.GetString("announce_draw")) {
		mView.DisplayMessage(m_i18n.GetString("announce_draw")+ m_i18n.GetString("announce_reason") + gameResultData.reason, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	}
	else { mView.DisplayMessage(m_i18n.GetString("announce_game_ended_no_winner") + gameResultData.reason, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK); }

	if (mFileHandler.saveGameResult(gameResultData)) {
		mView.DisplayMessage(m_i18n.GetString("game_result_saved"), true, CONSOLE_COLOR_GREEN, CONSOLE_COLOR_BLACK);
	}
	else { mView.DisplayMessage(m_i18n.GetString("game_result_saving_error"), true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK); }
	
	int statsY = finalMessageStartY + ((winner != PlayerColor::NONE) ? 4 : 3) + 1;
	GoToXY(0, statsY); DisplayCurrentStats();
	int pressY = statsY + 6; GoToXY(0, pressY); mView.ClearLines(pressY, 1, CONSOLE_WIDTH_ASSUMED); GoToXY(0, pressY);
	mView.DisplayMessage(m_i18n.GetString("press_enter_to_menu"), true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_BLACK);
	if (std::cin.rdbuf()->in_avail() > 0) std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
	std::cin.get();
}