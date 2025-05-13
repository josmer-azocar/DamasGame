#pragma once
#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <sstream> // Necesario para std::ostringstream

enum class PlayerColor {
    PLAYER_1, // Negras, piezas 'b', 'B'
    PLAYER_2, // Blancas, piezas 'w', 'W'
    NONE
};

enum class PieceType {
    P1_MAN, P1_KING,
    P2_MAN, P2_KING,
    EMPTY
};

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
    char rowChar = ('8' - r);
    std::string s = "";
    s += colChar;
    s += rowChar;
    return s;
}

// Declaración de PlayerColorToString ANTES de Move para que ToNotation la pueda usar
inline std::string PlayerColorToString(PlayerColor color);


struct Move {
    int startR_ = -1;
    int startC_ = -1;
    int endR_ = -1;
    int endC_ = -1;
    PieceType pieceMoved_ = PieceType::EMPTY;
    PlayerColor playerColor_ = PlayerColor::NONE; // MIEMBRO AÑADIDO
    bool isCapture_ = false;

    Move() = default;
    bool IsNull() const { return startR_ == -1; }

    // Versión corregida de ToNotation sin argumentos
    std::string ToNotation() const {
        if (IsNull()) return "N/A (Mov. Nulo)";
        std::ostringstream oss;
        // Asegurarse que PlayerColorToString esté declarada antes o incluir su definición aquí si es pequeña
        oss << PlayerColorToString(playerColor_) << " movio de "
            << ToAlgebraic(startR_, startC_) << " a " << ToAlgebraic(endR_, endC_);
        if (isCapture_) {
            oss << " (captura)";
        }
        return oss.str();
    }
};

struct GameStats {
    int player1CapturedCount = 0;
    int player2CapturedCount = 0;
    int currentTurnNumber = 1;
};

// Definición de PlayerColorToString
inline std::string PlayerColorToString(PlayerColor color) {
    switch (color) {
    case PlayerColor::PLAYER_1: return "Negras (b)";
    case PlayerColor::PLAYER_2: return "Blancas (w)";
    case PlayerColor::NONE:     return "NADIE";
    default:                    return "DESCONOCIDO";
    }
}

#endif // COMMON_TYPES_H