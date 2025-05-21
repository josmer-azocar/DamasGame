#pragma once

#include <string> 
#include "LocalizationManager.h"

struct GameResult {
    std::string date;
    std::string time;
    std::string playerTypes; 
    std::string winner;      
    std::string reason;      
    int totalTurns;
    int winnerCaptures;
    int loserCaptures;
};

class FileHandler {
public:
    
    FileHandler(const LocalizationManager& i18n);
    ~FileHandler();

    // Guarda los datos de una partida al final del archivo de resultados
    // Retorna true si se guardó correctamente, false en caso contrario.
    bool saveGameResult(const GameResult& result);

    // Lee todos los resultados guardados en el archivo y los muestra en la consola
    void displayGameHistory();

private:
    // Nombre del archivo donde se guardarán los resultados
    const std::string RESULTS_FILENAME = "damas_results.txt";
    const LocalizationManager& m_i18n;
};