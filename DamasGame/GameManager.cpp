// GameManager.cpp

#include "GameManager.h"
#include "ConsoleView.h"
#include "InputHandler.h"
#include "Board.h"
#include "CommonTypes.h" 
#include "MoveGenerator.h"

#include <iostream>
#include <string>
#include <limits>    
#include <vector>
#include <cmath>     

// Constantes para el layout de la pantalla de juego
const int GAME_TITLE_LINES = 4;
const int BOARD_VISUAL_HEIGHT = 1 + 1 + (Board::BOARD_SIZE * 2 - 1) + 1; // Coords Sup + Borde Sup + Filas Contenido + Borde Inf = 19
const int CONSOLE_WIDTH_ASSUMED = 80;


GameManager::GameManager(Board& board, ConsoleView& view, InputHandler& inputHandler)
    : mGameBoard(board),
    mView(view),
    mInputHandler(inputHandler),
    mMoveGenerator(),
    mCurrentPlayer(PlayerColor::PLAYER_1),
    mIsGameOver(false),
    mLastMove(),
    mInCaptureSequence(false),
    mForcedPieceRow(-1),
    mForcedPieceCol(-1),
    mCurrentGameMode(GameMode::NONE)
{
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
            switch (selectedOption) {
            case 1:
                mView.SetGameColorsAndClear();
                StartNewGame(GameMode::PLAYER_VS_PLAYER);
                RunGameLoop();
                break;
            case 2:
                mView.SetMenuColorsAndClear();
                GoToXY(0, 10);
                mView.DisplayMessage("Modo Jugador vs Computadora no implementado aun.", true, CONSOLE_COLOR_YELLOW);
                mView.DisplayMessage("Presione Enter para volver al menu...", true);
                if (std::cin.peek() == '\n') std::cin.ignore();
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                break;
            case 3:
                mView.SetMenuColorsAndClear();
                GoToXY(0, 10);
                mView.DisplayMessage("Modo Computadora vs Computadora no implementado aun.", true, CONSOLE_COLOR_YELLOW);
                mView.DisplayMessage("Presione Enter para volver al menu...", true);
                if (std::cin.peek() == '\n') std::cin.ignore();
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                break;
            case 4:
                ShowGlobalStats();
                break;
            case 5:
                exitMenu = true;
                break;
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

void GameManager::StartNewGame(GameMode mode) {
    mCurrentGameMode = mode;
    mGameBoard.InitializeBoard();
    mCurrentPlayer = PlayerColor::PLAYER_1;
    mIsGameOver = false;
    mGameStats = GameStats{};
    mLastMove = Move{};
    mInCaptureSequence = false;
    mForcedPieceRow = -1;
    mForcedPieceCol = -1;
}

void GameManager::RunGameLoop() {
    if (mCurrentGameMode == GameMode::NONE) {
        mView.DisplayMessage("Error: Modo de juego no seleccionado.", true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
        return;
    }
    GoToXY(0, 0);
    mView.ClearLines(0, GAME_TITLE_LINES, CONSOLE_WIDTH_ASSUMED);
    GoToXY(0, 0);

    if (mCurrentGameMode == GameMode::PLAYER_VS_PLAYER) {
        mView.DisplayMessage("=== JUEGO DE DAMAS: JUGADOR VS JUGADOR ===", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
    }
    mView.DisplayMessage("Reglas: Capturas multiples. Captura es obligatoria.", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
    mView.DisplayMessage("Comandos: 'Origen Destino' (ej: b6 a5), 'stats', 'salir'.", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
    mView.DisplayMessage("--------------------------------------------------", true, CONSOLE_COLOR_WHITE, CONSOLE_COLOR_BLACK);

    mIsGameOver = false;

    while (!mIsGameOver) {
        ProcessPlayerTurn();
    }
    AnnounceResult();
}

void GameManager::DisplayCurrentStats() {
    // Calcula la Y de inicio para las estadísticas.
    // Asumimos que esto se llama DESPUÉS de que el prompt ">" de InputHandler se haya mostrado.
    // El prompt usualmente está en turnMessageY + 1.
    // Las estadísticas irán debajo de un posible mensaje de feedback.
    int statsY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 1 + 1 + 2 + 1; // Título + Tablero + LastMove + TurnMsg + Prompt + Espacio_para_prompt_>
    // Esta línea es donde podría ir el feedback de movimiento. Stats van debajo.
    statsY += 1; // Una línea más abajo para las stats.

    GoToXY(0, statsY);
    mView.ClearLines(statsY, 6, CONSOLE_WIDTH_ASSUMED); // Limpiar área para stats (aprox 6 líneas)
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
        mView.ClearLines(GAME_TITLE_LINES, BOARD_VISUAL_HEIGHT + 15, CONSOLE_WIDTH_ASSUMED); // Limpiar área de juego + mensajes
        GoToXY(0, GAME_TITLE_LINES);

        mView.DisplayBoard(mGameBoard, CONSOLE_COLOR_BLACK);
        DisplayLastMove();

        int turnMessageY = GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 1;
        GoToXY(0, turnMessageY);
        // mView.ClearLines(turnMessageY, 1, CONSOLE_WIDTH_ASSUMED); // No es necesario si el área general ya se limpió
        // GoToXY(0, turnMessageY);

        if (!mInCaptureSequence && !mMoveGenerator.HasAnyValidMoves(mGameBoard, mCurrentPlayer)) {
            mView.DisplayMessage("El jugador " + PlayerColorToString(mCurrentPlayer) + " no tiene movimientos validos.", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
            PlayerColor winner = (mCurrentPlayer == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
            mView.DisplayMessage("FIN DEL JUEGO. GANA " + PlayerColorToString(winner) + "!", true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
            mIsGameOver = true;
            turnActionSuccessfullyCompleted = true;
            continue;
        }

        std::string turnMsg = "Turno de " + PlayerColorToString(mCurrentPlayer) + ". ";
        if (mInCaptureSequence) {
            std::string forcedPiecePosStr = ToAlgebraic(mForcedPieceRow, mForcedPieceCol);
            turnMsg += "CONTINUAR CAPTURA con la pieza en " + forcedPiecePosStr + ".";
        }
        mView.DisplayMessage(turnMsg, true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_BLACK);

        MoveInput userInput = mInputHandler.GetPlayerMoveInput(mCurrentPlayer);

        int feedbackMessageY = turnMessageY + 2;
        GoToXY(0, feedbackMessageY);
        mView.ClearLines(feedbackMessageY, 6, CONSOLE_WIDTH_ASSUMED);
        GoToXY(0, feedbackMessageY);

        if (userInput.wantsToExit) {
            mView.DisplayMessage("Saliendo de la partida...", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
            mIsGameOver = true;
            turnActionSuccessfullyCompleted = true;
            mInCaptureSequence = false;
        }
        else if (userInput.wantsToShowStats) {
            // El tablero y mensajes de turno ya están visibles.
            // DisplayCurrentStats se posicionará y limpiará su propia área.
            DisplayCurrentStats();

            // Posicionar el mensaje "Presione Enter" debajo de las estadísticas.
            // DisplayCurrentStats usa ~6 líneas. Su Y de inicio es calculado en la propia función.
            // Para evitar solapamientos, calculamos una Y segura para el "Presione Enter".
            // Asumiendo que statsY es donde empiezan las stats (calculado en DisplayCurrentStats).
            int pressEnterY = (GAME_TITLE_LINES + BOARD_VISUAL_HEIGHT + 7) + 6; // statsY (aprox) + altura_stats
            GoToXY(0, pressEnterY);
            mView.ClearLines(pressEnterY, 1, CONSOLE_WIDTH_ASSUMED); // Limpiar solo su línea
            GoToXY(0, pressEnterY);
            mView.DisplayMessage("Presione Enter para continuar con su turno...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);

            if (std::cin.peek() == '\n') std::cin.ignore();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

            // turnActionSuccessfullyCompleted sigue siendo false,
            // por lo que el bucle while de ProcessPlayerTurn se repetirá.
            // La primera acción del bucle es limpiar el área de juego y redibujar el tablero.
            continue; // Saltar el resto de la lógica del turno y volver al inicio del bucle.
        }
        else if (!userInput.isValidFormat) {
            mView.DisplayMessage("FORMATO DE ENTRADA INCORRECTO. Use 'Origen Destino' (ej: b6 a5).", true, CONSOLE_COLOR_LIGHT_RED, CONSOLE_COLOR_BLACK);
            mView.DisplayMessage("Presione Enter para reintentar...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
            if (std::cin.peek() == '\n') std::cin.ignore();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
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
                    std::vector<Move> possibleNextJumps = mMoveGenerator.GetPossibleJumpsForSpecificPiece(mGameBoard, mForcedPieceRow, mForcedPieceCol);
                    bool isValidContinuationJump = false;
                    for (const auto& jump : possibleNextJumps) {
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
                        if (possibleNextJumps.empty()) {
                            specificErrorMessage += " (Ninguno - la secuencia debio terminar)";
                            mInCaptureSequence = false;
                        }
                        else {
                            for (size_t i = 0; i < possibleNextJumps.size(); ++i) {
                                specificErrorMessage += ToAlgebraic(possibleNextJumps[i].endR_, possibleNextJumps[i].endC_) + (i == possibleNextJumps.size() - 1 ? "" : ", ");
                            }
                        }
                        isValidAttemptGeneral = false;
                    }
                }
            }
            else {
                PieceType pieceAtStart = mGameBoard.GetPieceAt(inputStartR, inputStartC);
                if (pieceAtStart == PieceType::EMPTY) {
                    specificErrorMessage = "Casilla origen (" + ToAlgebraic(inputStartR, inputStartC) + ") esta vacia.";
                    isValidAttemptGeneral = false;
                }
                else if (mMoveGenerator.GetPlayerFromPiece(pieceAtStart) != mCurrentPlayer) {
                    specificErrorMessage = "La pieza en " + ToAlgebraic(inputStartR, inputStartC) + " no te pertenece.";
                    isValidAttemptGeneral = false;
                }
                if (isValidAttemptGeneral) {
                    if (!mMoveGenerator.IsValidMove(mGameBoard, inputStartR, inputStartC, inputEndR, inputEndC, mCurrentPlayer, actualMoveIsCapture)) {
                        isValidAttemptGeneral = false;
                        std::vector<Move> availableJumpsAnywhere;
                        bool captureWasMandatory = false;
                        for (int r_scan = 0; r_scan < Board::BOARD_SIZE; ++r_scan) {
                            for (int c_scan = 0; c_scan < Board::BOARD_SIZE; ++c_scan) {
                                if (mMoveGenerator.GetPlayerFromPiece(mGameBoard.GetPieceAt(r_scan, c_scan)) == mCurrentPlayer) {
                                    std::vector<Move> jumpsForScannedPiece = mMoveGenerator.GetPossibleJumpsForSpecificPiece(mGameBoard, r_scan, c_scan);
                                    if (!jumpsForScannedPiece.empty()) {
                                        captureWasMandatory = true;
                                        availableJumpsAnywhere.insert(availableJumpsAnywhere.end(), jumpsForScannedPiece.begin(), jumpsForScannedPiece.end());
                                    }
                                }
                            }
                        }
                        bool proposedMoveWasIntendedAsCapture = (std::abs(inputStartR - inputEndR) == 2 && std::abs(inputStartC - inputEndC) == 2);
                        if (captureWasMandatory && !proposedMoveWasIntendedAsCapture) {
                            specificErrorMessage = "MOVIMIENTO INVALIDO: Debes realizar una captura.\n";
                            specificErrorMessage += "  Captura(s) disponible(s) para ti:\n";
                            for (const auto& jump_move : availableJumpsAnywhere) {
                                specificErrorMessage += "  -> " + jump_move.ToNotation() + "\n";
                            }
                        }
                        else if (mGameBoard.GetPieceAt(inputEndR, inputEndC) != PieceType::EMPTY && !proposedMoveWasIntendedAsCapture) {
                            specificErrorMessage = "MOVIMIENTO INVALIDO: Casilla destino (" + ToAlgebraic(inputEndR, inputEndC) + ") esta ocupada (para mov. simple).";
                        }
                        else {
                            specificErrorMessage = "MOVIMIENTO INVALIDO: El movimiento de " +
                                ToAlgebraic(inputStartR, inputStartC) + " a " +
                                ToAlgebraic(inputEndR, inputEndC) + " no es legal segun las reglas.";
                        }
                    }
                }
            }

            if (!isValidAttemptGeneral) {
                mView.DisplayMessage(specificErrorMessage, true, CONSOLE_COLOR_LIGHT_RED, CONSOLE_COLOR_BLACK);
                mView.DisplayMessage("Presione Enter para reintentar...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
                if (std::cin.peek() == '\n') std::cin.ignore();
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                continue;
            }

            Move currentSegmentMove;
            currentSegmentMove.startR_ = inputStartR; currentSegmentMove.startC_ = inputStartC;
            currentSegmentMove.endR_ = inputEndR; currentSegmentMove.endC_ = inputEndC;
            currentSegmentMove.pieceMoved_ = mGameBoard.GetPieceAt(inputStartR, inputStartC);
            currentSegmentMove.playerColor_ = mCurrentPlayer;
            currentSegmentMove.isCapture_ = actualMoveIsCapture;

            PieceType pieceToMove = mGameBoard.GetPieceAt(inputStartR, inputStartC);
            mGameBoard.SetPieceAt(inputEndR, inputEndC, pieceToMove);
            mGameBoard.SetPieceAt(inputStartR, inputStartC, PieceType::EMPTY);
            mView.DisplayMessage("Movimiento realizado: " + currentSegmentMove.ToNotation(), true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
            mLastMove = currentSegmentMove;

            if (actualMoveIsCapture) {
                int capturedRow = inputStartR + (inputEndR - inputStartR) / 2;
                int capturedCol = inputStartC + (inputEndC - inputStartC) / 2;
                mGameBoard.SetPieceAt(capturedRow, capturedCol, PieceType::EMPTY);
                if (mCurrentPlayer == PlayerColor::PLAYER_1) mGameStats.player1CapturedCount++;
                else mGameStats.player2CapturedCount++;
            }
            mGameBoard.PromotePieceIfNecessary(inputEndR, inputEndC);

            if (actualMoveIsCapture) {
                std::vector<Move> furtherJumps = mMoveGenerator.GetPossibleJumpsForSpecificPiece(mGameBoard, inputEndR, inputEndC);
                if (!furtherJumps.empty()) {
                    mInCaptureSequence = true;
                    mForcedPieceRow = inputEndR; mForcedPieceCol = inputEndC;
                    if (std::cin.rdbuf()->in_avail() > 0) { std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); }
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
                PlayerColor opponent = (mCurrentPlayer == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
                if (mGameBoard.GetPieceCount(opponent) == 0) {
                    mView.DisplayMessage("FIN DEL JUEGO. " + PlayerColorToString(mCurrentPlayer) + " gana por captura de todas las piezas!", true, CONSOLE_COLOR_LIGHT_GREEN, CONSOLE_COLOR_BLACK);
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
        mCurrentPlayer = (mCurrentPlayer == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
    }
}

void GameManager::AnnounceResult() {
    int finalMessageStartY = GAME_TITLE_LINES;
    GoToXY(0, finalMessageStartY);
    mView.ClearLines(finalMessageStartY, BOARD_VISUAL_HEIGHT + 20, CONSOLE_WIDTH_ASSUMED);
    GoToXY(0, finalMessageStartY);

    if (mIsGameOver && mGameStats.currentTurnNumber > 0) {
        mView.DisplayMessage("--- PARTIDA FINALIZADA ---", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
    }
    else if (mIsGameOver) {
        mView.DisplayMessage("Partida no iniciada o terminada.", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
    }

    // Posicionar las stats después del mensaje "PARTIDA FINALIZADA"
    // AnnounceResult solo se llama una vez al final, así que este GoToXY para DisplayCurrentStats está bien.
    int statsAnnounceY = finalMessageStartY + 2; // Dejar una línea de espacio
    GoToXY(0, statsAnnounceY);
    DisplayCurrentStats();

    // Posicionar "Presione Enter" debajo de las stats.
    // DisplayCurrentStats usa ~6 líneas. Su Y de inicio es ahora 'statsAnnounceY'.
    int pressEnterAnnounceY = statsAnnounceY + 6;
    GoToXY(0, pressEnterAnnounceY);
    mView.ClearLines(pressEnterAnnounceY, 1, CONSOLE_WIDTH_ASSUMED); // Limpiar su línea
    GoToXY(0, pressEnterAnnounceY);
    mView.DisplayMessage("Presione Enter para volver al menu principal...", true, CONSOLE_COLOR_LIGHT_CYAN, CONSOLE_COLOR_BLACK);

    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
    std::cin.get();
}