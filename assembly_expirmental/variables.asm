; extern ExitProcess
; section .text
;     global main
; main:
;     push 10
;     push 20
;     push 30 ; <-----
;     push 40 
;     push 50 ; rsp now is here

;     mov     ecx, [rsp + 16]
;     call ExitProcess

;     ; (4 - 2) * 8 = 16
;     ; size - its place * 8




extern ExitProcess
section .text
    global main
main:
    push 10
    push 20
    push 30 ; <----- i want to change the value of this
    push 40 
    push 50 
    mov QWORD [rsp + 16], 5

    mov     ecx, [rsp + 16]
    call ExitProcess

    ; (4 - 2) * 8 = 16
    ; size - its place * 8


