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
  xor eax, eax
  xor edx, edx
  push edx
  mov esi, expression
  eval:
    cmp byte [esi], '+'
    je operator_add
    cmp byte [esi], '-'
    je operator_sub

    mov al, byte [esi]
    sub al, 30h      ; ascii to int
    pop edx
    add edx, eax
    push edx
    inc esi
    jmp next

    operator_add:
      inc esi
      mov al, byte [esi]
      sub eax, 30h
      pop edx
      add edx, eax
      push edx
      inc esi
      jmp next
    operator_sub:
      inc esi
      mov al, byte [esi]
      sub eax, 30h
      pop edx
      sub edx, eax
      push edx
      inc esi
      jmp next

    next:
    cmp byte [esi], 0h
  jnz eval
  pop edx
  mov [expression_result], edx
  pop edx
  pop eax
  ret



section '.data'

  msg db "Enter expression: ", 0h
  msglen = $ - msg
  expression_fmt db "%s", 0h
  expression_msg db "Entered: %s", 0Ah, 0h
  result_msg db "Result: %d", 0Ah, 0h

section '.bss' writable
  
  expression rb 100
  expression_result rd 1


