#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "CommonTypes.h"
#include "Board.h"
#include "MoveGenerator.h"
#include <memory> // Para std::unique_ptr

// Declaraciones anticipadas
class ConsoleView;
class InputHandler; // Seguirá siendo necesario para el menú
class Player;       // Nueva clase base de jugador

enum class GameMode {
    NONE,
    PLAYER_VS_PLAYER,
    PLAYER_VS_COMPUTER,
    COMPUTER_VS_COMPUTER
};

class GameManager {
public:
    // InputHandler se sigue pasando para el menú, pero HumanPlayer lo usará para el juego.
    GameManager(Board& board, ConsoleView& view, InputHandler& inputHandler);
    ~GameManager(); // Necesario para std::unique_ptr a tipos incompletos si se declaran aquí

    void InitializeApplication();
    // StartNewGame ya no necesita GameMode aquí, se establece antes en ShowMainMenu
    // y se usa para crear los Player apropiados.
    void StartNewGame();
    void RunGameLoop();

private:
    Board& mGameBoard;
    ConsoleView& mView;
    InputHandler& mInputHandler; // Usado por el menú y por HumanPlayer
    MoveGenerator mMoveGenerator;

    PlayerColor mCurrentPlayerTurnColor; // Color del jugador cuyo turno es
    bool mIsGameOver;
    GameStats mGameStats;
    Move mLastMove;

    bool mInCaptureSequence;
    int mForcedPieceRow;
    int mForcedPieceCol;

    GameMode mCurrentGameMode;

    // Punteros a los jugadores actuales. Usamos unique_ptr para gestión automática de memoria.
    std::unique_ptr<Player> m_player1;
    std::unique_ptr<Player> m_player2;
    Player* m_currentPlayerObject; // Puntero al objeto Player del turno actual (m_player1 o m_player2)

    void ShowMainMenu();
    void ProcessPlayerTurn();
    void SwitchPlayer();
    void AnnounceResult();
    void DisplayCurrentStats();
    void DisplayLastMove();
    void ShowGlobalStats();
};

#endif // GAME_MANAGER_H_MANAGER_H