// GameManager.cpp

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
#include <thread>   // Para std::this_thread::sleep_for
#include <chrono>   // Para std::chrono::milliseconds
#include <algorithm> // Para std::transform en la pausa de CvC

// Constantes para el layout de la pantalla de juego
const int GAME_TITLE_LINES = 4;
const int BOARD_VISUAL_HEIGHT = 1 + 1 + (Board::BOARD_SIZE * 2 - 1) + 1;
const int CONSOLE_WIDTH_ASSUMED = 80;


GameManager::GameManager(Board& board, ConsoleView& view, InputHandler& inputHandler)
	: mGameBoard(board),
	mView(view),
	mInputHandler(inputHandler),
	mMoveGenerator(),
	mCurrentPlayerTurnColor(PlayerColor::PLAYER_1), // PLAYER_1 (Blancas) 
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

void GameManager::InitializeApplication() {
	ShowMainMenu();
}

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
			case 1:
				mCurrentGameMode = GameMode::PLAYER_VS_PLAYER;
				gameModeSelected = true;
				break;
			case 2:
				mCurrentGameMode = GameMode::PLAYER_VS_COMPUTER;
				gameModeSelected = true;
				break;
			case 3:
				mCurrentGameMode = GameMode::COMPUTER_VS_COMPUTER;
				gameModeSelected = true;
				break;
			case 4:
				ShowGlobalStats();
				break;
			case 5:
				exitMenu = true;
				break;
			}

			if (gameModeSelected) {
				mView.SetGameColorsAndClear();
				StartNewGame();
				RunGameLoop();
			}
		}
		else if (choice < 0) {
			selectedOption = -choice;
		}
	}
}

void GameManager::ShowGlobalStats() {
	mView.SetMenuColorsAndClear();
	GoToXY(0, 3);
	mView.DisplayMessage("--- ESTADISTICAS GLOBALES ---", true, CONSOLE_COLOR_YELLOW);
	mView.DisplayMessage("Esta funcionalidad aun no esta implementada.", true);
	mView.DisplayMessage("Aqui se mostrarian estadisticas guardadas de partidas anteriores.", true);
	mView.DisplayMessage("\nPresione Enter para volver al menu...", true);
	if (std::cin.peek() == '\n') std::cin.ignore();
	std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}

