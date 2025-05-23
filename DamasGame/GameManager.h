#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "LocalizationManager.h"
#include "ConsoleView.h"
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


// Enumeracion para los diferentes modos de juego disponibles
// NONE: sin modo seleccionado
// PLAYER_VS_PLAYER: dos jugadores humanos
// PLAYER_VS_COMPUTER: humano contra IA
// COMPUTER_VS_COMPUTER: IA contra IA
enum class GameMode {
	NONE,
	PLAYER_VS_PLAYER,
	PLAYER_VS_COMPUTER,
	COMPUTER_VS_COMPUTER
};

class GameManager {
public:
	// Constructor de GameManager
	// Recibe una referencia al tablero y al manejador de entrada
	GameManager(Board& board, InputHandler& inputHandler);
	
	// Destructor de GameManager
	~GameManager();

	// Inicializa la aplicacion, carga configuraciones y recursos
	void InitializeApplication();
	// Inicia una nueva partida, resetea el estado del juego
	void StartNewGame();
	// Ejecuta el bucle principal del juego hasta que termine
	void RunGameLoop();

private:
	LocalizationManager m_i18n; // Maneja la localizacion y traduccion de textos
	ConsoleView mView; // Vista para mostrar informacion en consola
	Board& mGameBoard; // Referencia al tablero de juego
	//ConsoleView& mView;
	InputHandler& mInputHandler; // Referencia al manejador de entrada del usuario
	MoveGenerator mMoveGenerator; // Generador de movimientos validos
	FileHandler mFileHandler; // Objeto para manejar archivos de resultados

	PlayerColor mCurrentPlayerTurnColor; // Color del jugador que tiene el turno actual
	bool mIsGameOver; // Indica si la partida ha terminado
	GameStats mGameStats; // Estadisticas de la partida actual
	Move mLastMove; // Ultimo movimiento realizado

	bool mInCaptureSequence; // Indica si se esta en una secuencia de capturas obligatorias
	int mForcedPieceRow; // Fila de la pieza obligada a capturar
	int mForcedPieceCol; // Columna de la pieza obligada a capturar

	GameMode mCurrentGameMode; // Modo de juego actual

	std::unique_ptr<Player> m_player1; // Puntero al jugador 1 (puede ser humano o IA)
	std::unique_ptr<Player> m_player2; // Puntero al jugador 2 (puede ser humano o IA)
	Player* m_currentPlayerObject; // Puntero al jugador que tiene el turno

	// Metodos privados auxiliares
	// Muestra el menu principal y gestiona la seleccion del usuario
	void ShowMainMenu();
	// Procesa el turno del jugador actual
	void ProcessPlayerTurn();
	// Cambia el turno al otro jugador
	void SwitchPlayer();
	// Anuncia el resultado de la partida y guarda los datos
	void AnnounceResult();      // Aqui se prepararan los datos para GameResult y se guardaran
	// Muestra las estadisticas actuales de la partida
	void DisplayCurrentStats();
	// Muestra el ultimo movimiento realizado
	void DisplayLastMove();
	// Muestra las estadisticas globales de partidas anteriores
	void ShowGlobalStats();     // Usara mFileHandler.displayGameHistory()

	// Funcion auxiliar para obtener la fecha y hora actual en un formato especifico
	std::string getCurrentDateTime(const std::string& format);
};

#endif // GAME_MANAGER_H