section .text
    global asm_init_maze
    global asm_is_wall
    global asm_game_time

; Función para verificar si una celda es pared
; Parámetros (Windows x64 calling convention):
;   RCX = puntero al maze (char maze[][12])
;   EDX = x cordenada  
;   R8D = y cordenada
; Retorna: EAX = 1 si es pared '#', 0 si no es pared
asm_is_wall:
    ; Guardar registros
    push rbx
    push rdx
    
    ; Verificar límites del laberinto
    cmp edx, 0
    jl .out_of_bounds
    cmp edx, 12
    jge .out_of_bounds
    cmp r8d, 0
    jl .out_of_bounds
    cmp r8d, 12
    jge .out_of_bounds
    
    ; Calcular posición en el array: offset = y * 12 + x
    mov eax, r8d        ; y cordenada
    mov ebx, 12         ; ancho del laberinto
    imul eax, ebx       ; y * 12
    add eax, edx        ; + x
    
    ; Verificar el contenido de la celda
    movzx eax, byte [rcx + rax]  ; Cargar el byte
    cmp al, '#'         ; ¿Es una pared?
    je .is_wall
    
    ; No es pared
    mov eax, 0
    jmp .end_check

.is_wall:
    mov eax, 1
    jmp .end_check

.out_of_bounds:
    mov eax, 1          ; Fuera de límites = pared

.end_check:
    pop rdx
    pop rbx
    ret

; Función dummy para inicializar (el C maneja la inicialización)
asm_init_maze:
    ret

; Función avanzada de manejo del juego
; Parámetros:
;   RCX = puntero a matriz
;   RDX = filas (12)
;   R8 = columnas (12) 
;   R9 = movimiento (carácter)
; Retorna: 
;   EAX = 0 (movimiento exitoso), 1 (llegada a salida), -1 (colisión), 2 (salir)
asm_game_time:
    ; Prólogo estándar Windows x64
    push rbp
    mov rbp, rsp
    sub rsp, 32         ; Shadow space para Windows x64
    push r12            ; Preservar registros no volátiles
    push r13
    push r14
    push r15

    ; Parámetros Windows x64:
    mov r12, rcx        ; r12 = puntero a matriz
    mov r13, rdx        ; r13 = número de filas
    mov r14, r8         ; r14 = número de columnas
    mov r15, r9         ; r15 = movimiento

    ; Encontrar posición actual del jugador 'P'
    xor r9, r9          ; i = 0 (contador de filas)

.buscar_jugador_fila:
    cmp r9, r13         ; ¿i < filas?
    jge .jugador_no_encontrado

    xor r10, r10        ; j = 0 (contador de columnas)

.buscar_jugador_columna:
    cmp r10, r14        ; ¿j < columnas?
    jge .siguiente_fila_buscar

    ; Calcular posición: matriz[i][j] = matriz + (i * columnas + j)
    mov rax, r9         ; rax = i
    imul rax, r14       ; rax = i * columnas
    add rax, r10        ; rax = i * columnas + j

    ; Verificar si es el jugador 'P'
    cmp byte [r12 + rax], 'P'
    je .jugador_encontrado

    inc r10             ; j++
    jmp .buscar_jugador_columna

.siguiente_fila_buscar:
    inc r9              ; i++
    jmp .buscar_jugador_fila

.jugador_encontrado:
    ; r9 = fila actual del jugador
    ; r10 = columna actual del jugador
    
    ; Calcular nueva posición basada en el movimiento
    mov r11, r9         ; r11 = nueva fila
    mov rax, r10        ; rax = nueva columna

    ; Verificar movimiento y calcular nueva posición
    cmp r15b, 'q'       ; Salir del juego
    je .salir_juego
    cmp r15b, 'Q'       ; Salir del juego (mayúscula)
    je .salir_juego
    cmp r15b, 'w'       ; Arriba
    je .mover_arriba
    cmp r15b, 'W'       ; Arriba (mayúscula)
    je .mover_arriba
    cmp r15b, 's'       ; Abajo
    je .mover_abajo
    cmp r15b, 'S'       ; Abajo (mayúscula)
    je .mover_abajo
    cmp r15b, 'a'       ; Izquierda
    je .mover_izquierda
    cmp r15b, 'A'       ; Izquierda (mayúscula)
    je .mover_izquierda
    cmp r15b, 'd'       ; Derecha
    je .mover_derecha
    cmp r15b, 'D'       ; Derecha (mayúscula)
    je .mover_derecha
    jmp .movimiento_invalido

