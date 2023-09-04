    bits 64
    extern malloc, puts, printf, fflush, abort
    global main

    section   .data
empty_str: db 0x0
int_format: db "%ld ", 0x0
data: dq 4, 8, 15, 16, 23, 42
data_length: equ ($-data) / 8

    section   .text
;;; print_int proc
print_int:
    mov rsi, rdi // copy int
    mov rdi, int_format // copy format
    xor rax, rax // null ptr
    call printf

    xor rdi, rdi // null
    call fflush

    ret

;;; p proc
p:
    mov rax, rdi // 4 ...
    and rax, 1 // нечетность
    ret

;;; add_element proc
add_element:
    push rbp // 1
    push rbx // data_len

    mov rbp, rdi // data[data_len(6)-1] // передаваемое значение
    mov rbx, rsi // ptr, предыдущий указатель от malloc

    mov rdi, 16
    call malloc // rax(+32) и rsi(rax+16) меняются
    test rax, rax // в rax новый указатель
    jz abort

    mov [rax], rbp // в rax структура, в нее кладем rbp (16)
    mov [rax + 8], rbx // и со смещением 8 кладем ptr

    pop rbx
    pop rbp

    ret

;;; m proc
m:
    test rdi, rdi // проверка ptr
    jz outm

    push rbp
    push rbx

    mov rbx, rdi // copy ptr
    mov rbp, rsi // copy print_int ptr

    mov rdi, [rdi] // 4, 8 ...
    call rsi // вызов print_int

    mov rdi, [rbx + 8] // след ptr
    mov rsi, rbp // copy print_int ptr
    call m

    pop rbx
    pop rbp

outm:
    ret

;;; f proc
f:
    mov rax, rsi // index 0

    test rdi, rdi // указатель на элемент массива
    jz outf

    push rbx
    push r12
    push r13

    mov rbx, rdi // указатель на элемент массива
    mov r12, rsi // index 0
    mov r13, rdx // функц p

    mov rdi, [rdi] // 4, 8, 15 ...
    call rdx // p
    test rax, rax // check bool
    jz z // if false

    mov rdi, [rbx] // *15 ...
    mov rsi, r12 // 0
    call add_element
    mov rsi, rax // указатель на текущий элемент
    jmp ff

z:  // после метки идет дальше вниз
    mov rsi, r12 // 0

ff:
    mov rdi, [rbx + 8] // указатель на следующий элемент
    mov rdx, r13 // p
    call f

    pop r13
    pop r12
    pop rbx

outf:
    ret

;;; main proc
main:
    push rbx // начало

    xor rax, rax // обнулить rax
    mov rbx, data_length // загрузить i = data_len = 6
adding_loop:
    mov rdi, [data - 8 + rbx * 8] // загрузить data[data_len(6)-1]
    mov rsi, rax // copy ptr, размер структуры 16
    call add_element // вызвать add_element (data_len, 1)
    dec rbx // декремент i
    jnz adding_loop // если не ноль то в начало цикла

    mov rbx, rax // copy ptr

        mov rdi, rax // copy ptr
        mov rsi, print_int // загрузить строку
        call m // вызов

        mov rdi, empty_str // загрузить пустую строку
        call puts // вызов

    mov rdx, p // загрузить функц
    xor rsi, rsi // обнулить
    mov rdi, rbx // загрузить массив
    call f // вызов

        mov rdi, rax // обнулить
        mov rsi, print_int // загрузить строку
        call m // вызов

        mov rdi, empty_str // загрузить пустую строку
        call puts // вызов

    pop rbx // завершение

    xor rax, rax
    ret
