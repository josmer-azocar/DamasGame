// -----------------------------------------------------------------------------
// GameManager.h
// Declaraci�n de la clase GameManager, responsable de orquestar el juego.
// -----------------------------------------------------------------------------

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "CommonTypes.h" 
#include "Board.h"       
#include "MoveGenerator.h" 

// Declaraciones anticipadas
class ConsoleView;
class InputHandler;

class GameManager {
public:
    GameManager(Board& board, ConsoleView& view, InputHandler& inputHandler);
    void StartNewGame();
    void RunGameLoop();

private:
    Board& mGameBoard;
    ConsoleView& mView;
    InputHandler& mInputHandler;
    MoveGenerator mMoveGenerator;

    PlayerColor mCurrentPlayer;
    bool mIsGameOver;
    GameStats mGameStats;
    Move mLastMove; // <--- Declaración añadida

    void ProcessPlayerTurn();
    void SwitchPlayer();
    void AnnounceResult();
    void DisplayCurrentStats(); // <--- Declaración añadida
    void DisplayLastMove();     // <--- Declaración añadida
};

#endif // GAME_MANAGER_H