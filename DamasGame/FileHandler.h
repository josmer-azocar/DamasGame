#pragma once

#include <string> 
#include "LocalizationManager.h"

// Estructura que representa el resultado de una partida
struct GameResult {
    std::string date;           // Fecha de la partida
    std::string time;           // Hora de la partida
    std::string playerTypes;    // Tipos de jugadores (ej: Humano vs IA)
    std::string winner;         // Nombre o tipo del ganador
    std::string reason;         // Razon por la que termino la partida
    int totalTurns;             // Numero total de turnos jugados
    int winnerCaptures;         // Cantidad de capturas realizadas por el ganador
    int loserCaptures;          // Cantidad de capturas realizadas por el perdedor
};

// Clase encargada de manejar la lectura y escritura de resultados de partidas en archivo
class FileHandler {
public:
    // Constructor: recibe una referencia al manejador de localizacion
    FileHandler(const LocalizationManager& i18n);
    // Destructor
    ~FileHandler();

    // Guarda los datos de una partida al final del archivo de resultados
    // Retorna true si se guardo correctamente, false en caso contrario
    bool saveGameResult(const GameResult& result);

    // Lee todos los resultados guardados en el archivo y los muestra en la consola
    void displayGameHistory();

private:
    // Nombre del archivo donde se guardaran los resultados
    const std::string RESULTS_FILENAME = "damas_results.txt";
    // Referencia al manejador de localizacion para mostrar mensajes en el idioma adecuado
    const LocalizationManager& m_i18n;
};