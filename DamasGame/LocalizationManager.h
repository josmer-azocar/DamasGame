#ifndef LOCALIZATION_MANAGER_H
#define LOCALIZATION_MANAGER_H

#include <string>
#include <map>
#include <vector> 

class ConsoleView;  
class InputHandler; 

// Clase para manejar la carga y obtencion de cadenas de texto traducidas
class LocalizationManager {
public:
    // Constructor: inicializa el idioma por defecto y el mapa de cadenas
    LocalizationManager();

    // Muestra el menu de seleccion de idioma y retorna el codigo seleccionado
    std::string SelectLanguageUI(const ConsoleView& view, InputHandler& inputHandler);

    // Carga el archivo de idioma especificado (por codigo) y llena el mapa de cadenas
    bool LoadLanguage(const std::string& langCode);
    // Devuelve el string asociado a una clave, o la clave entre corchetes si no existe
    std::string GetString(const std::string& key) const;
    // Devuelve el string asociado a una clave, reemplazando los placeholders por los valores dados
    std::string GetString(const std::string& key, const std::map<std::string, std::string>& replacements) const;

private:
    std::map<std::string, std::string> m_strings; // Almacena las cadenas cargadas (clave -> valor)
    std::string m_currentLangCode; // Codigo del idioma actualmente cargado

    // Funcion helper para reemplazar placeholders en una cadena
    std::string ReplacePlaceholders(std::string text, const std::map<std::string, std::string>& replacements) const;
};

#endif // LOCALIZATION_MANAGER_H