#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "CommonTypes.h"
#include "Board.h"
#include "MoveGenerator.h"

class ConsoleView;
class InputHandler;

enum class GameMode { // NUEVO
    NONE,
    PLAYER_VS_PLAYER,
    PLAYER_VS_COMPUTER,
    COMPUTER_VS_COMPUTER
};

class GameManager {
public:
    GameManager(Board& board, ConsoleView& view, InputHandler& inputHandler);

    void InitializeApplication(); // NUEVO: Mostrará el menú y obtendrá el modo
    void StartNewGame(GameMode mode); // Modificado para aceptar modo
    void RunGameLoop(); // Se llamará después de seleccionar un modo que inicie un juego

private:
    Board& mGameBoard;
    ConsoleView& mView;
    InputHandler& mInputHandler;
    MoveGenerator mMoveGenerator;

    PlayerColor mCurrentPlayer;
    bool mIsGameOver;
    GameStats mGameStats;
    Move mLastMove;

    bool mInCaptureSequence;
    int mForcedPieceRow;
    int mForcedPieceCol;

    GameMode mCurrentGameMode; // NUEVO

    void ShowMainMenu(); // NUEVO: Bucle para manejar el menú principal
    void ProcessPlayerTurn();
    void SwitchPlayer();
    void AnnounceResult();
    void DisplayCurrentStats();
    void DisplayLastMove();
    void ShowGlobalStats(); // NUEVO (placeholder)
};

#endif // GAME_MANAGER_H