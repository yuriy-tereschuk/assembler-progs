;
; Allocate memory for two arrays, allign memory to 32 bytes.
; Peform set of operations on memory.
;
format ELF

section '.data'
  mem_len equ 16            ; buffer len
  bad_alignment_msg db "Bad alignment! SRC:%x | DST: %x", 0Ah, 0

section '.text' executable

extrn printf
extrn posix_memalign
extrn free
extrn exit

public main

main:
  call allocate_and_align_buffers
  xor eax, eax
  call check_mem_align
  cmp eax, 00h
  jne skip_simd_compute

  call init_src_buffer

  xor eax, eax
  call simd_compute

  skip_simd_compute:
  call free_buffers
  push 0Ah
  call exit
  add esp, 4
  ret

init_src_buffer:   ; init buffers with values
  push edi
  push ecx
  mov edi, [mem_src]
  mov ecx, mem_len
  init:
    mov [edi], ecx
    inc edi
  loop init
  pop ecx
  pop edi
  ret

simd_compute:     ; copy from source into destination
  xor ecx, ecx

  mov edi, [mem_dst]
  mov esi, [mem_src]

  movaps xmm0, [esi]    ; no issues if the size of memory block behind pointer 
  movaps [edi], xmm0    ; the same as size of register, otherwise pointer bocome
                        ; invalid and operation free doesn't work for pointer

  ret

check_mem_align:  ; ensure memory aligned in 16 byte boundary
  push ebx
  push edx
  mov ebx, 10h

  xor edx, edx
  mov eax, [mem_src]
  div ebx
  
  cmp edx, 0h
  jne bad_alignment

  xor edx, edx
  mov eax, [mem_dst]
  div ebx

  cmp edx, 0h
  jne bad_alignment

  xor eax, eax
  jmp ok_aligned

  bad_alignment:
  push [mem_dst]
  push [mem_src]
  push bad_alignment_msg
  call printf
  mov eax, 1
  add esp, 12
    
  ok_aligned:
  pop edx
  pop ebx
  ret

allocate_and_align_buffers: ; allocate alligned memory for tree buffers
  push ebp

  push mem_len
  push 10h                  ; align at 16 bytes boundary
  push mem_src
  call posix_memalign

  add esp, 4
  push mem_dst
  call posix_memalign

  add esp, 12               ; remove buffers addresses from stack
  pop ebp
  ret

free_buffers:
  push [mem_src]
  call free
  push [mem_dst]
  call free
  add esp, 8h
  ret

section '.bss' writable
  mem_src rd 1            ; source 
  mem_dst rd 1            ; destination
