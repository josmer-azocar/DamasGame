#include "MoveGenerator.h"
#include "Board.h"
#include "CommonTypes.h"

#include <iostream> // Para std::cout de depuración
#include <vector>
#include <algorithm> // Para std::any_of si fuera necesario (no usado aquí aún)

MoveGenerator::MoveGenerator() {}

PlayerColor MoveGenerator::GetPlayerFromPiece(PieceType piece) const {
    if (piece == PieceType::P1_MAN || piece == PieceType::P1_KING) return PlayerColor::PLAYER_1;
    if (piece == PieceType::P2_MAN || piece == PieceType::P2_KING) return PlayerColor::PLAYER_2;
    return PlayerColor::NONE;
}

// Los métodos FindSimplePawnMoves, FindPawnJumps, FindSimpleKingMoves, FindKingJumps
// ya estaban bien para ser reutilizados, solo nos aseguramos que tomen player y piece.
// No necesitan cambios respecto a tu última versión si ya tomaban player y piece.
// Por completitud, los incluyo (asumiendo que son los mismos que me pasaste).

void MoveGenerator::FindSimplePawnMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const {
    if (piece != PieceType::P1_MAN && piece != PieceType::P2_MAN) return;
    int forwardDirection = (player == PlayerColor::PLAYER_1) ? 1 : -1; // P1 (Negras) mueve hacia abajo (incrementa fila), P2 (Blancas) hacia arriba (decrementa fila)

    // Direcciones diagonales relativas
    int dCols[] = { -1, 1 };

    for (int dc : dCols) {
        int endRow = r + forwardDirection;
        int endCol = c + dc;
        if (gameBoard.IsWithinBounds(endRow, endCol) && gameBoard.GetPieceAt(endRow, endCol) == PieceType::EMPTY) {
            Move m;
            m.startR_ = r; m.startC_ = c; m.endR_ = endRow; m.endC_ = endCol;
            m.pieceMoved_ = piece; m.playerColor_ = player; m.isCapture_ = false;
            moves.push_back(m);
        }
    }
}

