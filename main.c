#include <stdio.h>      // Entrada y salida estándar (printf, etc.)
#include <stdlib.h>     // Funciones del sistema (system, etc.)
#include <conio.h>      // Para _getch() y _kbhit() — captura de teclado sin Enter
#include <windows.h>    // Para manipular la consola (cursor, sleep)
#include <ctype.h>      // Para tolower(), convierte caracteres a minúscula
#include <time.h>       // Para medir el tiempo del juego

// Constantes del juego
#define MAZE_SIZE 12    // Tamaño del laberinto (12x12)
#define WALL '#'        // Representa una pared
#define PATH '.'        // Representa un camino
#define PLAYER 'P'      // Jugador
#define EXIT 'X'        // Meta o salida

// Estructura para guardar la posición del jugador
typedef struct {
    int x;
    int y;
} Position;

// Funciones ensamblador externas (deben estar en NASM)
extern void asm_init_maze(char maze[][MAZE_SIZE]);
extern int asm_is_wall(char maze[][MAZE_SIZE], int x, int y);
extern int asm_game_time(char maze[][MAZE_SIZE], int rows, int cols, char movement);

// Laberinto predefinido (matriz de 12x12)
char maze_template[MAZE_SIZE][MAZE_SIZE] = {
    {'#','#','#','#','#','#','#','#','#','#','#','#'},
    {'#','P','.','.','.','.','.','.','.','.','.','#'},
    {'#','.','#','#','.','#','#','#','.','#','#','#'},
    {'#','.','.','.','.','.','.','.','.','.','.','.'},
    {'#','#','#','.','#','#','.','#','#','.','#','.'},
    {'#','.','.','.','.','.','.','.','.','.','.','#'},
    {'#','.','#','#','#','#','.','#','.','#','#','#'},
    {'#','.','.','.','.','.','.','.','.','.','.','.'},
    {'#','.','#','.','#','#','#','#','#','#','.','#'},
    {'#','.','#','.','.','.','.','.','.','.','.','#'},
    {'#','.','.','.','.','#','.','#','#','#','X','#'},
    {'#','#','#','#','#','#','#','#','#','#','#','#'}
};

// Función para encontrar la posición del jugador en el laberinto
Position find_player(char maze[][MAZE_SIZE]) {
    Position pos = {-1, -1};
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            if (maze[i][j] == PLAYER) {
                pos.x = j;
                pos.y = i;
                return pos;
            }
        }
    }
    return pos;
}

// Oculta el cursor de la consola y limpia pantalla
void configurar_consola() {
    system("cls");
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// Muestra las instrucciones del juego
void mostrar_instrucciones() {
    printf("+======================================+\n");
    printf("|          LABERINTO GAME              |\n");
    printf("|                                      |\n");
    printf("+======================================+\n\n");
    printf("Controles:\n");
    printf("  W - Arriba    A - Izquierda\n");
    printf("  S - Abajo     D - Derecha\n");
    printf("  R - Reiniciar Q - Salir\n");
}

// Dibuja el laberinto actual en pantalla
void dibujar_laberinto(char maze[][MAZE_SIZE]) {
    system("cls");
    mostrar_instrucciones();
    
    printf("+");
    for (int i = 0; i < MAZE_SIZE * 2 + 1; i++) printf("-");
    printf("+\n");

    for (int i = 0; i < MAZE_SIZE; i++) {
        printf("| ");
        for (int j = 0; j < MAZE_SIZE; j++) {
            printf("%c ", maze[i][j]);
        }
        printf("|\n");
    }

    printf("+");
    for (int i = 0; i < MAZE_SIZE * 2 + 1; i++) printf("-");
    printf("+\n");
}

// Copia el laberinto original al laberinto actual (reinicio)
void inicializar_laberinto(char maze[][MAZE_SIZE]) {
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            maze[i][j] = maze_template[i][j];
        }
    }
    asm_init_maze(maze);  // Llama a función de ensamblador (aunque esté vacía)
}

