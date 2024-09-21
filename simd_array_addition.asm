format ELF

section '.text' executable

public asm_vector_add

asm_vector_add:

  push ebx
  mov ebx, esp

  mov ecx, [ebx + vector_len]
  mov edi, [ebx + vector_c]
  mov eax, [ebx + vector_a]
  mov edx, [ebx + vector_b]

  loop_vector:
    movdqa xmm0, dqword [eax]
    movdqa xmm1, dqword [edx]
    paddd xmm0, xmm1
    movapd dqword [edi], xmm0

    add eax, 10h
    add edx, 10h
    add edi, 10h

  loop loop_vector

  pop ebx

  ret

section '.data' writable

  return_main = dword 0h
  vector_a    = dword 8h
  vector_b    = dword 0Ch
  vector_c    = dword 10h
  vector_len  = dword 14h
