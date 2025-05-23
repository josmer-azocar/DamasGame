#include "FileHandler.h" 
#include <fstream>       
#include <iostream>      
#include <string>        
#include <vector>        
#include <chrono>        
#include <ctime>         


// Constructor de FileHandler
// Recibe una referencia al manejador de localizacion para mostrar mensajes en el idioma adecuado
FileHandler::FileHandler(const LocalizationManager& i18n) :
    m_i18n(i18n)
{
    // No requiere inicializacion adicional
}

// Destructor de FileHandler
FileHandler::~FileHandler() {
   // No requiere limpieza especial
}

// Implementacion para guardar el resultado de la partida en un archivo de texto
// Devuelve true si el guardado fue exitoso, false si hubo un error
bool FileHandler::saveGameResult(const GameResult& result) {
    // Abrir el archivo en modo append (ios::app)
    // Si el archivo no existe, se creara
    std::ofstream outputFile(RESULTS_FILENAME, std::ios::app);

    // Verificar si el archivo se abrio correctamente
    if (!outputFile.is_open()) {
        std::cerr << m_i18n.GetString("error_opening_results_file") << RESULTS_FILENAME << std::endl;
        return false; // Indicar que fallo el guardado
    }

    // Formatear la linea de texto segun la estructura:
    // Fecha Hora;TipoJugador1 vs TipoJugador2;Ganador;RazonGanador;Turnos;CapturasGanador;CapturasPerdedor
    outputFile << result.date << " " << result.time << ";"
        << result.playerTypes << ";"
        << result.winner << ";"
        << result.reason << ";"
        << result.totalTurns << ";"
        << result.winnerCaptures << ";"
        << result.loserCaptures << std::endl; // std::endl anade un salto de linea y vacia el buffer

    // Cerrar el archivo
    outputFile.close();

    // Indicar que se guardo correctamente
    return true;
}

// Implementacion para leer y mostrar el historial de partidas guardadas
void FileHandler::displayGameHistory() {
    // Abrir el archivo en modo lectura (ios::in es por defecto para ifstream)
    std::ifstream inputFile(RESULTS_FILENAME);

    // Verificar si el archivo se abrio correctamente
    // Esto tambien verifica si el archivo existe.
    if (!inputFile.is_open()) {
        std::cout << "-----------------------------------" << std::endl;
        std::cout << m_i18n.GetString("stats_history_title") << std::endl;
        std::cout << "-----------------------------------" << std::endl;
        std::cout << m_i18n.GetString("stats_no_history") << std::endl;
        return; // Salir de la funcion si no se puede abrir (o no existe)
    }

    std::cout << "-----------------------------------" << std::endl;
    std::cout << m_i18n.GetString("stats_history_title") << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    std::string line;
    // Leer el archivo linea por linea
    while (std::getline(inputFile, line)) {
        // Imprimir cada linea leida (que ya esta formateada)
        std::cout << line << std::endl;
    }

    // Cerrar el archivo
    inputFile.close();

    std::cout << "-----------------------------------" << std::endl; // Linea de cierre para la visualizacion
}

