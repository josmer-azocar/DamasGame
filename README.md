# ♟️ Juego de Damas en C++
<img width="650" height="506" alt="image" src="https://github.com/user-attachments/assets/f6c9d3bc-0c39-4864-93e8-31cf1871e9da" />

![C++](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows%2FLinux%2FmacOS-lightgrey)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

Una implementación del clásico juego de mesa "Damas" desarrollada completamente en **C++ estándar**, sin dependencias de librerías o frameworks externos. El proyecto se ejecuta en una consola o terminal y se enfoca en la lógica pura del juego, la algoritmia y la gestión de memoria.

Este proyecto es un excelente ejemplo de la aplicación de principios de Programación Orientada a Objetos y estructuras de datos para modelar un sistema complejo desde cero.

---

<p align="center">
<img width="400" alt="image" src="https://github.com/user-attachments/assets/b393e64c-4e40-4cc5-b3c5-71a7cde817d6"/>
<img width="400" "alt="image2" src="https://github.com/user-attachments/assets/87f4fbc1-1111-4242-aef0-5b8080bb5412"/>
</p>

---

## 📜 Tabla de Contenidos

- [Demostración](#-demostración)
- [✨ Características Principales](#-características-principales)
- [🛠️ Arquitectura y Diseño en C++](#-arquitectura-y-diseño-en-c)
- [⚙️ Compilación y Ejecución](#-compilación-y-ejecución)
- [🚀 Cómo Jugar](#-cómo-jugar)
- [📊 Diagrama UML](#-diagrama-uml)
- [📄 Licencia](#-licencia)

## 📸 Demostración

```
  +---+---+---+---+---+---+---+---+
8 | b |   | b |   | b |   | b |   |
  +---+---+---+---+---+---+---+---+
7 |   | b |   | b |   | b |   | b |
  +---+---+---+---+---+---+---+---+
6 | b |   | b |   | b |   | b |   |
  +---+---+---+---+---+---+---+---+
5 |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
4 |   |   |   |   |   |   |   |   |
  +---+---+---+---+---+---+---+---+
3 |   | w |   | w |   | w |   | w |
  +---+---+---+---+---+---+---+---+
2 | w |   | w |   | w |   | w |   |
  +---+---+---+---+---+---+---+---+
1 |   | w |   | w |   | w |   | w |
  +---+---+---+---+---+---+---+---+
    A   B   C   D   E   F   G   H

Turno de las Blancas (w).
Ingrese movimiento (ej: C3 D4):
```

## ✨ Características Principales

- **Cero Dependencias:** Construido con C++ estándar y la librería STL, garantizando máxima portabilidad.
- **Lógica de Juego Completa:** Implementa todas las reglas fundamentales, incluyendo:
    - Movimiento simple y captura de fichas.
    - Capturas múltiples en un solo turno.
    - **Coronación:** Las fichas se convierten en "Reinas" (Damas) al alcanzar el final del tablero, ganando la habilidad de moverse en cualquier dirección diagonal.
- **Interfaz de Consola:** Interacción a través de la línea de comandos, ideal para demostrar la lógica del backend sin una capa de UI.
- **Representación del Tablero:** Utiliza una matriz 2D para una gestión eficiente del estado del juego.
- **Validación de Entradas:** El sistema valida que los movimientos ingresados por el usuario sean sintáctica y lógicamente correctos.

## 🛠️ Arquitectura y Diseño en C++

El código está estructurado siguiendo los paradigmas de la Programación Orientada a Objetos para asegurar un diseño limpio, modular y extensible.

- **`main.cpp`**: Punto de entrada de la aplicación. Contiene el bucle principal del juego (`game loop`) que gestiona los turnos, renderiza el tablero y procesa las entradas del usuario hasta que se cumple una condición de victoria.

- **Clase `Tablero`**: Encapsula la representación del tablero, una matriz de 8x8. Provee métodos para obtener el estado de una casilla, modificarla y renderizar la representación visual en la consola.

- **Clase `Ficha`**: Modela las piezas del juego. Contiene atributos como el color (blanco/negro) y un booleano para indicar si es una "Reina".

- **Módulo de Lógica**: Centraliza las reglas del juego. Funciones o métodos para validar si un movimiento es legal, verificar capturas, gestionar la coronación y determinar el fin de la partida.

## ⚙️ Compilación y Ejecución

Para compilar y ejecutar el proyecto, solo necesitas un compilador de C++ moderno (compatible con C++17 o superior), como G++ o Clang.

**Pasos para compilar:**

1.  **Clona el repositorio:**
    ```bash
    git clone https://github.com/josueazocar/DamasGame.git
    ```
2.  **Navega al directorio del proyecto:**
    ```bash
    cd DamasGame
    ```
3.  **Compila los archivos fuente:**
    ```bash
    g++ -std=c++17 -o damas main.cpp # Añade aquí otros archivos .cpp si los tienes
    ```
    *Si tienes los archivos separados (ej. `LogicaJuego.cpp`), compílalos juntos:*
    ```bash
    g++ -std=c++17 -o damas main.cpp LogicaJuego.cpp
    ```

4.  **Ejecuta el juego:**
    ```bash
    ./damas
    ```

## 🚀 Cómo Jugar

- El juego se controla introduciendo coordenadas en la terminal.
- El formato de movimiento es `[Origen][Destino]`, por ejemplo: `C3 D4`.
- `C3` es la casilla donde está la ficha que quieres mover.
- `D4` es la casilla a la que quieres moverla.
- El juego te indicará de quién es el turno y esperará tu entrada.

## 📊 Diagrama UML

A continuación se presenta el diagrama de clases del sistema, que modela las principales entidades y sus relaciones.
<img width="4463" height="4059" alt="Diagrama DamasGame Arreglado" src="https://github.com/user-attachments/assets/4f5e31b5-f7f8-4fe8-a1a1-abe008c79ccd" />


## 📄 Licencia

Este proyecto está bajo la Licencia MIT. Consulta el archivo `LICENSE` para más detalles.
