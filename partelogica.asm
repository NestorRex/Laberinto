
.data
    ; Template básico del laberinto 10x10
    maze_template db '#','#','#','#','#','#','#','#','#','#'
                 db '#','.','.','#','.','.','.','.','.','.','#'
                 db '#','.','#','#','.','#','#','#','.','#'
                 db '#','.','.','.','.','.','.','.','.','.','#'
                 db '#','#','#','.','#','#','.','#','#','#'
                 db '#','.','.','.','.','.','.','.','.','#'
                 db '#','.','#','#','#','#','.','#','.','#'
                 db '#','.','.','.','.','.','.','.','.','.','#'
                 db '#','.','#','.','#','#','#','#','.','X'
                 db '#','#','#','#','#','#','#','#','#','#'

.code

; Función para inicializar el laberinto
; Parámetros: RCX = puntero al laberinto destino (maze[][10])
asm_init_maze PROC
    push rsi
    push rdi
    push rcx
    
    mov rsi, offset maze_template  ; Fuente
    mov rdi, rcx                   ; Destino
    mov rcx, 100                   ; 10x10 = 100 bytes
    
    ; Copiar laberinto byte por byte
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
    
    ; Verificar límites del laberinto
    cmp rdx, 0
    jl collision_found
    cmp rdx, 9
    jg collision_found
    cmp r8, 0
    jl collision_found
    cmp r8, 9
    jg collision_found
    
    ; Calcular posición en el array
    ; offset = y * 10 + x
    mov rax, r8        ; y
    mov rbx, 10        ; ancho
    mul rbx            ; y * 10
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

END