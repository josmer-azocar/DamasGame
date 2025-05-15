#ifndef COMMON_TYPES_H // Guarda de inclusión INICIO
#define COMMON_TYPES_H

#pragma once // Directiva común, pero las guardas son más estándar

#include <string>
#include <vector>
#include <sstream> // Para std::ostringstream en Move::ToNotation
#include <map>     // Si usaras std::map en algún momento
#include <utility> // Si usaras std::pair

// Definición de PlayerColor
enum class PlayerColor {
    PLAYER_1, // Blancas (w)
    PLAYER_2, // Negras (b)
    NONE
};

// Definición de PieceType
enum class PieceType {
    P1_MAN, P1_KING, // Piezas del Jugador 1 (Blancas)
    P2_MAN, P2_KING, // Piezas del Jugador 2 (Negras)
    EMPTY
};

// Estructura para la entrada de movimiento del jugador
struct MoveInput {
    int startRow = -1;
    int startCol = -1;
    int endRow = -1;
    int endCol = -1;
    bool isValidFormat = false;
    bool wantsToExit = false;
    bool wantsToShowStats = false;
};

// Función para convertir coordenadas internas (0-7) a notación A1-H8
inline std::string ToAlgebraic(int r, int c) {
    if (r < 0 || r > 7 || c < 0 || c > 7) {
        return "??";
    }
    char colChar = 'A' + c;
    char rowChar = ('8' - r); // Si la fila 0 del array es la 8 del tablero
    std::string s = "";
    s += colChar;
    s += rowChar;
    return s;
}

// Función para convertir PlayerColor a string
inline std::string PlayerColorToString(PlayerColor color) {
    switch (color) {
    case PlayerColor::PLAYER_1: return "Blancas (w)";
    case PlayerColor::PLAYER_2: return "Negras (b)";
    case PlayerColor::NONE:     return "NADIE";
    default:                    return "DESCONOCIDO";
    }
}

// Estructura para representar un movimiento
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

    std::string ToNotation() const {
        if (IsNull()) return "N/A (Mov. Nulo)";
        std::ostringstream oss;
        oss << PlayerColorToString(playerColor_) << " movio de "
            << ToAlgebraic(startR_, startC_) << " a " << ToAlgebraic(endR_, endC_);
        if (isCapture_) {
            oss << " (captura)";
        }
        return oss.str();
    }
};

// Definición de GameOverReason (DENTRO de las guardas)
enum class GameOverReason {
    NONE,
    NO_PIECES,
    NO_MOVES,
    PLAYER_EXIT,
    STALEMATE_BY_RULES
};

// Definición de GameStats (DENTRO de las guardas)
struct GameStats {
    int player1CapturedCount = 0;
    int player2CapturedCount = 0;
    int currentTurnNumber = 1;
    PlayerColor winner = PlayerColor::NONE;
    GameOverReason reason = GameOverReason::NONE;
};

#endif // COMMON_TYPES_H (Guarda de inclusión FIN)