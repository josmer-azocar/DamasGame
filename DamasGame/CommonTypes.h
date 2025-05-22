#ifndef COMMON_TYPES_H 
#define COMMON_TYPES_H

#pragma once 

#include <string>
#include <vector>
#include <sstream> 
#include <map>     
#include <utility> 

#include "LocalizationManager.h" 

// Definición de PlayerColor
enum class PlayerColor {
	PLAYER_1, // Blancas (w)
	PLAYER_2, // Negras (b)
	NONE
};

// Definición de PieceType
enum class PieceType {
	P1_MAN, P1_KING,
	P2_MAN, P2_KING,
	EMPTY
};

// Estructura MoveInput
struct MoveInput {
	int startRow = -1;
	int startCol = -1;
	int endRow = -1;
	int endCol = -1;
	bool isValidFormat = false;
	bool wantsToExit = false;
	bool wantsToShowStats = false;
};

// ToAlgebraic no necesita localización
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

// --- PlayerColorToString IMPLEMENTACIÓN INLINE ---
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
struct Move {
	int startR_ = -1;
	int startC_ = -1;
	int endR_ = -1;
	int endC_ = -1;
	PieceType pieceMoved_ = PieceType::EMPTY;
	PlayerColor playerColor_ = PlayerColor::NONE;
	bool isCapture_ = false;

	Move() = default;
	bool IsNull() const { return startR_ == -1; }

	//  ToNotation IMPLEMENTACIÓN INLINE 
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

// Definición de GameOverReason
enum class GameOverReason {
	NONE,
	NO_PIECES,
	NO_MOVES,
	PLAYER_EXIT,
	STALEMATE_BY_RULES
};

// Definición de GameStats
struct GameStats {
	int player1CapturedCount = 0;
	int player2CapturedCount = 0;
	int currentTurnNumber = 1;
	PlayerColor winner = PlayerColor::NONE;
	GameOverReason reason = GameOverReason::NONE;
};

#endif // COMMON_TYPES_H