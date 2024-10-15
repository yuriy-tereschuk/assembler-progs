format ELF64

section '.text' executable

public matrix_mul_asm
matrix_mul_asm:
  push rbp
  mov rbp, rsp

  mov [matrix_a], rdi ; word
  mov [matrix_b], rsi ; word
  mov [matrix_c], rdx ; dword
  mov [rows], rcx
  mov [cols], r8

  mov rax, rcx
  shl rax, 1          ; convert count from words to bytes
  mov [ents], rax     ; count of steps to read row/col

  xor rax, rax
  xor rcx, rcx
  vxorpd ymm3, ymm3, ymm3

  _matrix_a:
    xor r8, r8
    _matrix_b:
      xor r9, r9
      _entries:
        vmovdqa ymm0, yword [rdi + r9]
        vmovdqa ymm1, yword [rsi + r9]

        vpmaddwd ymm2, ymm0, ymm1
        vphaddd ymm2, ymm2, ymm3
        vphaddd ymm2, ymm2, ymm3
;        vpmaskmovd yword [a], ymm4, ymm2
        vmovdqa yword [tmp], ymm2
        add eax, dword [tmp]
        add eax, dword [tmp + 10h]

      add r9, 20h           ; step to the next elements in matrix row/col
      cmp r9, [ents]
      jl _entries

      mov [rdx], eax        ; store prod in result matrix_c
      add rdx, 4            ; step to the next entry in matrix_c
      xor eax, eax

    add rsi, 20h            ; step to the next col in matrix_b
    inc r8
    cmp r8, [cols]
    jl _matrix_b

  mov rsi, [matrix_b]       ; restore pointer on top of matrix_b
  add rdi, 20h              ; step to the next row in matrix_a
  inc rcx
  cmp rcx, [rows]
  jl _matrix_a

  leave
  ret

section '.bss' writable align 32
  ents  rq 1
  cols  rq 1
  rows  rq 1
  matrix_a  rq  1
  matrix_b  rq  1
  matrix_c  rq  1
  align 32
  tmp       rq  4

