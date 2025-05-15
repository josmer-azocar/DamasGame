#ifndef PLAYER_H
#define PLAYER_H

#include "CommonTypes.h" 
#include "Board.h"       
#include "MoveGenerator.h" 

// Clase base abstracta para representar un jugador (humano o computadora).
class Player {
public:
    Player(PlayerColor color);
    virtual ~Player() = default;

    PlayerColor GetColor() const;

    // Obtiene la intención de movimiento del jugador.
    virtual MoveInput GetChosenMoveInput(
        const Board& board,
        const MoveGenerator& moveGenerator,
        bool isInCaptureSequence,
        int forcedRow,
        int forcedCol,
        const std::vector<Move>& availableMandatoryJumps
    ) = 0;

protected:
    PlayerColor m_color; // Color de las piezas de este jugador.
};

#endif // PLAYER_H