void GameManager::StartNewGame() {
	mGameBoard.InitializeBoard();
	mCurrentPlayerTurnColor = PlayerColor::PLAYER_1;
	mIsGameOver = false;
	mGameStats = GameStats{};
	mLastMove = Move{};
	mInCaptureSequence = false;
	mForcedPieceRow = -1;
	mForcedPieceCol = -1;

	switch (mCurrentGameMode) {
	case GameMode::PLAYER_VS_PLAYER:
		m_player1 = std::make_unique<HumanPlayer>(PlayerColor::PLAYER_1, mInputHandler, mView);
		m_player2 = std::make_unique<HumanPlayer>(PlayerColor::PLAYER_2, mInputHandler, mView);
		break;
	case GameMode::PLAYER_VS_COMPUTER:
		m_player1 = std::make_unique<HumanPlayer>(PlayerColor::PLAYER_1, mInputHandler, mView);
		m_player2 = std::make_unique<ComputerPlayer>(PlayerColor::PLAYER_2, mMoveGenerator);
		break;
	case GameMode::COMPUTER_VS_COMPUTER:
		m_player1 = std::make_unique<ComputerPlayer>(PlayerColor::PLAYER_1, mMoveGenerator);
		m_player2 = std::make_unique<ComputerPlayer>(PlayerColor::PLAYER_2, mMoveGenerator);
		break;
	case GameMode::NONE:
	default:
		mView.DisplayMessage("Error: Modo de juego invalido. Iniciando Jugador vs Jugador.", true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
		if (std::cin.peek() == '\n') std::cin.ignore();
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		mCurrentGameMode = GameMode::PLAYER_VS_PLAYER;
		m_player1 = std::make_unique<HumanPlayer>(PlayerColor::PLAYER_1, mInputHandler, mView);
		m_player2 = std::make_unique<HumanPlayer>(PlayerColor::PLAYER_2, mInputHandler, mView);
		break;
	}
	m_currentPlayerObject = m_player1.get();
}

void GameManager::RunGameLoop() {
	if (mCurrentGameMode == GameMode::NONE || !m_player1 || !m_player2) {
		mView.DisplayMessage("Error: Juego no inicializado correctamente.", true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
		std::cin.get();
		return;
	}
	GoToXY(0, 0);
	mView.ClearLines(0, GAME_TITLE_LINES, CONSOLE_WIDTH_ASSUMED);
	GoToXY(0, 0);

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

	while (!mIsGameOver) {
		ProcessPlayerTurn();
	}
	AnnounceResult();
}

void GameManager::DisplayCurrentStats() {
	int statsY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 7;
	GoToXY(0, statsY);
	mView.ClearLines(statsY, 6, CONSOLE_WIDTH_ASSUMED);
	GoToXY(0, statsY);

	mView.DisplayMessage("--- Estadisticas Actuales ---", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage("Turno Actual Nro: " + std::to_string(mGameStats.currentTurnNumber), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	int piecesP1 = mGameBoard.GetPieceCount(PlayerColor::PLAYER_1); int kingsP1 = mGameBoard.GetKingCount(PlayerColor::PLAYER_1); int menP1 = piecesP1 - kingsP1;
	int piecesP2 = mGameBoard.GetPieceCount(PlayerColor::PLAYER_2); int kingsP2 = mGameBoard.GetKingCount(PlayerColor::PLAYER_2); int menP2 = piecesP2 - kingsP2;
	mView.DisplayMessage(PlayerColorToString(PlayerColor::PLAYER_1) + ": " + std::to_string(piecesP1) + " (" + std::to_string(menP1) + "p, " + std::to_string(kingsP1) + "D). Capt: " + std::to_string(mGameStats.player1CapturedCount), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage(PlayerColorToString(PlayerColor::PLAYER_2) + ": " + std::to_string(piecesP2) + " (" + std::to_string(menP2) + "p, " + std::to_string(kingsP2) + "D). Capt: " + std::to_string(mGameStats.player2CapturedCount), true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	mView.DisplayMessage("-----------------------------", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
}

void GameManager::DisplayLastMove() {
	int lastMoveY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT;
	GoToXY(0, lastMoveY);
	mView.ClearLines(lastMoveY, 1, CONSOLE_WIDTH_ASSUMED);
	GoToXY(0, lastMoveY);
	if (!mLastMove.IsNull()) {
		mView.DisplayMessage("Ultimo movimiento: " + mLastMove.ToNotation(), true, CONSOLE_COLOR_WHITE, CONSOLE_COLOR_BLACK);
	}
}

void GameManager::ProcessPlayerTurn() {
	bool turnActionSuccessfullyCompleted = false;

	while (!turnActionSuccessfullyCompleted && !mIsGameOver) {

		GoToXY(0, GAME_TITLE_LINES);
		mView.ClearLines(GAME_TITLE_LINES, BOARD_VISUAL_HEIGHT + 15, CONSOLE_WIDTH_ASSUMED);
		GoToXY(0, GAME_TITLE_LINES);

		mView.DisplayBoard(mGameBoard, CONSOLE_COLOR_BLACK);
		DisplayLastMove();

		int turnMessageY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 1;
		GoToXY(0, turnMessageY);

		std::vector<Move> mandatoryJumpsForCurrentPlayer;
		MandatoryActionType currentMandatoryAction = MandatoryActionType::NONE;

		if (!mInCaptureSequence) {
			currentMandatoryAction = mMoveGenerator.GetMandatoryActionType(mGameBoard, mCurrentPlayerTurnColor, mandatoryJumpsForCurrentPlayer);
		}
		else {
			mandatoryJumpsForCurrentPlayer = mMoveGenerator.GetPossibleJumpsForSpecificPiece(mGameBoard, mForcedPieceRow, mForcedPieceCol);
			PieceType forcedPiece = mGameBoard.GetPieceAt(mForcedPieceRow, mForcedPieceCol);
			if (forcedPiece == PieceType::P1_KING || forcedPiece == PieceType::P2_KING) {
				currentMandatoryAction = MandatoryActionType::KING_CAPTURE;
			}
			else {
				currentMandatoryAction = MandatoryActionType::PAWN_CAPTURE;
			}
		}

		if (!mInCaptureSequence && !mMoveGenerator.HasAnyValidMoves(mGameBoard, mCurrentPlayerTurnColor)) {
			mView.DisplayMessage("El jugador " + PlayerColorToString(mCurrentPlayerTurnColor) + " no tiene movimientos validos.", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
			mGameStats.winner = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
			mGameStats.reason = GameOverReason::NO_MOVES;
			mIsGameOver = true;
			turnActionSuccessfullyCompleted = true;
			continue;
		}

		std::string turnMsg = "Turno de " + PlayerColorToString(mCurrentPlayerTurnColor) + ". ";
		if (mInCaptureSequence) {
			std::string forcedPiecePosStr = ToAlgebraic(mForcedPieceRow, mForcedPieceCol);
			turnMsg += "CONTINUAR CAPTURA con la pieza en " + forcedPiecePosStr + ".";
		}
		mView.DisplayMessage(turnMsg, true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_BLACK);

		if (mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER && dynamic_cast<ComputerPlayer*>(m_currentPlayerObject)) {
			GoToXY(0, turnMessageY + 1);
			mView.ClearLines(turnMessageY + 1, 2, CONSOLE_WIDTH_ASSUMED);
			GoToXY(0, turnMessageY + 1);
			mView.DisplayMessage("IA (" + PlayerColorToString(mCurrentPlayerTurnColor) + ") va a mover. Presione ENTER o ingrese 'stats'/'salir':", false, CONSOLE_COLOR_DARK_GRAY, CONSOLE_COLOR_BLACK);
			std::cout << std::endl << "> ";

			std::string cvc_command_line;
			if (std::cin.peek() == '\n') {
				std::cin.ignore();
			}
			std::getline(std::cin, cvc_command_line);
			std::string command_lower = cvc_command_line;
			std::transform(command_lower.begin(), command_lower.end(), command_lower.begin(),
				[](unsigned char c) { return std::tolower(c); }); // Usar lambda para std::tolower

			if (command_lower == "salir") {
				mView.DisplayMessage("Saliendo de la partida CvC por comando del observador...", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
				mIsGameOver = true;
				mGameStats.reason = GameOverReason::PLAYER_EXIT;
				mGameStats.winner = PlayerColor::NONE;
				turnActionSuccessfullyCompleted = true;
				continue;
			}
			else if (command_lower == "stats") {
				DisplayCurrentStats();
				int pressEnterCvCY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 7 + 6 + 1;
				GoToXY(0, pressEnterCvCY);
				mView.ClearLines(pressEnterCvCY, 1, CONSOLE_WIDTH_ASSUMED);
				GoToXY(0, pressEnterCvCY);
				mView.DisplayMessage("Estadisticas mostradas. Presione Enter para continuar con el turno de la IA...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
				if (std::cin.peek() == '\n') std::cin.ignore();
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				continue;
			}
			GoToXY(0, turnMessageY + 1);
			mView.ClearLines(turnMessageY + 1, 2, CONSOLE_WIDTH_ASSUMED);
			GoToXY(0, turnMessageY + 1);
		}

		if (!m_currentPlayerObject) {
			mView.DisplayMessage("Error crítico: Jugador actual no definido.", true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
			mIsGameOver = true;
			continue;
		}
		MoveInput userInput = m_currentPlayerObject->GetChosenMoveInput(
			mGameBoard,
			mMoveGenerator,
			mInCaptureSequence,
			mForcedPieceRow,
			mForcedPieceCol,
			mandatoryJumpsForCurrentPlayer
		);

		int feedbackMessageY = turnMessageY + 2;
		if (mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER && dynamic_cast<ComputerPlayer*>(m_currentPlayerObject)) {
			feedbackMessageY = turnMessageY + 1;
			feedbackMessageY = turnMessageY + 3;
		}

		GoToXY(0, feedbackMessageY);
		mView.ClearLines(feedbackMessageY, 8, CONSOLE_WIDTH_ASSUMED);
		GoToXY(0, feedbackMessageY);

		if (userInput.wantsToExit) {
			mView.DisplayMessage("Jugador " + PlayerColorToString(mCurrentPlayerTurnColor) + " elige salir de la partida...", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
			mIsGameOver = true;
			mGameStats.reason = GameOverReason::PLAYER_EXIT;
			mGameStats.winner = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
			turnActionSuccessfullyCompleted = true;
			mInCaptureSequence = false;
		}
		else if (userInput.wantsToShowStats && dynamic_cast<HumanPlayer*>(m_currentPlayerObject)) {
			DisplayCurrentStats();
			int pressEnterY = (GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 7) + 6;
			GoToXY(0, pressEnterY);
			mView.ClearLines(pressEnterY, 1, CONSOLE_WIDTH_ASSUMED);
			GoToXY(0, pressEnterY);
			mView.DisplayMessage("Presione Enter para continuar con su turno...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
			if (std::cin.peek() == '\n') std::cin.ignore();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			continue;
		}
		else if (!userInput.isValidFormat) {
			if (dynamic_cast<HumanPlayer*>(m_currentPlayerObject)) {
				mView.DisplayMessage("FORMATO DE ENTRADA INCORRECTO. Use 'Origen Destino' (ej: a3 b4).", true, CONSOLE_COLOR_LIGHT_RED, CONSOLE_COLOR_BLACK);
				mView.DisplayMessage("Presione Enter para reintentar...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
				if (std::cin.peek() == '\n') std::cin.ignore();
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			}
			else {
				mView.DisplayMessage("IA (" + PlayerColorToString(mCurrentPlayerTurnColor) + ") no pudo generar un movimiento valido.", true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
				mGameStats.winner = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
				mGameStats.reason = GameOverReason::NO_MOVES;
				mIsGameOver = true;
				turnActionSuccessfullyCompleted = true;
			}
		}
		else {
			int inputStartR = userInput.startRow;
			int inputStartC = userInput.startCol;
			int inputEndR = userInput.endRow;
			int inputEndC = userInput.endCol;
			bool actualMoveIsCapture = false;
			bool isValidAttemptGeneral = true;
			std::string specificErrorMessage = "";

			if (mInCaptureSequence) {
				if (inputStartR != mForcedPieceRow || inputStartC != mForcedPieceCol) {
					specificErrorMessage = "Error: Debe mover la pieza en " + ToAlgebraic(mForcedPieceRow, mForcedPieceCol) + " para continuar la captura.";
					isValidAttemptGeneral = false;
				}
				else {
					bool isValidContinuationJump = false;
					for (const auto& jump : mandatoryJumpsForCurrentPlayer) {
						if (jump.endR_ == inputEndR && jump.endC_ == inputEndC) {
							isValidContinuationJump = true;
							actualMoveIsCapture = true;
							break;
						}
					}
					if (!isValidContinuationJump) {
						specificErrorMessage = "MOVIMIENTO INVALIDO: Desde " + ToAlgebraic(mForcedPieceRow, mForcedPieceCol) +
							" no puede saltar a " + ToAlgebraic(inputEndR, inputEndC) + " en esta secuencia.\n";
						specificErrorMessage += "  Los saltos posibles son: ";
						if (mandatoryJumpsForCurrentPlayer.empty()) {
							specificErrorMessage += " (Ninguno - la secuencia debio terminar)";
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
				if (!mMoveGenerator.IsValidMove(mGameBoard, inputStartR, inputStartC, inputEndR, inputEndC, mCurrentPlayerTurnColor, actualMoveIsCapture)) {
					isValidAttemptGeneral = false;
					PieceType attemptedMovePieceType = mGameBoard.GetPieceAt(inputStartR, inputStartC);

					if (currentMandatoryAction == MandatoryActionType::KING_CAPTURE) {
						specificErrorMessage = "MOVIMIENTO INVALIDO: Debes realizar una captura con una DAMA.\n";
						if (!mandatoryJumpsForCurrentPlayer.empty()) {
							specificErrorMessage += "  Captura(s) de Dama disponible(s):\n";
							for (const auto& jump_move : mandatoryJumpsForCurrentPlayer) {
								specificErrorMessage += "  -> " + jump_move.ToNotation() + "\n";
							}
						}
						else {
							specificErrorMessage += "  (Error interno: Se esperaba captura de Dama pero no se encontraron obligatorias).\n";
						}
					}
					else if (currentMandatoryAction == MandatoryActionType::PAWN_CAPTURE) {
						if (attemptedMovePieceType == PieceType::P1_KING || attemptedMovePieceType == PieceType::P2_KING) {
							specificErrorMessage = "MOVIMIENTO INVALIDO: No puedes mover/capturar con una Dama\n";
							specificErrorMessage += "  cuando una captura de PEON es obligatoria (y ninguna Dama puede capturar).\n";
						}
						else {
							specificErrorMessage = "MOVIMIENTO INVALIDO: Debes realizar una captura con un PEON.\n";
						}
						if (!mandatoryJumpsForCurrentPlayer.empty()) {
							specificErrorMessage += "  Captura(s) de Peon disponible(s):\n";
							for (const auto& jump_move : mandatoryJumpsForCurrentPlayer) {
								specificErrorMessage += "  -> " + jump_move.ToNotation() + "\n";
							}
						}
						else {
							specificErrorMessage += "  (Error interno: Se esperaba captura de Peón pero no se encontraron obligatorias).\n";
						}
					}
					else if (currentMandatoryAction == MandatoryActionType::NONE) {
						bool proposedMoveWasIntendedAsCapture = (std::abs(inputStartR - inputEndR) == 2 && std::abs(inputStartC - inputEndC) == 2);
						if (proposedMoveWasIntendedAsCapture) {
							specificErrorMessage = "MOVIMIENTO INVALIDO: No se puede realizar una captura si no es obligatoria.";
						}
						else if (mGameBoard.GetPieceAt(inputEndR, inputEndC) != PieceType::EMPTY) {
							specificErrorMessage = "MOVIMIENTO INVALIDO: Casilla destino (" + ToAlgebraic(inputEndR, inputEndC) + ") esta ocupada.";
						}
						else {
							specificErrorMessage = "MOVIMIENTO INVALIDO: El movimiento de " +
								ToAlgebraic(inputStartR, inputStartC) + " a " +
								ToAlgebraic(inputEndR, inputEndC) + " no es legal.";
						}
					}
					else {
						specificErrorMessage = "MOVIMIENTO INVALIDO por reglas no especificadas.";
					}
				}
			}

			if (!isValidAttemptGeneral) {
				mView.DisplayMessage(specificErrorMessage, true, CONSOLE_COLOR_LIGHT_RED, CONSOLE_COLOR_BLACK);
				if (dynamic_cast<HumanPlayer*>(m_currentPlayerObject)) {
					mView.DisplayMessage("Presione Enter para reintentar...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
					if (std::cin.peek() == '\n') std::cin.ignore();
					std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				}
				else if (mCurrentGameMode == GameMode::COMPUTER_VS_COMPUTER) {
					mView.DisplayMessage("Presione Enter para continuar (IA fallo)...", true, CONSOLE_COLOR_DARK_GRAY, CONSOLE_COLOR_BLACK);
					if (std::cin.peek() == '\n') std::cin.ignore();
					std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				}
				continue;
			}

			Move currentSegmentMove;
			currentSegmentMove.startR_ = inputStartR; currentSegmentMove.startC_ = inputStartC;
			currentSegmentMove.endR_ = inputEndR; currentSegmentMove.endC_ = inputEndC;
			currentSegmentMove.pieceMoved_ = mGameBoard.GetPieceAt(inputStartR, inputStartC);
			currentSegmentMove.playerColor_ = mCurrentPlayerTurnColor;
			currentSegmentMove.isCapture_ = actualMoveIsCapture;

			PieceType pieceToMove = mGameBoard.GetPieceAt(inputStartR, inputStartC);
			mGameBoard.SetPieceAt(inputEndR, inputEndC, pieceToMove);
			mGameBoard.SetPieceAt(inputStartR, inputStartC, PieceType::EMPTY);

			if (mCurrentGameMode != GameMode::COMPUTER_VS_COMPUTER || !dynamic_cast<ComputerPlayer*>(m_currentPlayerObject)) {
				mView.DisplayMessage("Movimiento realizado: " + currentSegmentMove.ToNotation(), true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
			}
			else {
				mView.DisplayMessage("IA (" + PlayerColorToString(mCurrentPlayerTurnColor) + ") realizo: " + currentSegmentMove.ToNotation(), true, CONSOLE_COLOR_CYAN, CONSOLE_COLOR_BLACK);
			}
			mLastMove = currentSegmentMove;

			if (actualMoveIsCapture) {
				int capturedRow = inputStartR + (inputEndR - inputStartR) / 2;
				int capturedCol = inputStartC + (inputEndC - inputStartC) / 2;
				mGameBoard.SetPieceAt(capturedRow, capturedCol, PieceType::EMPTY);
				if (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) mGameStats.player1CapturedCount++;
				else mGameStats.player2CapturedCount++;
			}
			mGameBoard.PromotePieceIfNecessary(inputEndR, inputEndC);

			if (actualMoveIsCapture) {
				std::vector<Move> furtherJumps = mMoveGenerator.GetPossibleJumpsForSpecificPiece(mGameBoard, inputEndR, inputEndC);
				if (!furtherJumps.empty()) {
					mInCaptureSequence = true;
					mForcedPieceRow = inputEndR; mForcedPieceCol = inputEndC;
					if (dynamic_cast<HumanPlayer*>(m_currentPlayerObject) && std::cin.rdbuf()->in_avail() > 0) {
						std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
					}
				}
				else {
					mInCaptureSequence = false;
					turnActionSuccessfullyCompleted = true;
				}
			}
			else {
				mInCaptureSequence = false;
				turnActionSuccessfullyCompleted = true;
			}

			if (turnActionSuccessfullyCompleted) {
				mGameStats.currentTurnNumber++;
				PlayerColor opponent = (mCurrentPlayerTurnColor == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
				if (mGameBoard.GetPieceCount(opponent) == 0) {
					mView.DisplayMessage("FIN DEL JUEGO!", true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
					mGameStats.winner = mCurrentPlayerTurnColor;
					mGameStats.reason = GameOverReason::NO_PIECES;
					mIsGameOver = true;
				}
				else if (!mIsGameOver) {
					SwitchPlayer();
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
	int finalMessageStartY = GAME_TITLE_LINES;
	GoToXY(0, finalMessageStartY);
	mView.ClearLines(finalMessageStartY, BOARD_VISUAL_HEIGHT + 20, CONSOLE_WIDTH_ASSUMED);
	GoToXY(0, finalMessageStartY);

	mView.DisplayMessage("--- PARTIDA FINALIZADA ---", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);

	std::string reasonStr = "Razon desconocida.";
	switch (mGameStats.reason) {
	case GameOverReason::NO_PIECES:
		reasonStr = PlayerColorToString((mGameStats.winner == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1) + " se quedo sin fichas.";
		break;
	case GameOverReason::NO_MOVES:
		reasonStr = PlayerColorToString((mGameStats.winner == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1) + " no tiene movimientos (bloqueado).";
		break;
	case GameOverReason::PLAYER_EXIT:
		reasonStr = "Un jugador ha salido de la partida.";
		break;
	case GameOverReason::STALEMATE_BY_RULES:
		reasonStr = "Empate por reglas del juego (ej. limite de movimientos).";
		break;
	case GameOverReason::NONE:
		if (mGameBoard.GetPieceCount(PlayerColor::PLAYER_1) == 0 && mGameBoard.GetPieceCount(PlayerColor::PLAYER_2) > 0 && mGameStats.winner == PlayerColor::PLAYER_2) {
			reasonStr = PlayerColorToString(PlayerColor::PLAYER_1) + " se quedo sin fichas.";
		}
		else if (mGameBoard.GetPieceCount(PlayerColor::PLAYER_2) == 0 && mGameBoard.GetPieceCount(PlayerColor::PLAYER_1) > 0 && mGameStats.winner == PlayerColor::PLAYER_1) {
			reasonStr = PlayerColorToString(PlayerColor::PLAYER_2) + " se quedo sin fichas.";
		}
		else if (mIsGameOver) {
			reasonStr = "El juego termino por una condicion no especificada explicitamente.";
		}
		else {
			reasonStr = "El juego aun no ha terminado.";
		}
		break;
	default:
		reasonStr = "El juego termino por una razon no manejada.";
		break;
	}

	if (mGameStats.winner != PlayerColor::NONE) {
		mView.DisplayMessage("GANADOR: " + PlayerColorToString(mGameStats.winner) + "!", true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
		mView.DisplayMessage("Razon: " + reasonStr, true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
	}
	else if (mGameStats.reason != GameOverReason::NONE && mGameStats.reason != GameOverReason::STALEMATE_BY_RULES) {
		mView.DisplayMessage("Juego terminado. " + reasonStr, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	}
	else if (mGameStats.reason == GameOverReason::STALEMATE_BY_RULES) {
		mView.DisplayMessage("EMPATE. " + reasonStr, true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	}
	else {
		mView.DisplayMessage("Juego terminado.", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
	}

	int statsAnnounceY = finalMessageStartY + 4;
	GoToXY(0, statsAnnounceY);
	DisplayCurrentStats();

	int pressEnterAnnounceY = statsAnnounceY + 6;
	GoToXY(0, pressEnterAnnounceY);
	mView.ClearLines(pressEnterAnnounceY, 1, CONSOLE_WIDTH_ASSUMED);
	GoToXY(0, pressEnterAnnounceY);
	mView.DisplayMessage("Presione Enter para volver al menu principal...", true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_BLACK);

	if (std::cin.rdbuf()->in_avail() > 0) {
		std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
	}
	std::cin.get();
}