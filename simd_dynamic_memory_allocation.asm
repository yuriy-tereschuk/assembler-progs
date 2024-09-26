;
; Allocate memory for two arrays, call posix_memalign for 16 bytes.
; Copy buffer_1 to buffer_2 using SSE instructions.
;
; Compile: fasm <name>.asm <name>.o
; Linkage: i686-linux-gnu-gcc <name>.o -o <name>
;
format ELF

section '.data'
  mem_len equ 100000000                   ; buffer len in bytes
  half_mem_len equ 50000000              ; somewhere at the middle of mem_len...
  align_boundary equ 10h           ; memory alignment
  bad_alignment_msg     db "Bad alignment! SRC:%x | DST: %x", 0Ah, 0
  test_buffers_msg      db "%s Buffer values: ", 0
  test_buffers_pattern  db " %d; %d; %d; ", 0
  new_line              db "", 0Ah, 0
  src_prefix            db "[SRC]", 0
  dst_prefix            db "[DST]", 0

section '.text' executable

extrn printf
extrn malloc
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

  push esi
  mov esi, [mem_src]
  call init_src_buffer
  pop esi

  xor eax, eax
  call simd_compute

  push esi
  mov eax, src_prefix
  mov esi, [mem_src]
  call test_buffers
  mov eax, dst_prefix
  mov esi, [mem_dst]
  call test_buffers
  pop esi

  skip_simd_compute:
  call free_buffers
  push 0Ah
  call exit
  add esp, 4
  ret

test_buffers:
  
  push eax
  push test_buffers_msg
  call printf
  add esp, 8h

  push dword [esi + 8]
  push dword [esi + 4]
  push dword [esi]
  push test_buffers_pattern
  call printf
  add esp, 10h
  
  push dword [esi + half_mem_len + 4]
  push dword [esi + half_mem_len]
  push dword [esi + half_mem_len - 4]
  push test_buffers_pattern
  call printf
  add esp, 10h

  push dword [esi + mem_len - 4]
  push dword [esi + mem_len - 8]
  push dword [esi + mem_len - 12]
  push test_buffers_pattern
  call printf
  add esp, 10h

  push new_line
  call printf
  add esp, 4

  ret

init_src_buffer:   ; init buffers with values
  push ecx
  mov ecx, mem_len
  init:
    mov [esi], ecx
    sub ecx, 4h
    add esi, 4h
  cmp ecx, 0h
  jnl init
  pop ecx
  ret

simd_compute:     ; copy from source into destination
  push ecx
  xor ecx, ecx

  mov ecx, mem_len
  mov edi, [mem_dst]
  mov esi, [mem_src]

  copy_mem:
    movaps xmm0, [esi]
    movaps [edi], xmm0
    add esi, 10h
    add edi, 10h
    sub ecx, 10h
  cmp ecx, 0h
  jnle copy_mem

  pop ecx

  ret

check_mem_align:  ; ensure memory aligned in 16 byte boundary
  push ebx
  push edx
  mov ebx, align_boundary

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
  push align_boundary       ; align at 32 bytes boundary
  push mem_src
  call posix_memalign
;  call malloc
;  mov [mem_src], eax

  add esp, 4
  push mem_dst
  call posix_memalign
;  call malloc
;  mov [mem_dst], eax

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
