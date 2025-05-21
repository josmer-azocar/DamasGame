#ifndef LOCALIZATION_MANAGER_H
#define LOCALIZATION_MANAGER_H

#include <string>
#include <map>
#include <vector> 

class ConsoleView;  
class InputHandler; 

// Clase para manejar la carga y obtención de cadenas de texto traducidas.
class LocalizationManager {
public:
    LocalizationManager();

    std::string SelectLanguageUI(const ConsoleView& view, InputHandler& inputHandler);

    bool LoadLanguage(const std::string& langCode);
    std::string GetString(const std::string& key) const;
    std::string GetString(const std::string& key, const std::map<std::string, std::string>& replacements) const;


private:
    std::map<std::string, std::string> m_strings; // Almacena las cadenas cargadas (clave -> valor).
    std::string m_currentLangCode; // Código del idioma actualmente cargado.

    // Función helper para reemplazar placeholders en una cadena.
    std::string ReplacePlaceholders(std::string text, const std::map<std::string, std::string>& replacements) const;
};

#endif // LOCALIZATION_MANAGER_H