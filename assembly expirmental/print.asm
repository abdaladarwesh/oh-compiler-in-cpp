extern GetStdHandle
extern WriteConsoleA
extern ExitProcess
section .data
    msg db "anywoeed", 0
    msg_len equ $ - msg
    written dd 0
section .text
    global _main

_main:
    sub rsp, 40
    mov ecx, -11
    call GetStdHandle
    mov rcx, rax
    lea rdx, [rel msg]
    mov r8d, msg_len
    lea r9,[rel written]
    xor rax, rax
    call WriteConsoleA

    mov ecx, 15
    call ExitProcess
