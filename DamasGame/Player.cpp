// Implementacion de la clase base Player para el juego de Damas
#include "Player.h"

// Constructor de Player
// color: color asignado al jugador (blanco o negro)
Player::Player(PlayerColor color)
    : m_color(color) {
}

// Retorna el color del jugador
PlayerColor Player::GetColor() const {
    return m_color;
}