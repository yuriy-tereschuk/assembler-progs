format ELF

section '.data' writable align 32
  align 4
  mem_a rd 1
  mem_b rd 1
  mem_c rd 1
  array_size rd 1
  result rd 1
  align 32
  vector_prod rd 8

section '.text' executable

public vector_add_asm
public dot_product_asm

vector_add_asm:
  push ebp
  mov ebp, esp

  mov edx, [ebp + 8]    ; array size
  mov eax, [ebp + 12]   ; stack pointer to the mem_a
  mov [mem_a], eax
  mov eax, [ebp + 16]   ; stack pointer to the mem_b
  mov [mem_b], eax
  mov eax, [ebp + 20]   ; stack pointer to the mem_c
  mov [mem_c], eax

  imul edx, 4           ; adjust array size to handle 4 bytes for each enty
  xor ecx, ecx

  vector_loop:          ; vector addition

    mov eax, [mem_a]
    vmovdqa ymm0, yword [eax + ecx]
    mov eax, [mem_b]
    vmovdqa ymm1, yword [eax + ecx]
    vaddps ymm2, ymm0, ymm1
    mov eax, [mem_c]
    vmovaps yword [eax + ecx], ymm2

  add ecx, 20h
  cmp ecx, edx
  jl vector_loop

  leave                 ; restore base pointer
  ret

dot_product_asm:
  push ebp
  mov ebp, esp

  mov edx, [ebp + 8]    ; array size
  mov eax, [ebp + 12]   ; stack pointer to the mem_a
  mov [mem_a], eax
  mov eax, [ebp + 16]   ; stack pointer to the mem_b
  mov [mem_b], eax

  imul edx, 4           ; adjust array size to handle 4 bytes for each enty
  xor ecx, ecx
  vxorps ymm2, ymm2, ymm2

  prod_loop:            ; vector multiply and addition

    mov eax, [mem_a]
    vmovdqa ymm0, yword [eax + ecx]
    mov eax, [mem_b]
    vmovdqa ymm1, yword [eax + ecx]
    vfmadd231ps ymm2, ymm0, ymm1

  add ecx, 20h
  cmp ecx, edx
  jl prod_loop

  vmovapd yword [vector_prod], ymm2
  mov ecx, vector_prod
  xor edx, edx

  fninit
  fld  dword [ecx]
  fadd dword [ecx+4]
  fadd dword [ecx+8]
  fadd dword [ecx+12]
  fadd dword [ecx+16]
  fadd dword [ecx+20] 
  fadd dword [ecx+24]
  fadd dword [ecx+28]

  leave                 ; restore base pointer
  ret

