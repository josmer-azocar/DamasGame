#pragma once
#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <string>
#include <vector>
#include <map>     // Para el ToNotation de Move (aunque sea placeholder)
#include <utility> // Para std::pair (si se usa en Move, por ejemplo)

enum class PlayerColor {
    PLAYER_1, // Negras/Rojas
    PLAYER_2, // Blancas/Azules
    NONE
};

enum class PieceType {
    P1_MAN, P1_KING,
    P2_MAN, P2_KING,
    EMPTY
};

// Estructura para la entrada del movimiento del jugador
// Contiene las coordenadas y banderas de estado.
struct MoveInput {
    int startRow = -1;
    int startCol = -1;
    int endRow = -1;
    int endCol = -1;
    bool isValidFormat = false;
    bool wantsToExit = false;
};

// Estructura para representar un movimiento en el juego
struct Move {
    int startR_ = -1;
    int startC_ = -1;
    int endR_ = -1;
    int endC_ = -1;
    PieceType pieceMoved_ = PieceType::EMPTY;
    bool isCapture_ = false;
    // std::vector<std::pair<int, int>> capturedPiecesCoords_; // Para capturas múltiples
    // bool causesPromotion_ = false;

    Move() = default;

    bool IsNull() const { return startR_ == -1; }

    // Placeholder, se mejorará cuando ConsoleView tenga mapas de coordenadas
    std::string ToNotation(const std::map<std::pair<int, int>, int>& /*coordsToNumMap = {}*/) const {
        if (IsNull()) return "N/A (Mov. Nulo)";
        return "De (" + std::to_string(startR_) + "," + std::to_string(startC_) + ") a (" +
            std::to_string(endR_) + "," + std::to_string(endC_) + ")";
    }
};

struct GameStats {
    int player1CapturedCount = 0;
    int player2CapturedCount = 0;
    int currentTurnNumber = 1;
};

// ... (GameResultInfo si la necesitas definida aquí) ...

inline std::string PlayerColorToString(PlayerColor color) {
    switch (color) {
    case PlayerColor::PLAYER_1: return "Negras (o)";
    case PlayerColor::PLAYER_2: return "Blancas (x)";
    case PlayerColor::NONE:     return "NADIE";
    default:                    return "DESCONOCIDO";
    }
}

#endif // COMMON_TYPES_H