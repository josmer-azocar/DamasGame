// -----------------------------------------------------------------------------
// GameManager.cpp
// Implementación de la clase GameManager, integrando MoveGenerator básico.
// -----------------------------------------------------------------------------

#include "GameManager.h"
#include "ConsoleView.h"
#include "InputHandler.h"
#include "Board.h"
#include "CommonTypes.h"
#include "MoveGenerator.h" // Asegúrate que este include esté y sea el correcto

#include <iostream>
#include <string>
#include <limits>
#include <vector> // Para usar std::vector<Move> (aunque no directamente en este archivo aún)

// Constructor
GameManager::GameManager(Board& board, ConsoleView& view, InputHandler& inputHandler)
    : mGameBoard(board),
    mView(view),
    mInputHandler(inputHandler),
    mMoveGenerator(), // Se crea el objeto MoveGenerator
    mCurrentPlayer(PlayerColor::PLAYER_1),
    mIsGameOver(false) {
}

void GameManager::StartNewGame() {
    mGameBoard.InitializeBoard();
    mCurrentPlayer = PlayerColor::PLAYER_1;
    mIsGameOver = false;
    mGameStats = GameStats{}; // Resetea estadísticas
    mView.ClearScreen();
    mView.DisplayMessage("=== NUEVO JUEGO DE DAMAS INICIADO ===", true);
    mView.DisplayMessage("Integrando validador de movimiento y captura simple...", true);
    mView.DisplayMessage("--------------------------------------------------", true);
}

void GameManager::RunGameLoop() {
    StartNewGame();

    while (!mIsGameOver) {
        mView.DisplayBoard(mGameBoard);
        // mView.DisplayStats(mGameStats, mGameBoard); // Se podría añadir después

        ProcessPlayerTurn();

        if (mIsGameOver) {
            break;
        }
        SwitchPlayer();
        if (!mIsGameOver) {
            mView.DisplayMessage("--------------------------------------------------", true);
        }
    }

    AnnounceResult(); // Mostrar el resultado final
}

