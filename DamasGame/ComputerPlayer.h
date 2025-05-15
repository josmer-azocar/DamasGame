#ifndef COMPUTER_PLAYER_H
#define COMPUTER_PLAYER_H

#include "Player.h"
#include "MoveGenerator.h" // Para pasar como dependencia y usar sus utilidades
#include <random>          // Para std::mt19937 si se quiere aleatoriedad en desempates
#include <vector>          // Para std::vector
#include <limits>          // Para std::numeric_limits

// Clase que representa a un jugador controlado por la computadora (IA).
class ComputerPlayer : public Player {
public:
    // Constructor: toma el color, una referencia al MoveGenerator y una dificultad opcional.
    ComputerPlayer(PlayerColor color, const MoveGenerator& moveGenerator, int difficulty = 1);

    // Implementación del método para que la IA decida su movimiento.
    MoveInput GetChosenMoveInput(
        const Board& board,
        const MoveGenerator& moveGenerator, // Ya es miembro, pero la firma de Player lo requiere
        bool isInCaptureSequence,
        int forcedRow,
        int forcedCol,
        const std::vector<Move>& availableMandatoryJumps
    ) override;

private:
    int m_difficulty;                   // Nivel de dificultad de la IA.
    const MoveGenerator& m_moveGeneratorRef; // Referencia al generador de movimientos.

    // Para desempates o IA más aleatoria (opcional)
    std::mt19937 m_rng;

    // Función de evaluación heurística para puntuar un estado del tablero.
    int EvaluateBoardState(const Board& currentBoard, PlayerColor perspectiveColor) const;
};

#endif // COMPUTER_PLAYER_H