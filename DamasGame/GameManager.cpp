// -----------------------------------------------------------------------------
// GameManager.cpp
// Implementaci�n de la clase GameManager.
// -----------------------------------------------------------------------------

#include "GameManager.h"    
#include "ConsoleView.h"    
#include "InputHandler.h"   
#include "Board.h"          
#include "CommonTypes.h"    
#include "MoveGenerator.h"  

#include <iostream>         
#include <string>           
#include <limits>           // Para std::numeric_limits
#include <vector>           

GameManager::GameManager(Board& board, ConsoleView& view, InputHandler& inputHandler)
    : mGameBoard(board),
    mView(view),
    mInputHandler(inputHandler),
    mMoveGenerator(),
    mCurrentPlayer(PlayerColor::PLAYER_1),
    mIsGameOver(false),
    mLastMove() {
}

void GameManager::DisplayCurrentStats() {
    mView.DisplayMessage("--- Estadisticas Actuales ---", true);
    // ... (resto de la función DisplayCurrentStats como la teníamos) ...
    mView.DisplayMessage("Turno Actual Nro: " + std::to_string(mGameStats.currentTurnNumber), true);
    int piecesP1 = mGameBoard.GetPieceCount(PlayerColor::PLAYER_1); int kingsP1 = mGameBoard.GetKingCount(PlayerColor::PLAYER_1); int menP1 = piecesP1 - kingsP1;
    int piecesP2 = mGameBoard.GetPieceCount(PlayerColor::PLAYER_2); int kingsP2 = mGameBoard.GetKingCount(PlayerColor::PLAYER_2); int menP2 = piecesP2 - kingsP2;
    mView.DisplayMessage(PlayerColorToString(PlayerColor::PLAYER_1) + ": " + std::to_string(piecesP1) + " piezas (" + std::to_string(menP1) + " peones, " + std::to_string(kingsP1) + " damas). Piezas del oponente capturadas: " + std::to_string(mGameStats.player1CapturedCount), true);
    mView.DisplayMessage(PlayerColorToString(PlayerColor::PLAYER_2) + ": " + std::to_string(piecesP2) + " piezas (" + std::to_string(menP2) + " peones, " + std::to_string(kingsP2) + " damas). Piezas del oponente capturadas: " + std::to_string(mGameStats.player2CapturedCount), true);
    mView.DisplayMessage("-----------------------------", true);
}

void GameManager::DisplayLastMove() {
    if (!mLastMove.IsNull()) {
        mView.DisplayMessage("Ultimo movimiento: " + mLastMove.ToNotation(), true);
        // Ya no necesitamos la línea de separación aquí, InputHandler la pondrá
    }
}

void GameManager::StartNewGame() {
    mGameBoard.InitializeBoard();
    mCurrentPlayer = PlayerColor::PLAYER_1;
    mIsGameOver = false;
    mGameStats = GameStats{};
    mLastMove = Move{};
}

void GameManager::RunGameLoop() {
    // Mensajes iniciales del juego se muestran una vez.
    mView.ClearScreen();
    mView.DisplayMessage("=== NUEVO JUEGO DE DAMAS: HUMANO VS HUMANO ===", true);
    mView.DisplayMessage("Reglas actuales: Peones y Damas implementados con coronacion.", true);
    mView.DisplayMessage("Comandos: 'Origen Destino' (ej: b6 a5), 'stats', 'salir'.", true);
    // No es necesario un DisplayBoard aquí, ProcessPlayerTurn lo hará.

    StartNewGame(); // Inicializa el tablero y estado del juego

    while (!mIsGameOver) {
        ProcessPlayerTurn();
    }
    AnnounceResult();
}

