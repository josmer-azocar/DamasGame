#pragma once
#ifndef BOARD_H // Guardia de Inclusión - Inicio
#define BOARD_H // (Asegúrate de que estas líneas estén o añádelas si no)

// --- Includes Necesarios ---
#include "CommonTypes.h" // MUY IMPORTANTE: Para usar PieceType, PlayerColor, etc.
                         // Asume que CommonTypes.h está en la misma carpeta Headers.
#include <array>         // Para usar std::array para el tablero (como sugiere el PDF)
#include <vector>        // Podría ser necesario para devolver listas de movimientos después
#include <string>        // Para notaciones o mensajes futuros

// --- Declaración de la Clase Board ---
// (Class Board Declaration)
// Representa el tablero de juego de Damas 8x8.
class Board {
public: // Interfaz Pública (Public Interface)

    // Constante para el tamaño del tablero (Standard checkers board size)
    // Usamos constexpr para constantes conocidas en tiempo de compilación.
    static constexpr int BOARD_SIZE = 8;

    // --- Constructor ---
    // Se llama al crear un objeto Board.
    Board();

    // --- Métodos Principales (Core Methods) ---

    // Inicializa el tablero a la posición inicial estándar de Damas.
    // (Initializes the board to the standard starting checkers position)
    void InitializeBoard();

    // Obtiene el tipo de pieza en una coordenada específica.
    // (Gets the type of piece at a specific coordinate)
    // Parameters (Parámetros):
    // - row (fila): int, la fila (0-7).
    // - col (columna): int, la columna (0-7).
    // Returns (Devuelve): PieceType, el tipo de pieza (o EMPTY si está vacía/fuera de límites).
    // 'const' indica que este método no modifica el tablero.
    PieceType GetPieceAt(int row, int col) const;

    // Coloca una pieza de un tipo específico en una coordenada.
    // ¡IMPORTANTE! Este método también debe actualizar los contadores de piezas.
    // (Places a piece of a specific type at a coordinate)
    // Parameters (Parámetros):
    // - row (fila): int, la fila (0-7).
    // - col (columna): int, la columna (0-7).
    // - pieceType (tipoDeFicha): PieceType, el tipo de pieza a colocar.
    void SetPieceAt(int row, int col, PieceType pieceType);

    // --- Métodos Auxiliares y de Consulta (Utility and Query Methods) ---

    // Verifica si una coordenada (fila, columna) está dentro de los límites del tablero.
    // (Checks if a coordinate is within the board boundaries)
    // Returns (Devuelve): bool, true si está dentro, false si está fuera.
    bool IsWithinBounds(int row, int col) const;

    // Verifica si una casilla es jugable (según las reglas de Damas, usualmente las negras).
    // (Checks if a square is playable - usually the dark squares in Checkers)
    // ¡IMPORTANTE! Debemos ser consistentes con la convención (ej: ¿(0,0) es blanca o negra?).
    // Asumiremos (0,0) es BLANCA (NO jugable), por lo tanto, jugables son las de suma impar (r+c)%2 != 0.
    // Si quieres que (0,0) sea NEGRA (jugable), cambia la lógica a (r+c)%2 == 0.
    bool IsPlayableSquare(int row, int col) const;

    // Obtiene el número total de piezas (Peones + Damas) para un jugador.
    // (Gets the total piece count (Pawns + Kings) for a player)
    // Parameters (Parámetros):
    // - player (jugador): PlayerColor, el jugador (PLAYER_1 o PLAYER_2).
    // Returns (Devuelve): int, el número total de piezas.
    int GetPieceCount(PlayerColor player) const;

    // Obtiene el número de Damas (Reyes) para un jugador.
    // (Gets the King count for a player)
    // Parameters (Parámetros):
    // - player (jugador): PlayerColor, el jugador (PLAYER_1 o PLAYER_2).
    // Returns (Devuelve): int, el número de Damas.
    int GetKingCount(PlayerColor player) const;

    // Obtiene el tamaño del tablero (siempre 8 en este caso).
    // (Gets the size of the board - always 8 here)
    int GetBoardSize() const { return BOARD_SIZE; }

    void DisplayBoard() const;

private: // Miembros Privados (Private Members)

    // --- Datos Miembro (Member Data) ---

    // La cuadrícula del tablero. Usamos std::array porque el tamaño es fijo (8x8).
    // Es más eficiente que std::vector para tamaños fijos conocidos en compilación.
    // grid_[fila][columna]
    std::array<std::array<PieceType, BOARD_SIZE>, BOARD_SIZE> mGrid; // (cuadriculaMiembro)

    // Contadores de piezas. Mantenemos un registro separado para cada jugador y tipo.
    // pieceCounts_[índiceJugador][índiceTipo]
    // índiceJugador: 0 para PLAYER_1, 1 para PLAYER_2
    // índiceTipo: 0 para MAN (Peón), 1 para KING (Dama)
    // Accederíamos p.ej. como pieceCounts_[0][0] para P1_MAN.
    int mPieceCounts[2][2]; // (contadoresPiezasMiembro)

    // --- Métodos Auxiliares Privados (Private Helper Methods) ---

    // Reinicia todos los contadores de piezas a cero.
    // (Resets all piece counters to zero)
    void ResetPieceCounts();

    // Actualiza los contadores cuando una pieza cambia en SetPieceAt.
    // (Updates the counters when a piece changes in SetPieceAt)
    // Parameters (Parámetros):
    // - row, col: coordenadas donde ocurrió el cambio.
    // - oldPiece (piezaAntigua): El tipo de pieza que HABÍA ANTES.
    // - newPiece (piezaNueva): El tipo de pieza que HAY AHORA.
    void UpdateCountsForSetPiece(int r, int c, PieceType oldPiece, PieceType newPiece);

    // Limpia el tablero (pone todas las casillas a EMPTY). Útil antes de InitializeBoard.
    // (Clears the board, setting all squares to EMPTY)
    void ClearBoard();
};

#endif // BOARD_H // Guardia de Inclusión - Fin
