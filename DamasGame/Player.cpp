#include "Player.h"

Player::Player(PlayerColor color)
    : m_color(color) {
}

PlayerColor Player::GetColor() const {
    return m_color;
}