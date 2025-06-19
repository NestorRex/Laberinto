#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

// Definiciones del laberinto - ahora 12x12
#define MAZE_SIZE 12
#define WALL '#'
#define PATH '.'
#define PLAYER 'P'
#define EXIT 'X'

// Declaraciones de funciones en ensamblador
extern void asm_init_maze(char maze[][MAZE_SIZE]);
extern int asm_check_collision(char maze[][MAZE_SIZE], int x, int y);
extern void asm_move_player(char maze[][MAZE_SIZE], int* player_x, int* player_y, int new_x, int new_y);

// Funciones para mejorar la interfaz
void clear_screen() {
    system("cls");
}

void hide_cursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void set_color(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Imprimir el laberinto con colores
void print_maze(char maze[][MAZE_SIZE], int moves, int start_time) {
    clear_screen();
    
    // Título del juego
    set_color(14); // Amarillo
    printf("╔══════════════════════════════════════╗\n");
    printf("║          🎮 LABERINTO GAME 🎮        ║\n");
    printf("╚══════════════════════════════════════╝\n\n");
    
    // Instrucciones
    set_color(11); // Cian claro
    printf("Controles: W↑ A← S↓ D→ | R=Reiniciar Q=Salir\n");
    printf("Objetivo: Llega a la X desde P\n\n");
    
    // Laberinto
    set_color(7); // Blanco
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            if (maze[i][j] == WALL) {
                set_color(8); // Gris oscuro
                printf("██");
            } else if (maze[i][j] == PLAYER) {
                set_color(10); // Verde brillante
                printf("🚀");
            } else if (maze[i][j] == EXIT) {
                set_color(12); // Rojo brillante
                printf("🎯");
            } else {
                set_color(7); // Blanco
                printf("  ");
            }
        }
        printf("\n");
    }
    
    // Estadísticas
    set_color(13); // Magenta
    printf("\n═══════════════════════════════════════\n");
    printf("Movimientos: %d | Tiempo: %ds\n", moves, (int)(time(NULL) - start_time));
    printf("═══════════════════════════════════════\n");
    set_color(7);
}

void show_instructions() {
    clear_screen();
    set_color(14);
    printf("╔══════════════════════════════════════╗\n");
    printf("║          🎮 LABERINTO GAME 🎮        ║\n");
    printf("╚══════════════════════════════════════╝\n\n");
    
    set_color(11);
    printf("🎯 OBJETIVO:\n");
    printf("   Encuentra la salida del laberinto\n\n");
    
    printf("🕹️ CONTROLES:\n");
    printf("   W - Mover arriba\n");
    printf("   A - Mover izquierda\n");
    printf("   S - Mover abajo\n");
    printf("   D - Mover derecha\n");
    printf("   R - Reiniciar nivel\n");
    printf("   Q - Salir del juego\n\n");
    
    printf("📊 EL JUEGO REGISTRA:\n");
    printf("   • Número de movimientos\n");
    printf("   • Tiempo transcurrido\n\n");
    
    set_color(10);
    printf("Presiona cualquier tecla para comenzar...\n");
    set_color(7);
    _getch();
}

void show_victory(int moves, int time_taken) {
    clear_screen();
    set_color(10);
    printf("╔══════════════════════════════════════╗\n");
    printf("║              🎉 ¡VICTORIA! 🎉        ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║                                      ║\n");
    printf("║    ¡Felicidades! Has completado     ║\n");
    printf("║         el laberinto! 🏆             ║\n");
    printf("║                                      ║\n");
    set_color(14);
    printf("║    📊 Estadísticas finales:          ║\n");
    printf("║       • Movimientos: %-3d             ║\n", moves);
    printf("║       • Tiempo: %-3d segundos        ║\n", time_taken);
    set_color(10);
    printf("║                                      ║\n");
    printf("║    Presiona cualquier tecla...       ║\n");
    printf("║                                      ║\n");
    printf("╚══════════════════════════════════════╝\n");
    set_color(7);
    _getch();
}

int main() {
    char maze[MAZE_SIZE][MAZE_SIZE];
    int player_x = 1, player_y = 1;
    int moves = 0;
    int game_running = 1;
    char input;
    
    hide_cursor();
    show_instructions();
    
    while (game_running) {
        // Reiniciar juego
        asm_init_maze(maze);
        player_x = 1;
        player_y = 1;
        moves = 0;
        maze[player_y][player_x] = PLAYER;
        
        int level_running = 1;
        int start_time = (int)time(NULL);
        
        while (level_running) {
            print_maze(maze, moves, start_time);
            
            input = _getch();
            input = tolower(input);
            
            int new_x = player_x;
            int new_y = player_y;
            int valid_input = 0;
            
            switch (input) {
                case 'w':
                    new_y = player_y - 1;
                    valid_input = 1;
                    break;
                case 's':
                    new_y = player_y + 1;
                    valid_input = 1;
                    break;
                case 'a':
                    new_x = player_x - 1;
                    valid_input = 1;
                    break;
                case 'd':
                    new_x = player_x + 1;
                    valid_input = 1;
                    break;
                case 'r':
                    level_running = 0; // Reiniciar
                    break;
                case 'q':
                    level_running = 0;
                    game_running = 0;
                    break;
            }
            
            if (valid_input) {
                if (!asm_check_collision(maze, new_x, new_y)) {
                    // Verificar si llegó a la salida antes de mover
                    if (maze[new_y][new_x] == EXIT) {
                        int time_taken = (int)(time(NULL) - start_time);
                        moves++;
                        show_victory(moves, time_taken);
                        level_running = 0;
                    } else {
                        asm_move_player(maze, &player_x, &player_y, new_x, new_y);
                        moves++;
                    }
                }
            }
        }
    }
    
    clear_screen();
    set_color(11);
    printf("¡Gracias por jugar! 👋\n");
    set_color(7);
    return 0;
}