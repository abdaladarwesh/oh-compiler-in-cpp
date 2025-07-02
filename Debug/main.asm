extern ExitProcess
section .text
    global main
main:
    push 6
    push 10
    mov QWORD [rsp + 8], 87
    mov ecx, [rsp + 8]
    call ExitProcess
    mov     ecx, 0
    call ExitProcess
