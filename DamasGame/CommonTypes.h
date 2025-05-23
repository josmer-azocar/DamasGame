#ifndef COMMON_TYPES_H 
#define COMMON_TYPES_H

#pragma once 

#include <string>
#include <vector>
#include <sstream> 
#include <map>     
#include <utility> 

#include "LocalizationManager.h" 

// Definicion de PlayerColor
// Enumera los colores disponibles para los jugadores
enum class PlayerColor {
	PLAYER_1, // Blancas (w)
	PLAYER_2, // Negras (b)
	NONE
};

// Definicion de PieceType
// Enumera los tipos de piezas y sus respectivas versiones (normal y rey)
enum class PieceType {
	P1_MAN, P1_KING,
	P2_MAN, P2_KING,
	EMPTY
};

// Estructura MoveInput
// Estructura para manejar la entrada del movimiento del jugador
struct MoveInput {
	int startRow = -1; // Fila de inicio
	int startCol = -1; // Columna de inicio
	int endRow = -1;   // Fila de destino
	int endCol = -1;   // Columna de destino
	bool isValidFormat = false; // Indica si el formato de movimiento es valido
	bool wantsToExit = false;   // Indica si el jugador quiere salir
	bool wantsToShowStats = false; // Indica si el jugador quiere ver las estadisticas
};

// ToAlgebraic no necesita localizacion
// Convierte coordenadas de fila y columna a notacion algebraica
inline std::string ToAlgebraic(int r, int c) {
	if (r < 0 || r > 7 || c < 0 || c > 7) {
		return "??";
	}
	char colChar = 'A' + c;
	char rowChar = ('8' - r);
	std::string s = "";
	s += colChar;
	s += rowChar;
	return s;
}

// --- PlayerColorToString IMPLEMENTACION INLINE ---
// Convierte un color de jugador a una cadena localizada
inline std::string PlayerColorToString(PlayerColor color, const LocalizationManager& i18n) {
	std::string key;
	switch (color) {
	case PlayerColor::PLAYER_1: key = "player_color_white"; break;
	case PlayerColor::PLAYER_2: key = "player_color_black"; break;
	case PlayerColor::NONE:     key = "player_color_none"; break;
	default:                    key = "player_color_unknown"; break;
	}
	return i18n.GetString(key); // Llama a LocalizationManager
}

// Estructura Move
// Representa un movimiento en el juego, incluyendo la posicion inicial y final, la pieza movida, y el color del jugador
struct Move {
	int startR_ = -1; // Fila de inicio
	int startC_ = -1; // Columna de inicio
	int endR_ = -1;   // Fila de destino
	int endC_ = -1;   // Columna de destino
	PieceType pieceMoved_ = PieceType::EMPTY; // Tipo de pieza que se mueve
	PlayerColor playerColor_ = PlayerColor::NONE; // Color del jugador que realiza el movimiento
	bool isCapture_ = false; // Indica si el movimiento es una captura

	Move() = default;
	bool IsNull() const { return startR_ == -1; }

	//  ToNotation IMPLEMENTACION INLINE 
	// Convierte el movimiento a una notacion legible por humanos
	inline std::string ToNotation(const LocalizationManager& i18n) const {
		if (IsNull()) return i18n.GetString("move_notation_null");

		std::ostringstream oss;
		std::map<std::string, std::string> replacements;

		replacements["player_color"] = PlayerColorToString(playerColor_, i18n);
		replacements["start_pos"] = ToAlgebraic(startR_, startC_);
		replacements["end_pos"] = ToAlgebraic(endR_, endC_);

		oss << i18n.GetString("move_notation_format", replacements);

		if (isCapture_) {
			oss << " " << i18n.GetString("move_notation_capture_suffix");
		}
		return oss.str();
	}
};

// Definicion de GameOverReason
// Enumera las posibles razones por las que puede terminar una partida
enum class GameOverReason {
	NONE,               // No ha terminado
	NO_PIECES,          // Un jugador se quedo sin piezas
	NO_MOVES,           // Un jugador no tiene movimientos validos
	PLAYER_EXIT,        // Un jugador decidio salir
	STALEMATE_BY_RULES  // Empate por reglas
};

// Definicion de GameStats
// Estructura que almacena estadisticas de la partida en curso
struct GameStats {
	int player1CapturedCount = 0;    // Cantidad de piezas capturadas por el jugador 1
	int player2CapturedCount = 0;    // Cantidad de piezas capturadas por el jugador 2
	int currentTurnNumber = 1;       // Numero de turno actual
	PlayerColor winner = PlayerColor::NONE; // Color del ganador (si lo hay)
	GameOverReason reason = GameOverReason::NONE; // Razon por la que termino la partida
};

#endif // COMMON_TYPES_H