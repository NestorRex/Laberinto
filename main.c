#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

// Definiciones básicas del laberinto
#define MAZE_SIZE 10
#define WALL '#'
#define PATH '.'
#define PLAYER 'P'
#define EXIT 'X'

// Declaraciones de funciones en ensamblador (por implementar)
extern void asm_init_maze(char maze[][MAZE_SIZE]);
extern int asm_check_collision(char maze[][MAZE_SIZE], int x, int y);

// Función para limpiar pantalla
void clear_screen() {
    system("cls");
}

// Función para imprimir el laberinto
void print_maze(char maze[][MAZE_SIZE]) {
    clear_screen();
    printf("=== LABERINTO GAME ===\n\n");
    
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            printf("%c ", maze[i][j]);
        }
        printf("\n");
    }
    printf("\nUsa WASD para moverte, Q para salir\n");
}

int main() {
    char maze[MAZE_SIZE][MAZE_SIZE];
    int player_x = 1, player_y = 1;
    char input;
    
    // Inicializar laberinto
    asm_init_maze(maze);
    
    // Colocar jugador
    maze[player_y][player_x] = PLAYER;
    
    printf("Bienvenido al Laberinto!\n");
    printf("Encuentra la salida (X)\n");
    printf("Presiona cualquier tecla para comenzar...\n");
    _getch();
    
    // Bucle principal del juego
    while (1) {
        print_maze(maze);
        printf("Movimiento: ");
        input = _getch();
        
        int new_x = player_x;
        int new_y = player_y;
        
        // Procesar entrada
        switch (input) {
            case 'w':
            case 'W':
                new_y = player_y - 1;
                break;
            case 's':
            case 'S':
                new_y = player_y + 1;
                break;
            case 'a':
            case 'A':
                new_x = player_x - 1;
                break;
            case 'd':
            case 'D':
                new_x = player_x + 1;
                break;
            case 'q':
            case 'Q':
                printf("Gracias por jugar!\n");
                return 0;
            default:
                continue;
        }
        
        // Verificar si el movimiento es válido
        if (!asm_check_collision(maze, new_x, new_y)) {
            // Limpiar posición anterior
            maze[player_y][player_x] = PATH;
            
            // Actualizar posición
            player_x = new_x;
            player_y = new_y;
            
            // Verificar si llegó a la salida
            if (maze[player_y][player_x] == EXIT) {
                clear_screen();
                printf("=== FELICIDADES! ===\n");
                printf("Has encontrado la salida!\n");
                printf("Presiona cualquier tecla para salir...\n");
                _getch();
                break;
            }
            
            // Colocar jugador en nueva posición
            maze[player_y][player_x] = PLAYER;
        }
    }
    
    return 0;
}