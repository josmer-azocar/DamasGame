#include <iostream>
#include <windows.h> // Para GetStdHandle, CONSOLE_CURSOR_INFO, etc.

#include "ConsoleView.h"  // Para SetConsoleTextColor y constantes de color
#include "InputHandler.h"
#include "Board.h"
#include "GameManager.h"

// Funcion principal
int main() {
	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hConsoleOut, &cursorInfo);
	bool originalCursorVisibility = cursorInfo.bVisible; // Guardar estado original
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(hConsoleOut, &cursorInfo);

	Board gameBoard;
	//ConsoleView view;
	InputHandler inputHandler;

	GameManager game(gameBoard,inputHandler);
	// Inicializar la aplicación
	game.InitializeApplication(); // Esto maneja el menú y los bucles de juego

	
	// Restaurar cursor y colores de consola
	cursorInfo.bVisible = originalCursorVisibility;
	SetConsoleCursorInfo(hConsoleOut, &cursorInfo);

	// Resetear a colores estándar de la consola (gris claro sobre negro)
	WORD defaultAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Gris claro
	SetConsoleTextAttribute(hConsoleOut, defaultAttributes);
	system("cls"); // Limpiar la pantalla una última vez con los colores estándar

	std::cout << "Gracias por jugar. Programa finalizado." << std::endl;
	std::cout << "Thanks for playing. program over." << std::endl;
	return 0;
}