.salir_juego:
    mov rax, 2          ; Retornar 2 (salir del juego)
    jmp .fin_game

.mover_arriba:
    dec r11             ; nueva_fila--
    jmp .verificar_limites

.mover_abajo:
    inc r11             ; nueva_fila++
    jmp .verificar_limites

.mover_izquierda:
    dec rax             ; nueva_columna--
    jmp .verificar_limites

.mover_derecha:
    inc rax             ; nueva_columna++
    jmp .verificar_limites

.verificar_limites:
    ; Verificar si la nueva posición está dentro de los límites
    cmp r11, 0
    jl .movimiento_invalido
    cmp r11, r13
    jge .movimiento_invalido
    cmp rax, 0
    jl .movimiento_invalido
    cmp rax, r14
    jge .movimiento_invalido

    ; Calcular posición en memoria de la nueva celda
    push r9             ; Guardar posición actual
    push r10
    mov r9, r11         ; r9 = nueva fila
    mov r10, rax        ; r10 = nueva columna
    
    mov rax, r9         ; rax = nueva_fila
    imul rax, r14       ; rax = nueva_fila * columnas
    add rax, r10        ; rax = nueva_fila * columnas + nueva_columna

    ; Verificar contenido de la nueva celda
    movzx rcx, byte [r12 + rax]  ; rcx = contenido de nueva celda

    ; Verificar colisiones
    cmp rcx, '#'        ; ¿Es un muro?
    je .colision_muro
    cmp rcx, 'X'        ; ¿Es la salida?
    je .llegada_salida
    cmp rcx, '.'        ; ¿Es espacio libre?
    je .movimiento_valido

    ; Si no es ninguno de los anteriores, tratar como espacio libre
    jmp .movimiento_valido

.colision_muro:
    pop r10             ; Restaurar posición actual
    pop r9
    mov rax, -1         ; Retornar -1 (colisión con muro)
    jmp .fin_game

.llegada_salida:
    ; Mover jugador a la salida
    mov byte [r12 + rax], 'P'   ; Colocar 'P' en la nueva posición
    
    ; Limpiar posición anterior
    pop r10             ; Restaurar posición anterior
    pop r9
    mov rcx, r9         ; rcx = fila anterior
    imul rcx, r14       ; rcx = fila_anterior * columnas
    add rcx, r10        ; rcx = fila_anterior * columnas + columna_anterior
    mov byte [r12 + rcx], '.'   ; Colocar '.' en la posición anterior
    
    mov rax, 1          ; Retornar 1 (llegada a la salida)
    jmp .fin_game

.movimiento_valido:
    ; Realizar el movimiento
    mov byte [r12 + rax], 'P'   ; Colocar 'P' en la nueva posición
    
    ; Limpiar posición anterior
    pop r10             ; Restaurar posición anterior
    pop r9
    mov rcx, r9         ; rcx = fila anterior
    imul rcx, r14       ; rcx = fila_anterior * columnas
    add rcx, r10        ; rcx = fila_anterior * columnas + columna_anterior
    mov byte [r12 + rcx], '.'   ; Colocar '.' en la posición anterior
    
    mov rax, 0          ; Retornar 0 (movimiento exitoso)
    jmp .fin_game

.movimiento_invalido:
    mov rax, -1         ; Retornar -1 (movimiento inválido)
    jmp .fin_game

.jugador_no_encontrado:
    mov rax, -1         ; Retornar -1 (jugador no encontrado)

.fin_game:
    ; Epílogo de la función
    pop r15
    pop r14
    pop r13
    pop r12
    add rsp, 32         ; Restaurar shadow space
    pop rbp
    ret