void GameManager::ProcessPlayerTurn() {
    mView.DisplayMessage("Turno de " + PlayerColorToString(mCurrentPlayer), true);

    if (!mMoveGenerator.HasAnyValidMoves(mGameBoard, mCurrentPlayer)) {
        mView.DisplayMessage("El jugador " + PlayerColorToString(mCurrentPlayer) + " no tiene movimientos validos.", true);
        // Determinar ganador: si el jugador actual no puede mover, el otro gana.
        PlayerColor winner = (mCurrentPlayer == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
        mView.DisplayMessage("¡GANA " + PlayerColorToString(winner) + "!", true);
        mIsGameOver = true;
        return;
    }

    bool validMoveMade = false;
    while (!validMoveMade && !mIsGameOver) {
        MoveInput userInput = mInputHandler.GetPlayerMoveInput(mCurrentPlayer);

        if (userInput.wantsToExit) {
            mView.DisplayMessage("Juego terminado por el jugador " + PlayerColorToString(mCurrentPlayer) + ".", true);
            mIsGameOver = true;
            validMoveMade = true;
            continue;
        }

        if (!userInput.isValidFormat) {
            mView.DisplayMessage("Formato de entrada incorrecto. Use: filaOrigen colOrigen filaDestino colDestino (ej: 2 0 3 1)", true);
        }
        else {
            bool wasThisCapture = false; // Variable para el parámetro de salida de IsValidMove
            if (mMoveGenerator.IsValidMove(mGameBoard,        // <--- LLAMADA CORREGIDA
                userInput.startRow, userInput.startCol,
                userInput.endRow, userInput.endCol,
                mCurrentPlayer,
                wasThisCapture)) {  // <--- PASA LA VARIABLE wasThisCapture

                PieceType pieceToMove = mGameBoard.GetPieceAt(userInput.startRow, userInput.startCol);

                // Realizar el movimiento
                mGameBoard.SetPieceAt(userInput.endRow, userInput.endCol, pieceToMove);
                mGameBoard.SetPieceAt(userInput.startRow, userInput.startCol, PieceType::EMPTY);

                if (wasThisCapture) {
                    // Eliminar la pieza capturada del tablero
                    int capturedRow = userInput.startRow + (userInput.endRow - userInput.startRow) / 2;
                    int capturedCol = userInput.startCol + (userInput.endCol - userInput.startCol) / 2;
                    mGameBoard.SetPieceAt(capturedRow, capturedCol, PieceType::EMPTY);

                    mView.DisplayMessage("¡Captura realizada!", true);
                    // Actualizar estadísticas de captura
                    if (mCurrentPlayer == PlayerColor::PLAYER_1) mGameStats.player1CapturedCount++;
                    else mGameStats.player2CapturedCount++;
                }
                else {
                    mView.DisplayMessage("Movimiento realizado.", true);
                }

                // Chequear promoción después de que la pieza se haya movido y las capturas se hayan resuelto
                mGameBoard.PromotePieceIfNecessary(userInput.endRow, userInput.endCol);

                mGameStats.currentTurnNumber++; // Incrementar contador de movimientos/turnos completados
                validMoveMade = true;
            }
            else {
                mView.DisplayMessage("Movimiento invalido. Intente de nuevo.", true);
            }
        }

        if (!validMoveMade && !mIsGameOver) {
            std::cout << "Presione Enter para reintentar...";
#if defined(max)
#undef max
#endif
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            mView.DisplayBoard(mGameBoard);
            mView.DisplayMessage("Turno de " + PlayerColorToString(mCurrentPlayer), true);
        }
    }
}

void GameManager::SwitchPlayer() {
    mCurrentPlayer = (mCurrentPlayer == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
}

void GameManager::AnnounceResult() {
    // Esta función se llamará solo cuando mIsGameOver sea true.
    // La lógica para determinar el ganador (si no fue por 'salir')
    // ya habrá ocurrido en ProcessPlayerTurn si un jugador se queda sin movimientos.
    mView.DisplayBoard(mGameBoard);
    mView.DisplayMessage("", true);

    // Si salieron del juego, mIsGameOver es true, pero no hay un "ganador" por reglas de juego.
    // Necesitamos una mejor forma de saber si el juego terminó por "salir" o por condiciones de juego.
    // Por ahora, si mIsGameOver es true, y no hemos asignado un ganador explícito en ProcessPlayerTurn,
    // asumimos que es un "juego terminado" sin un ganador claro por reglas (podría ser por 'salir').

    // Lógica de ganador mejorada:
    PlayerColor winner = PlayerColor::NONE; // Asumir empate o salida
    if (!mMoveGenerator.HasAnyValidMoves(mGameBoard, PlayerColor::PLAYER_1) && mGameBoard.GetPieceCount(PlayerColor::PLAYER_1) > 0) {
        winner = PlayerColor::PLAYER_2; // P1 no puede mover, P2 gana
    }
    else if (!mMoveGenerator.HasAnyValidMoves(mGameBoard, PlayerColor::PLAYER_2) && mGameBoard.GetPieceCount(PlayerColor::PLAYER_2) > 0) {
        winner = PlayerColor::PLAYER_1; // P2 no puede mover, P1 gana
    }
    else if (mGameBoard.GetPieceCount(PlayerColor::PLAYER_1) == 0) {
        winner = PlayerColor::PLAYER_2; // P1 sin piezas, P2 gana
    }
    else if (mGameBoard.GetPieceCount(PlayerColor::PLAYER_2) == 0) {
        winner = PlayerColor::PLAYER_1; // P2 sin piezas, P1 gana
    }
    // Si 'wantsToExit' fue la causa de mIsGameOver, winner seguirá siendo NONE aquí.

    if (winner != PlayerColor::NONE) {
        mView.DisplayMessage("JUEGO TERMINADO! Ganador: " + PlayerColorToString(winner), true);
    }
    else {
        mView.DisplayMessage("Juego terminado.", true); // Podría ser por 'salir'
    }

    mView.DisplayMessage("Estadisticas: Turnos Jugados: " + std::to_string(mGameStats.currentTurnNumber - 1), true);
    mView.DisplayMessage("P1 Capturas: " + std::to_string(mGameStats.player1CapturedCount) +
        " | P2 Capturas: " + std::to_string(mGameStats.player2CapturedCount), true);


    mView.DisplayMessage("Presione Enter para volver al menu (simulado)...", false);
#if defined(max)
#undef max
#endif
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}