void GameManager::ProcessPlayerTurn() {
    // Bucle para el turno del jugador actual. Se repite si el input fue 'stats',
    // o si hubo un error de input/movimiento y el jugador debe reintentar.
    bool turnActionSuccessfullyCompleted = false;

    while (!turnActionSuccessfullyCompleted && !mIsGameOver) {
        // 1. PREPARAR PANTALLA ANTES DE PEDIR INPUT
        mView.ClearScreen();
        mView.DisplayBoard(mGameBoard);
        DisplayLastMove();
        // El mensaje de turno y ejemplos ahora lo maneja InputHandler, que se llama a continuación.

        // 2. VERIFICAR SI HAY MOVIMIENTOS (FIN DE JUEGO)
        if (!mMoveGenerator.HasAnyValidMoves(mGameBoard, mCurrentPlayer)) {
            mView.DisplayMessage("El jugador " + PlayerColorToString(mCurrentPlayer) + " no tiene movimientos validos.", true);
            PlayerColor winner = (mCurrentPlayer == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
            mView.DisplayMessage("FIN DEL JUEGO. GANA " + PlayerColorToString(winner) + "!", true);
            mIsGameOver = true;
            turnActionSuccessfullyCompleted = true;
            continue;
        }

        // 3. PEDIR INPUT
        MoveInput userInput = mInputHandler.GetPlayerMoveInput(mCurrentPlayer);

        // 4. PROCESAR INPUT
        if (userInput.wantsToExit) {
            mView.DisplayMessage("Juego terminado por el jugador " + PlayerColorToString(mCurrentPlayer) + ".", true);
            mIsGameOver = true;
            turnActionSuccessfullyCompleted = true;
        }
        else if (userInput.wantsToShowStats) {
            // La pantalla ya está limpia y el tablero/último mov mostrados.
            DisplayCurrentStats();
            std::cout << "Presione Enter para continuar con su turno...";
            if (std::cin.peek() == '\n') { std::cin.ignore(); }
            else { std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); }
            // El bucle while continuará, y al inicio se limpiará y redibujará todo.
        }
        else if (!userInput.isValidFormat) {
            // InputHandler ya mostró "COMANDO/FORMATO INCORRECTO..."
            // El mensaje de "Presione Enter para reintentar" se puede añadir aquí si se desea,
            // o confiar en que el usuario lo hará para que el bucle continúe.
            mView.DisplayMessage("Presione Enter para reintentar...", true); // Para claridad
            if (std::cin.peek() == '\n') { std::cin.ignore(); }
            else { std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); }
        }
        else { // Intento de movimiento (userInput.isValidFormat es true)
            bool wasCapture = false;
            if (mMoveGenerator.IsValidMove(mGameBoard, userInput.startRow, userInput.startCol, userInput.endRow, userInput.endCol, mCurrentPlayer, wasCapture)) {

                mLastMove = Move();
                mLastMove.startR_ = userInput.startRow; mLastMove.startC_ = userInput.startCol;
                mLastMove.endR_ = userInput.endRow; mLastMove.endC_ = userInput.endCol;
                mLastMove.pieceMoved_ = mGameBoard.GetPieceAt(userInput.startRow, userInput.startCol);
                mLastMove.playerColor_ = mCurrentPlayer;
                mLastMove.isCapture_ = wasCapture;

                mGameBoard.SetPieceAt(userInput.endRow, userInput.endCol, mLastMove.pieceMoved_);
                mGameBoard.SetPieceAt(userInput.startRow, userInput.startCol, PieceType::EMPTY);

                std::string feedbackMsg = "Movimiento realizado.";
                if (wasCapture) {
                    int capturedRow = userInput.startRow + (userInput.endRow - userInput.startRow) / 2;
                    int capturedCol = userInput.startCol + (userInput.endCol - userInput.startCol) / 2;
                    mGameBoard.SetPieceAt(capturedRow, capturedCol, PieceType::EMPTY);
                    feedbackMsg += " ¡PIEZA CAPTURADA!";
                    if (mCurrentPlayer == PlayerColor::PLAYER_1) mGameStats.player1CapturedCount++;
                    else mGameStats.player2CapturedCount++;
                }
                mView.DisplayMessage(feedbackMsg, true);

                mGameBoard.PromotePieceIfNecessary(userInput.endRow, userInput.endCol);
                mGameStats.currentTurnNumber++;
                turnActionSuccessfullyCompleted = true;
                SwitchPlayer();
            }
            else {
                mView.DisplayMessage("MOVIMIENTO INVALIDO. Razones posibles:", true);
                mView.DisplayMessage("- Casilla origen no contiene tu pieza.", false);
                mView.DisplayMessage(" - Movimiento no sigue reglas de peon/dama.", false);
                mView.DisplayMessage(" - Intento de captura invalido.", false);
                mView.DisplayMessage(" - Casilla destino ocupada (para mov. simples) o no es salto valido.", true);
                mView.DisplayMessage("Presione Enter para reintentar...", true);
                if (std::cin.peek() == '\n') { std::cin.ignore(); }
                else { std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); }
            }
        }
        // Si el turno no se completó con un movimiento/salida válido, el bucle while continuará.
        // Al inicio de la siguiente iteración del while, la pantalla se limpiará y redibujará.
    }
}

void GameManager::SwitchPlayer() {
    if (!mIsGameOver) {
        mCurrentPlayer = (mCurrentPlayer == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
    }
}

void GameManager::AnnounceResult() {
    // No necesitamos ClearScreen aquí si ProcessPlayerTurn lo hizo en su última acción
    // o si el juego terminó por una condición dentro de ProcessPlayerTurn que ya mostró mensajes.
    // Pero para asegurar un estado limpio final:
    mView.ClearScreen();
    mView.DisplayBoard(mGameBoard);
    mView.DisplayMessage("", true);
    DisplayLastMove();

    // ... (resto de AnnounceResult como lo teníamos) ...
    PlayerColor winner = PlayerColor::NONE; int piecesP1 = mGameBoard.GetPieceCount(PlayerColor::PLAYER_1); int piecesP2 = mGameBoard.GetPieceCount(PlayerColor::PLAYER_2);
    if (mIsGameOver) { if (piecesP1 == 0 && piecesP2 > 0) winner = PlayerColor::PLAYER_2; else if (piecesP2 == 0 && piecesP1 > 0) winner = PlayerColor::PLAYER_1; if (winner != PlayerColor::NONE) { mView.DisplayMessage("CONDICION FINAL: Ganador por falta de piezas: " + PlayerColorToString(winner), true); } else { /* El mensaje de "GANA X" o "Juego terminado por Y" ya se mostró */ mView.DisplayMessage("Juego Finalizado.", true); } }
    DisplayCurrentStats();
    mView.DisplayMessage("Presione Enter para salir del juego...", false);
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    if (std::cin.gcount() == 0 && std::cin.peek() != '\n') {}
    std::cin.get();
}