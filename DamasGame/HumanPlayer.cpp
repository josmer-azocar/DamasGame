#include "HumanPlayer.h"
#include "CommonTypes.h" // Para PlayerColorToString, ToAlgebraic
#include <iostream>      // Para std::cout si se usara (trataremos de usar m_view)

HumanPlayer::HumanPlayer(PlayerColor color, InputHandler& inputHandler, const ConsoleView& view)
    : Player(color), m_inputHandler(inputHandler), m_view(view) {
}

MoveInput HumanPlayer::GetChosenMoveInput(
    const Board& board,
    const MoveGenerator& moveGenerator, // No usado directamente por HumanPlayer básico para obtener input
    bool isInCaptureSequence,
    int forcedRow,
    int forcedCol,
    const std::vector<Move>& availableMandatoryJumps // Podría usarse para mostrar ayudas
) {
    // GameManager ya habrá mostrado el mensaje de "Turno de..." o "Continuar captura..."
    // Esta función ahora solo llama a InputHandler para obtener el string de entrada.

    // Ejemplo de cómo se podría usar availableMandatoryJumps si quisiéramos que HumanPlayer
    // diera un feedback más directo ANTES de que GameManager valide el movimiento.
    // Por ahora, esta lógica de feedback detallado está en GameManager.
    /*
    if (!availableMandatoryJumps.empty() && !isInCaptureSequence) {
        m_view.DisplayMessage("AVISO: Tienes capturas obligatorias disponibles:", true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
        for (const auto& jump : availableMandatoryJumps) {
            m_view.DisplayMessage("  -> " + jump.ToNotation(), true, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_BLACK);
        }
    }
    */

    // InputHandler se encarga del prompt ">" y de leer la línea.
    MoveInput userInput = m_inputHandler.GetPlayerMoveInput(m_color);

    // Validaciones superficiales de formato ya las hace InputHandler.
    // Validaciones de reglas del juego (si el movimiento es legal, si es captura obligatoria, etc.)
    // las hará GameManager después de recibir este MoveInput.

    return userInput;
}