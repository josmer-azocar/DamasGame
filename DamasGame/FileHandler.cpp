#include "FileHandler.h" // Incluimos nuestro archivo de cabecera
#include <fstream>       // Necesario para std::ofstream (output file stream) y std::ifstream (input file stream)
#include <iostream>      // Necesario para std::cout y std::cerr
#include <string>        // Ya incluido en el .h, pero buena práctica si se usa string aquí directamente
#include <vector>        // Necesario para std::vector si alguna función lo usara (no es el caso actual, pero útil)
#include <chrono>        // Necesario para obtener el tiempo actual (aunque usaremos ctime para formatearlo)
#include <ctime>         // Necesario para time_t, tm, localtime, strftime

// Constructor (puede estar vacío)
FileHandler::FileHandler() {
    // No se necesita inicialización compleja por ahora
}

// Destructor (puede estar vacío)
FileHandler::~FileHandler() {
    // No se necesita limpieza compleja por ahora
}

// Implementación para guardar el resultado de la partida
bool FileHandler::saveGameResult(const GameResult& result) {
    // Abrir el archivo en modo apéndice (ios::app)
    // Si el archivo no existe, se creará.
    std::ofstream outputFile(RESULTS_FILENAME, std::ios::app);

    // Verificar si el archivo se abrió correctamente
    if (!outputFile.is_open()) {
        std::cerr << "Error al abrir el archivo de resultados: " << RESULTS_FILENAME << std::endl;
        return false; // Indicar que falló el guardado
    }

    // Formatear la línea de texto según la estructura de la imagen:
    // Fecha Hora;TipoJugador1 vs TipoJugador2;Ganador;RazonGanador;Turnos;CapturasGanador;CapturasPerdedor
    outputFile << result.date << " " << result.time << ";"
        << result.playerTypes << ";"
        << result.winner << ";"
        << result.reason << ";"
        << result.totalTurns << ";"
        << result.winnerCaptures << ";"
        << result.loserCaptures << std::endl; // std::endl añade un salto de línea y vacía el buffer

    // Cerrar el archivo
    outputFile.close();

    // Indicar que se guardó correctamente
    return true;
}

// Implementación para leer y mostrar el historial de partidas
void FileHandler::displayGameHistory() {
    // Abrir el archivo en modo lectura (ios::in es por defecto para ifstream)
    std::ifstream inputFile(RESULTS_FILENAME);

    // Verificar si el archivo se abrió correctamente
    // Esto también verifica si el archivo existe.
    if (!inputFile.is_open()) {
        std::cout << "-----------------------------------" << std::endl;
        std::cout << "HISTORIAL DE PARTIDAS GUARDADAS" << std::endl;
        std::cout << "-----------------------------------" << std::endl;
        std::cout << "No hay historial de partidas guardado aun." << std::endl;
        return; // Salir de la función si no se puede abrir (o no existe)
    }

    std::cout << "-----------------------------------" << std::endl;
    std::cout << "HISTORIAL DE PARTIDAS GUARDADAS" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    std::string line;
    // Leer el archivo línea por línea
    while (std::getline(inputFile, line)) {
        // Imprimir cada línea leída (que ya está formateada)
        std::cout << line << std::endl;
    }

    // Cerrar el archivo
    inputFile.close();

    std::cout << "-----------------------------------" << std::endl; // Línea de cierre para la visualización
}

// Nota: Faltaría una función para obtener la fecha y hora actual en el formato "YYYY-MM-DD HH:MM:SS"
// Podemos agregarla aquí o donde se preparen los datos GameResult antes de llamar a saveGameResult.
// Es más limpio prepararla antes, en el GameManager.