// Muestra el estado actual del juego (movimientos, tiempo y modo)
void mostrar_estado(int movimientos, int tiempo, int modo) {
    printf("\nMovimientos: %d | Tiempo: %d segundos\n", movimientos, tiempo);
    printf("=======================================\n");
}

// Pantalla final al ganar el juego
void pantalla_victoria(int movimientos, int tiempo, int modo) {
    system("cls");
    printf("+======================================+\n");
    printf("|              ¡VICTORIA!              |\n");
    printf("+======================================+\n");
    printf("|  Felicidades! Has completado el      |\n");
    printf("|  laberinto en:                       |\n");
    printf("|  - Movimientos: %3d                  |\n", movimientos);
    printf("|  - Tiempo: %3d segundos              |\n", tiempo);
    printf("|                                      |\n", modo ? "Assembly" : "C");
    printf("+======================================+\n");
    printf("\nPresiona cualquier tecla para continuar...");
    _getch(); // Espera entrada del usuario
}

// Función principal
int main() {
    char maze[MAZE_SIZE][MAZE_SIZE];  // Laberinto actual
    int modo_asm = 0;                 // 0 = C, 1 = ASM
    int jugando = 1;                  // Controla si sigue el juego

    configurar_consola();

    printf("Iniciando juego del laberinto...\n");
    printf("Presiona cualquier tecla para comenzar...\n");
    _getch();  // Espera que el jugador presione una tecla

    while (jugando) {
        inicializar_laberinto(maze);        // Reinicia el laberinto
        int movimientos = 0;
        time_t inicio = time(NULL);         // Guarda el tiempo de inicio
        int nivel_activo = 1;

        dibujar_laberinto(maze);            // Muestra el laberinto
        mostrar_estado(movimientos, 0, modo_asm);

        while (nivel_activo) {
            if (_kbhit()) {                 // Si se presiona una tecla
                char entrada = tolower(_getch()); // Guarda la tecla en minúscula
                time_t ahora = time(NULL);
                int tiempo = (int)difftime(ahora, inicio);

                switch (entrada) {
                    case 'q': // Salir
                        nivel_activo = 0;
                        jugando = 0;
                        break;
                    case 'r': // Reiniciar
                        nivel_activo = 0;
                        break;
                    case 'm': 
                        modo_asm = !modo_asm;
                        dibujar_laberinto(maze);
                        mostrar_estado(movimientos, tiempo, modo_asm);
                        break;
                    case 'w':
                    case 'a':
                    case 's':
                    case 'd':
                        if (modo_asm) {
                            // Movimiento con lógica en ensamblador
                            int resultado = asm_game_time(maze, MAZE_SIZE, MAZE_SIZE, entrada);
                            if (resultado == 1) {
                                pantalla_victoria(movimientos, tiempo, modo_asm);
                                nivel_activo = 0;
                            } else if (resultado == 0) {
                                movimientos++;
                                dibujar_laberinto(maze);
                                mostrar_estado(movimientos, tiempo, modo_asm);
                            }
                        } else {
                            // Movimiento con lógica en C
                            Position pos = find_player(maze);
                            int nx = pos.x, ny = pos.y;

                            switch (entrada) {
                                case 'w': ny--; break;
                                case 's': ny++; break;
                                case 'a': nx--; break;
                                case 'd': nx++; break;
                            }

                            // Verifica si no es pared
                            if (maze[ny][nx] != WALL) {
                                maze[pos.y][pos.x] = PATH; // Borra posición anterior
                                if (maze[ny][nx] == EXIT) {
                                    pantalla_victoria(movimientos + 1, tiempo, modo_asm);
                                    nivel_activo = 0;
                                } else {
                                    maze[ny][nx] = PLAYER; // Mueve al jugador
                                    movimientos++;
                                    dibujar_laberinto(maze);
                                    mostrar_estado(movimientos, tiempo, modo_asm);
                                }
                            }
                        }
                        break;
                }
            }
            Sleep(50);  // Espera para no saturar la CPU
        }
    }

    system("cls");
    printf("Gracias por jugar al Laberinto!\n");
    return 0;
}