void MoveGenerator::FindPawnJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const {
    if (piece != PieceType::P1_MAN && piece != PieceType::P2_MAN) return;

    PlayerColor opponentColor = (player == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
    PieceType opponentPawn = (opponentColor == PlayerColor::PLAYER_1) ? PieceType::P1_MAN : PieceType::P2_MAN;
    PieceType opponentKing = (opponentColor == PlayerColor::PLAYER_1) ? PieceType::P1_KING : PieceType::P2_KING;

    // Los peones solo saltan hacia adelante.
    // La dirección de "adelante" depende del jugador.
    // P1 (Negras, asumiendo que empiezan arriba y mueven a filas mayores): forwardDirection = 1
    // P2 (Blancas, asumiendo que empiezan abajo y mueven a filas menores): forwardDirection = -1
    int forwardDirection = (player == PlayerColor::PLAYER_1) ? 1 : -1;

    // Desplazamientos para el salto: [fila_captura, col_captura, fila_aterrizaje, col_aterrizaje]
    // relativo a la posición actual (r,c)
    int jumpOffsets[][4] = {
        {forwardDirection, -1, 2 * forwardDirection, -2}, // Salto adelante-izquierda
        {forwardDirection,  1, 2 * forwardDirection,  2}  // Salto adelante-derecha
    };

    for (const auto& offset : jumpOffsets) {
        int capturedRow = r + offset[0];
        int capturedCol = c + offset[1];
        int landingRow = r + offset[2];
        int landingCol = c + offset[3];

        if (gameBoard.IsWithinBounds(landingRow, landingCol) &&
            gameBoard.GetPieceAt(landingRow, landingCol) == PieceType::EMPTY) {

            // Verificar que la casilla de captura esté dentro de los límites también
            if (gameBoard.IsWithinBounds(capturedRow, capturedCol)) {
                PieceType pieceAtCapturedPos = gameBoard.GetPieceAt(capturedRow, capturedCol);
                if (pieceAtCapturedPos == opponentPawn || pieceAtCapturedPos == opponentKing) {
                    Move m;
                    m.startR_ = r; m.startC_ = c; m.endR_ = landingRow; m.endC_ = landingCol;
                    m.pieceMoved_ = piece; m.playerColor_ = player; m.isCapture_ = true;
                    moves.push_back(m);
                }
            }
        }
    }
}


void MoveGenerator::FindSimpleKingMoves(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const {
    if (piece != PieceType::P1_KING && piece != PieceType::P2_KING) return;

    int dRows[] = { -1, -1, 1, 1 }; // Arriba-Izquierda, Arriba-Derecha, Abajo-Izquierda, Abajo-Derecha
    int dCols[] = { -1,  1, -1, 1 };

    for (int i = 0; i < 4; ++i) {
        int endRow = r + dRows[i];
        int endCol = c + dCols[i];
        if (gameBoard.IsWithinBounds(endRow, endCol) && gameBoard.GetPieceAt(endRow, endCol) == PieceType::EMPTY) {
            Move m;
            m.startR_ = r; m.startC_ = c; m.endR_ = endRow; m.endC_ = endCol;
            m.pieceMoved_ = piece; m.playerColor_ = player; m.isCapture_ = false;
            moves.push_back(m);
        }
    }
}

void MoveGenerator::FindKingJumps(const Board& gameBoard, int r, int c, PlayerColor player, PieceType piece, std::vector<Move>& moves) const {
    if (piece != PieceType::P1_KING && piece != PieceType::P2_KING) return;

    PlayerColor opponentColor = (player == PlayerColor::PLAYER_1) ? PlayerColor::PLAYER_2 : PlayerColor::PLAYER_1;
    PieceType opponentPawn = (opponentColor == PlayerColor::PLAYER_1) ? PieceType::P1_MAN : PieceType::P2_MAN;
    PieceType opponentKing = (opponentColor == PlayerColor::PLAYER_1) ? PieceType::P1_KING : PieceType::P2_KING;

    int dRows[] = { -1, -1,  1, 1 }; // Cambios en fila para pieza capturada
    int dCols[] = { -1,  1, -1, 1 }; // Cambios en columna para pieza capturada

    for (int i = 0; i < 4; ++i) { // 4 direcciones diagonales
        int capturedRow = r + dRows[i];
        int capturedCol = c + dCols[i];
        int landingRow = r + 2 * dRows[i];
        int landingCol = c + 2 * dCols[i];

        if (gameBoard.IsWithinBounds(landingRow, landingCol) &&
            gameBoard.GetPieceAt(landingRow, landingCol) == PieceType::EMPTY) {

            // Verificar que la casilla de captura esté dentro de los límites también
            if (gameBoard.IsWithinBounds(capturedRow, capturedCol)) {
                PieceType pieceAtCapturedPos = gameBoard.GetPieceAt(capturedRow, capturedCol);
                if (pieceAtCapturedPos == opponentPawn || pieceAtCapturedPos == opponentKing) {
                    Move m;
                    m.startR_ = r; m.startC_ = c; m.endR_ = landingRow; m.endC_ = landingCol;
                    m.pieceMoved_ = piece; m.playerColor_ = player; m.isCapture_ = true;
                    moves.push_back(m);
                }
            }
        }
    }
}

// --- IMPLEMENTACIÓN DE LA NUEVA FUNCIÓN ---
std::vector<Move> MoveGenerator::GetPossibleJumpsForSpecificPiece(const Board& gameBoard, int pieceRow, int pieceCol) const {
    std::vector<Move> jumps;
    PieceType piece = gameBoard.GetPieceAt(pieceRow, pieceCol);
    PlayerColor player = GetPlayerFromPiece(piece);

    if (player == PlayerColor::NONE || piece == PieceType::EMPTY) {
        return jumps; // Vacío si no hay pieza o no es de un jugador
    }

    if (piece == PieceType::P1_MAN || piece == PieceType::P2_MAN) {
        FindPawnJumps(gameBoard, pieceRow, pieceCol, player, piece, jumps);
    }
    else if (piece == PieceType::P1_KING || piece == PieceType::P2_KING) {
        FindKingJumps(gameBoard, pieceRow, pieceCol, player, piece, jumps);
    }
    return jumps;
}

std::vector<Move> MoveGenerator::GenerateMovesForPiece(const Board& gameBoard, int startRow, int startCol) const {
    std::vector<Move> allValidMoves;
    PieceType piece = gameBoard.GetPieceAt(startRow, startCol);
    PlayerColor player = GetPlayerFromPiece(piece);

    if (player == PlayerColor::NONE || piece == PieceType::EMPTY) {
        return allValidMoves;
    }

    // Primero, buscar saltos (capturas), ya que son obligatorios si existen.
    // Usamos la nueva función para asegurar que solo obtenemos saltos.
    allValidMoves = GetPossibleJumpsForSpecificPiece(gameBoard, startRow, startCol);

    // Si no hay saltos, entonces buscar movimientos simples.
    if (allValidMoves.empty()) {
        if (piece == PieceType::P1_MAN || piece == PieceType::P2_MAN) {
            FindSimplePawnMoves(gameBoard, startRow, startCol, player, piece, allValidMoves);
        }
        else if (piece == PieceType::P1_KING || piece == PieceType::P2_KING) {
            FindSimpleKingMoves(gameBoard, startRow, startCol, player, piece, allValidMoves);
        }
    }
    // NOTA: La regla de "Dama tiene prioridad para comer" aún no está implementada aquí.
    // Esta función actualmente devuelve *todos* los movimientos de *esta pieza*, dando prioridad a sus propios saltos.
    // La lógica de si el jugador *debe* usar esta pieza u otra (ej. una Dama en vez de un Peón para capturar)
    // se manejará en un nivel superior o necesitará una reestructuración más profunda aquí.
    // Por ahora, nos centramos en la captura múltiple de *una pieza dada*.
    return allValidMoves;
}

bool MoveGenerator::IsValidMove(const Board& gameBoard, int startRow, int startCol, int endRow, int endCol, PlayerColor player, bool& wasCapture) const {
    wasCapture = false;
    if (!gameBoard.IsWithinBounds(startRow, startCol) || !gameBoard.IsWithinBounds(endRow, endCol)) {
        // std::cout << "[Debug MG] IsValidMove: Fuera de limites." << std::endl;
        return false;
    }
    PieceType pieceAtStart = gameBoard.GetPieceAt(startRow, startCol);
    if (pieceAtStart == PieceType::EMPTY) {
        // std::cout << "[Debug MG] IsValidMove: Casilla origen vacia." << std::endl;
        return false;
    }

    PlayerColor pieceOwner = GetPlayerFromPiece(pieceAtStart);
    if (pieceOwner != player) {
        // std::cout << "[Debug MG] IsValidMove: Jugador no es dueño de la pieza." << std::endl;
        return false;
    }

    // REGLA DE CAPTURA OBLIGATORIA GENERAL:
    // Si hay CUALQUIER salto posible en el tablero para el jugador, debe realizar un salto.
    // Si el movimiento propuesto no es un salto, y hay saltos disponibles, el movimiento no es válido.
    std::vector<Move> allPossiblePlayerMoves;
    bool jumpAvailableAnywhere = false;
    for (int r = 0; r < Board::BOARD_SIZE; ++r) {
        for (int c = 0; c < Board::BOARD_SIZE; ++c) {
            if (GetPlayerFromPiece(gameBoard.GetPieceAt(r, c)) == player) {
                std::vector<Move> jumpsForThisPiece = GetPossibleJumpsForSpecificPiece(gameBoard, r, c);
                if (!jumpsForThisPiece.empty()) {
                    jumpAvailableAnywhere = true;
                    break;
                }
            }
        }
        if (jumpAvailableAnywhere) break;
    }

    // Generar los movimientos para la pieza específica que se intenta mover
    std::vector<Move> movesForThisPiece = GenerateMovesForPiece(gameBoard, startRow, startCol);

    for (const Move& move : movesForThisPiece) {
        if (move.endR_ == endRow && move.endC_ == endCol) {
            // El movimiento está en la lista de movimientos generados para esta pieza.
            // Ahora, aplicar la regla de captura obligatoria:
            if (jumpAvailableAnywhere && !move.isCapture_) {
                // std::cout << "[Debug MG] IsValidMove: Movimiento simple no permitido, hay captura disponible en otra parte." << std::endl;
                return false; // No es válido si hay un salto disponible en cualquier parte y este no es un salto
            }
            wasCapture = move.isCapture_;
            return true;
        }
    }
    // std::cout << "[Debug MG] IsValidMove: Movimiento no encontrado en GenerateMovesForPiece." << std::endl;
    return false;
}


bool MoveGenerator::HasAnyValidMoves(const Board& gameBoard, PlayerColor player) const {
    bool jumpAvailable = false;
    // Primero, verificar si hay algún salto disponible (ya que son obligatorios)
    for (int r = 0; r < Board::BOARD_SIZE; ++r) {
        for (int c = 0; c < Board::BOARD_SIZE; ++c) {
            PieceType piece = gameBoard.GetPieceAt(r, c);
            if (GetPlayerFromPiece(piece) == player) {
                if (!GetPossibleJumpsForSpecificPiece(gameBoard, r, c).empty()) {
                    jumpAvailable = true;
                    break;
                }
            }
        }
        if (jumpAvailable) break;
    }

    if (jumpAvailable) return true; // Si hay saltos, hay movimientos válidos.

    // Si no hay saltos, verificar movimientos simples
    for (int r = 0; r < Board::BOARD_SIZE; ++r) {
        for (int c = 0; c < Board::BOARD_SIZE; ++c) {
            PieceType piece = gameBoard.GetPieceAt(r, c);
            if (GetPlayerFromPiece(piece) == player) {
                std::vector<Move> simpleMoves;
                if (piece == PieceType::P1_MAN || piece == PieceType::P2_MAN) {
                    FindSimplePawnMoves(gameBoard, r, c, player, piece, simpleMoves);
                }
                else if (piece == PieceType::P1_KING || piece == PieceType::P2_KING) {
                    FindSimpleKingMoves(gameBoard, r, c, player, piece, simpleMoves);
                }
                if (!simpleMoves.empty()) {
                    return true;
                }
            }
        }
    }
    return false; // No se encontraron ni saltos ni movimientos simples
}