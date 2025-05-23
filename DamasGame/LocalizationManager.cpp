#include "LocalizationManager.h"
#include <fstream>
#include <iostream> 
#include <sstream>  
#include "ConsoleView.h"      
#include "InputHandler.h"     
#include "CommonTypes.h"      
#include <algorithm> 
#include <thread>    
#include <chrono>    
#include <limits>

#include "nlohmann/json.hpp" 

using json = nlohmann::json;

// Constructor de LocalizationManager
// Inicializa el idioma por defecto (ingles) e intenta cargarlo
LocalizationManager::LocalizationManager() : m_currentLangCode("en") {
    // Intentar cargar un idioma por defecto al inicio.
    // Si esto falla, el mapa m_strings estara vacio, y GetString devolvera "[clave]".
    if (!LoadLanguage(m_currentLangCode)) {
        // Si falla, se mantiene el idioma ingles como fallback
    }
}

// Muestra el menu de seleccion de idioma y permite al usuario elegir entre ingles y espanol
// Devuelve el codigo del idioma seleccionado
std::string LocalizationManager::SelectLanguageUI(const ConsoleView& view, InputHandler& inputHandler) {
    std::string finalSelectedLangCode = "en"; // Default a ingles si todo falla
    bool languageSelectedAndLoaded = false;
    int currentMenuOption = 1;
    const int numLangOptions = 2;

    // Usar textos fijos para el menu de seleccion de idioma, ya que el idioma aun no esta garantizado
    std::string titleText = "SELECT LANGUAGE / SELECCIONE IDIOMA";
    std::string spanishOptionText = "1. Espanol (Spanish)";
    std::string englishOptionText = "2. English";
    std::string instructionText = "Use W/S or Up/Down, Enter to select.";

    // Cargar 'en' primero para tener un fallback si la seleccion del usuario falla
    bool englishLoadedInitially = LoadLanguage("en");
    if (englishLoadedInitially) {
        finalSelectedLangCode = "en";
        // Intentar obtener textos para el menu de seleccion desde en.json
        std::string tempTitle = GetString("language_selection_title");
        if (tempTitle.rfind("[language_selection_title]", 0) != 0) titleText = tempTitle;
    }

    while (!languageSelectedAndLoaded) {
        view.SetGameColorsAndClear(); // Fondo negro
        view.DisplayLanguageSelectionMenu(currentMenuOption, titleText, spanishOptionText, englishOptionText, instructionText);

        int choice = inputHandler.GetMenuChoice(currentMenuOption, numLangOptions);
        std::string langToTryThisIteration = "";

        if (choice > 0) {
            currentMenuOption = choice;
            if (currentMenuOption == 1) {
                langToTryThisIteration = "es";
            }
            else if (currentMenuOption == 2) {
                langToTryThisIteration = "en";
            }

            if (!langToTryThisIteration.empty()) {
                if (LoadLanguage(langToTryThisIteration)) { // Intenta cargar el idioma seleccionado
                    finalSelectedLangCode = langToTryThisIteration;
                    languageSelectedAndLoaded = true;
                }
                else {
                    // Fallo la carga del idioma seleccionado. Informar y el bucle continuara.
                    view.SetGameColorsAndClear();
                    GoToXY(5, 15);
                    std::string errorMsg = "ERROR: No se pudo cargar '" + langToTryThisIteration + ".json'.";
                    if (langToTryThisIteration != "en" && englishLoadedInitially) {
                        errorMsg += "\nVolviendo a Ingles por defecto.";
                        finalSelectedLangCode = "en";
                        languageSelectedAndLoaded = true;
                    }
                    else if (langToTryThisIteration != "en" && !englishLoadedInitially) {
                        errorMsg += "\nERROR CRITICO: Ingles por defecto ('en.json') tampoco se pudo cargar.";
                    }
                    else if (langToTryThisIteration == "en" && !englishLoadedInitially) {
                        errorMsg = "ERROR CRITICO: No se pudo cargar 'en.json'. El programa no funcionara correctamente.";
                    }
                    view.DisplayMessage(errorMsg, true, CONSOLE_COLOR_RED, CONSOLE_COLOR_BLACK);
                    view.DisplayMessage("Presione Enter para continuar...", true, CONSOLE_COLOR_LIGHT_GRAY, CONSOLE_COLOR_BLACK);
                    if (std::cin.peek() == '\n') std::cin.ignore();
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                }
            }
        }
        else if (choice < 0) {
            currentMenuOption = -choice;
        }
    }

    return finalSelectedLangCode;
}

// Carga el archivo de idioma especificado y llena el mapa de strings
// Devuelve true si la carga fue exitosa, false si hubo un error
bool LocalizationManager::LoadLanguage(const std::string& langCodeToLoad) {
    m_strings.clear();
    std::string filePath = "resources/lang/" + langCodeToLoad + ".json";

    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false; // Simplemente falla si el archivo especifico no se abre
    }

    try {
        json langJsonData;
        file >> langJsonData;

        if (langJsonData.is_object()) {
            int count = 0;
            for (json::iterator it = langJsonData.begin(); it != langJsonData.end(); ++it) {
                const std::string& key = it.key();
                const json& valueJson = it.value();
                if (valueJson.is_string()) {
                    m_strings[key] = valueJson.get<std::string>();
                    count++;
                }
            }
            if (count == 0) { // Archivo JSON valido pero vacio o sin strings
                file.close();
                return false;
            }
        }
        else {
            file.close();
            return false;
        }
        m_currentLangCode = langCodeToLoad; // Solo establecer si la carga fue exitosa
        file.close();
        return true;
    }
    catch (const std::exception& e) {
        file.close();
        return false;
    }
}

// Devuelve el string asociado a una clave, o la clave entre corchetes si no existe
std::string LocalizationManager::GetString(const std::string& key) const {
    auto it = m_strings.find(key);
    if (it != m_strings.end()) {
        return it->second;
    }
    return "[" + key + "]";
}

// Reemplaza los placeholders en un string por los valores dados en replacements
std::string LocalizationManager::ReplacePlaceholders(std::string text, const std::map<std::string, std::string>& replacements) const {
    for (const auto& pair : replacements) {
        std::string placeholder = "{" + pair.first + "}"; 
        size_t start_pos = 0;
        while ((start_pos = text.find(placeholder, start_pos)) != std::string::npos) {
            text.replace(start_pos, placeholder.length(), pair.second);
            start_pos += pair.second.length();
        }
    }
    return text;
}

// Devuelve el string asociado a una clave, reemplazando los placeholders por los valores dados
std::string LocalizationManager::GetString(const std::string& key, const std::map<std::string, std::string>& replacements) const {
    std::string rawString = GetString(key);
    return ReplacePlaceholders(rawString, replacements);
}