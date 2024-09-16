; External methods from libc used.
; To build code run fasm and link output object file with i686 gnu compiler.
; GCC gnu compiler for i386 is available in multilib package gcc-multilib-i686-linux-gnu.
; fasm arithmetic_expression.asm
; i686-linux-gnu-gcc arithmetical_expression.o -o arithmetical_expression

format ELF

section '.text' executable

extrn exit
extrn printf
extrn scanf

public main
main:
  push msg
  call printf

  add esp, 4              ; remove printf args from stack
  push expression
  push expression_fmt
  call scanf

  add esp, 8              ; remove scanf args from stack
  push expression
  push expression_msg
  call printf

  add esp, 8              ; remove printf args from stack
  call evaluate_expression

  xor eax, eax
  mov eax, [expression_result]
  push eax
  push result_msg
  call printf

  call exit
  ret

evaluate_expression:
  push eax
  push edx
  push ecx
  xor eax, eax
  xor edx, edx
  push edx
  finit
  mov esi, expression
  eval:
    cmp byte [esi], '-'
    je operator_sub
    cmp byte [esi], '+'
    je operator_add
    jmp unsigned

    operator_add:
      inc esi
    unsigned:
      call read_number
      pop edx
      add edx, eax
      push edx
      jmp next
    operator_sub:
      inc esi
      call read_number
      pop edx
      sub edx, eax
      push edx

    next:
    cmp byte [esi], 0h
  jnz eval
  pop edx
  mov [expression_result], edx
  pop ecx
  pop edx
  pop eax
  ret

read_number:
    xor ebx,ebx
    call clear_buffers
    read:
        mov al, byte [esi]    ; read some character from expresion
        mov ah, al            ; make a copy to check number or operation
        or ah, 30h            ; value of ah doesn't change if ah has decimal character
        cmp ah, al            ; check if ah remains unchanged from previous operation
        jne end_read          ; if not number faced then stop reading
        sub al, 30h           ; ascii decimal to int
        shl ebx, 4            ; adjust packet data
        xor bl, al            ; pack int in binary-coded decimal
        inc esi               ; move next characted in expression
        jmp read              ; repeat

    end_read:
    xor eax, eax
    mov dword [bcd_to_float], ebx     ; start convertion of BCD to int
    fbld tbyte [bcd_to_float]         ; FPU instructions set used 
    fistp qword [from_fpu_to_int]     ; end BCD conversion
    mov eax, dword [from_fpu_to_int]
  ret

clear_buffers:
  xor ecx, ecx
  mov cl, 9
  mov edi, bcd_to_float
  clr_bcd:
    mov byte [edi], 0h
    inc edi
  loop clr_bcd
  pxor xmm0, xmm0
  movq [from_fpu_to_int], xmm0
  ret

section '.data'

  msg db "Enter expression: ", 0h
  msglen = $ - msg
  expression_fmt db "%s", 0h
  expression_msg db "Entered: %s", 0Ah, 0h
  result_msg db "Result: %d", 0Ah, 0h
  clr_mask_q dq 0h

section '.bss' writable
  
  expression rb 100
  expression_result rd 1
  bcd_to_float rb 80        ; can be replaced by MMX register
  from_fpu_to_int rq 1      ; can be replaced by MMX register

