#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "CommonTypes.h"
#include "Board.h"
#include "MoveGenerator.h"
#include "FileHandler.h" // Ya estaba, necesario para mFileHandler
#include <memory>        // Para std::unique_ptr
#include <string>        // Para std::string

// Declaraciones anticipadas
class ConsoleView;
class InputHandler;
class Player;

enum class GameMode {
	NONE,
	PLAYER_VS_PLAYER,
	PLAYER_VS_COMPUTER,
	COMPUTER_VS_COMPUTER
};

class GameManager {
public:
	GameManager(Board& board, ConsoleView& view, InputHandler& inputHandler);
	~GameManager();

	void InitializeApplication();
	void StartNewGame();
	void RunGameLoop();

private:
	Board& mGameBoard;
	ConsoleView& mView;
	InputHandler& mInputHandler;
	MoveGenerator mMoveGenerator;
	FileHandler mFileHandler; // Objeto para manejar archivos de resultados

	PlayerColor mCurrentPlayerTurnColor;
	bool mIsGameOver;
	GameStats mGameStats;
	Move mLastMove;

	bool mInCaptureSequence;
	int mForcedPieceRow;
	int mForcedPieceCol;

	GameMode mCurrentGameMode;

	std::unique_ptr<Player> m_player1;
	std::unique_ptr<Player> m_player2;
	Player* m_currentPlayerObject;

	// Métodos privados
	void ShowMainMenu();
	void ProcessPlayerTurn();
	void SwitchPlayer();
	void AnnounceResult();      // Aquí se prepararán los datos para GameResult y se guardarán
	void DisplayCurrentStats();
	void DisplayLastMove();
	void ShowGlobalStats();     // Usará mFileHandler.displayGameHistory()

	// Helper para obtener fecha y hora actual
	std::string getCurrentDateTime(const std::string& format);
};

#endif // GAME_MANAGER_H