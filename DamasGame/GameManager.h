// -----------------------------------------------------------------------------
// GameManager.h
// Declaración de la clase GameManager, responsable de orquestar el juego.
// -----------------------------------------------------------------------------

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "CommonTypes.h" // Para PlayerColor, MoveInput, GameStats, etc.
#include "Board.h"       // GameManager tendrá un Board
// class MoveGenerator; // Comentado o borrado si incluimos el .h
#include "MoveGenerator.h" 

// Declaraciones anticipadas para evitar includes circulares
class ConsoleView;
class InputHandler;
class MoveGenerator; // La necesitaremos pronto

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
    GameStats mGameStats; // Para las estadísticas que Miguel manejaba

    void ProcessPlayerTurn();
    void SwitchPlayer();
    bool CheckEndGameConditions(); // Para verificar si alguien ganó o no hay movimientos
    void AnnounceResult();         // Para mostrar el resultado final
};

#endif // GAME_MANAGER_H