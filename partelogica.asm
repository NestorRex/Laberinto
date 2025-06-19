.data
    ; Template del laberinto 12x12 mejorado
    maze_template db '#','#','#','#','#','#','#','#','#','#','#','#'
                 db '#','.','.','#','.','.','.','.','.','.','.','.','#'
                 db '#','.','#','#','.','#','#','#','.','#','#','.','#'
                 db '#','.','.','.','.','.','.','.','.','.','#','.','#'
                 db '#','#','#','.','#','#','.','#','#','.','#','.','#'
                 db '#','.','.','.','.','.','.','.','.','.','.','.','.','#'
                 db '#','.','#','#','#','#','.','#','.','#','#','#','#'
                 db '#','.','.','.','.','.','.','.','.','.','.','.','#'
                 db '#','.','#','.','#','#','#','#','#','#','.','#','#'
                 db '#','.','#','.','.','.','.','.','.','.','.','#','#'
                 db '#','.','.','.','#','.','#','#','#','.','.','.','X'
                 db '#','#','#','#','#','#','#','#','#','#','#','#'

.code

; Función para inicializar el laberinto 12x12
; Parámetros: RCX = puntero al laberinto destino
asm_init_maze PROC
    push rsi
    push rdi
    push rcx
    
    mov rsi, offset maze_template  ; Fuente
    mov rdi, rcx                   ; Destino
    mov rcx, 144                   ; 12x12 = 144 bytes
    
    ; Copiar laberinto
copy_loop:
    mov al, [rsi]
    mov [rdi], al
    inc rsi
    inc rdi
    loop copy_loop
    
    pop rcx
    pop rdi
    pop rsi
    ret
asm_init_maze ENDP

; Función para verificar colisión
; Parámetros: RCX = maze, RDX = x, R8 = y
; Retorna: RAX = 1 si hay colisión, 0 si no hay
asm_check_collision PROC
    push rbx
    
    ; Verificar límites del laberinto 12x12
    cmp rdx, 0
    jl collision_found
    cmp rdx, 11
    jg collision_found
    cmp r8, 0
    jl collision_found
    cmp r8, 11
    jg collision_found
    
    ; Calcular posición en el array
    ; offset = y * 12 + x
    mov rax, r8        ; y
    mov rbx, 12        ; ancho
    mul rbx            ; y * 12
    add rax, rdx       ; + x
    
    ; Verificar el contenido de la celda
    mov bl, [rcx + rax]
    cmp bl, '#'        ; Es una pared?
    je collision_found
    
    ; No hay colisión
    mov rax, 0
    jmp collision_end
    
collision_found:
    mov rax, 1
    
collision_end:
    pop rbx
    ret
asm_check_collision ENDP

; Nueva función para mover el jugador
; Parámetros: RCX = maze, RDX = &player_x, R8 = &player_y, R9 = new_x, [rsp+40] = new_y
asm_move_player PROC
    push rbx
    push rsi
    push rdi
    push r10
    push r11
    
    ; Obtener new_y del stack
    mov r11, [rsp + 40 + 40]  ; new_y (ajustado por los push)
    
    ; Obtener posiciones actuales
    mov esi, [rdx]     ; player_x actual
    mov edi, [r8]      ; player_y actual
    
    ; Calcular offset de posición actual
    mov rax, rdi       ; player_y actual
    mov rbx, 12
    mul rbx            ; player_y * 12
    add rax, rsi       ; + player_x
    
    ; Limpiar posición actual
    mov byte ptr [rcx + rax], '.'
    
    ; Actualizar coordenadas
    mov [rdx], r9d     ; nueva player_x
    mov [r8], r11d     ; nueva player_y
    
    ; Calcular offset de nueva posición
    mov rax, r11       ; new_y
    mov rbx, 12
    mul rbx            ; new_y * 12
    add rax, r9        ; + new_x
    
    ; Colocar jugador en nueva posición
    mov byte ptr [rcx + rax], 'P'
    
    pop r11
    pop r10
    pop rdi
    pop rsi
    pop rbx
    ret
asm_move_player ENDP

END