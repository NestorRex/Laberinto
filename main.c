#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <ctype.h>

// Definiciones del laberinto
#define MAZE_SIZE 12
#define WALL '#'
#define PATH '.'
#define PLAYER 'P'
#define EXIT 'X'
#define EMPTY ' '

// Estructura para posición
typedef struct {
    int x, y;
} Position;

// Declaraciones de funciones en ensamblador
extern int asm_check_collision(char maze[][MAZE_SIZE], int x, int y);
extern void asm_move_player(char maze[][MAZE_SIZE], int* player_x, int* player_y, int new_x, int new_y);
extern int asm_check_win(char maze[][MAZE_SIZE], int player_x, int player_y);
extern void asm_init_maze(char maze[][MAZE_SIZE]);

// Funciones auxiliares en C
void clear_screen() {
    system("cls");
}

void set_cursor_position(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hide_cursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void print_maze(char maze[][MAZE_SIZE]) {
    set_cursor_position(0, 2);
    printf("╔");
    for (int i = 0; i < MAZE_SIZE; i++) printf("══");
    printf("╗\n");
    
    for (int i = 0; i < MAZE_SIZE; i++) {
        printf("║");
        for (int j = 0; j < MAZE_SIZE; j++) {
            if (maze[i][j] == WALL) {
                printf("██");
            } else if (maze[i][j] == PLAYER) {
                printf("🚀");
            } else if (maze[i][j] == EXIT) {
                printf("🎯");
            } else {
                printf("  ");
            }
        }
        printf("║\n");
    }
    
    printf("╚");
    for (int i = 0; i < MAZE_SIZE; i++) printf("══");
    printf("╝\n");
}

void print_instructions() {
    printf("╔══════════════════════════════════════╗\n");
    printf("║          🎮 LABERINTO GAME 🎮        ║\n");
    printf("╚══════════════════════════════════════╝\n");
    printf("\n");
    printf("Controles:\n");
    printf("  W - Mover arriba ⬆️\n");
    printf("  A - Mover izquierda ⬅️\n");
    printf("  S - Mover abajo ⬇️\n");
    printf("  D - Mover derecha ➡️\n");
    printf("  R - Reiniciar juego 🔄\n");
    printf("  Q - Salir 🚪\n");
    printf("\n");
    printf("Objetivo: Llega al objetivo 🎯 desde tu posición 🚀\n\n");
}

void print_status(int moves, int time_elapsed) {
    set_cursor_position(0, MAZE_SIZE + 6);
    printf("═══════════════════════════════════════\n");
    printf("Movimientos: %d | Tiempo: %ds\n", moves, time_elapsed);
    printf("═══════════════════════════════════════\n");
}

void show_victory_screen(int moves, int time_elapsed) {
    clear_screen();
    printf("╔══════════════════════════════════════╗\n");
    printf("║              🎉 ¡VICTORIA! 🎉        ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║                                      ║\n");
    printf("║    ¡Felicidades! Has completado     ║\n");
    printf("║         el laberinto! 🏆             ║\n");
    printf("║                                      ║\n");
    printf("║    Estadísticas finales:             ║\n");
    printf("║    • Movimientos: %-3d               ║\n", moves);
    printf("║    • Tiempo: %-3d segundos           ║\n", time_elapsed);
    printf("║                                      ║\n");
    printf("║    Presiona cualquier tecla para     ║\n");
    printf("║    volver al menú principal...       ║\n");
    printf("║                                      ║\n");
    printf("╚══════════════════════════════════════╝\n");
    _getch();
}

int main() {
    char maze[MAZE_SIZE][MAZE_SIZE];
    int player_x = 1, player_y = 1;
    int moves = 0;
    int game_running = 1;
    char input;
    
    // Configurar consola
    hide_cursor();
    
    while (game_running) {
        // Inicializar laberinto
        asm_init_maze(maze);
        player_x = 1;
        player_y = 1;
        moves = 0;
        
        // Colocar jugador en posición inicial
        maze[player_y][player_x] = PLAYER;
        
        int level_running = 1;
        int start_time = (int)(GetTickCount() / 1000);
        
        while (level_running) {
            int current_time = (int)(GetTickCount() / 1000);
            int elapsed_time = current_time - start_time;
            
            clear_screen();
            print_instructions();
            print_maze(maze);
            print_status(moves, elapsed_time);
            
            printf("\nIngresa tu movimiento: ");
            input = _getch();
            input = tolower(input);
            
            int new_x = player_x;
            int new_y = player_y;
            int valid_move = 0;
            
            switch (input) {
                case 'w':
                    new_y = player_y - 1;
                    valid_move = 1;
                    break;
                case 's':
                    new_y = player_y + 1;
                    valid_move = 1;
                    break;
                case 'a':
                    new_x = player_x - 1;
                    valid_move = 1;
                    break;
                case 'd':
                    new_x = player_x + 1;
                    valid_move = 1;
                    break;
                case 'r':
                    level_running = 0;
                    break;
                case 'q':
                    level_running = 0;
                    game_running = 0;
                    break;
                default:
                    continue;
            }
            
            if (valid_move) {
                // Verificar colisión usando ensamblador
                if (!asm_check_collision(maze, new_x, new_y)) {
                    // Mover jugador usando ensamblador
                    asm_move_player(maze, &player_x, &player_y, new_x, new_y);
                    moves++;
                    
                    // Verificar victoria usando ensamblador
                    if (asm_check_win(maze, player_x, player_y)) {
                        int final_time = (int)(GetTickCount() / 1000) - start_time;
                        show_victory_screen(moves, final_time);
                        level_running = 0;
                    }
                }
            }
        }
    }
    
    // Mensaje de despedida
    clear_screen();
    printf("╔══════════════════════════════════════╗\n");
    printf("║                                      ║\n");
    printf("║        ¡Gracias por jugar! 👋       ║\n");
    printf("║                                      ║\n");
    printf("╚══════════════════════════════════════╝\n");
    
    return 0;
}