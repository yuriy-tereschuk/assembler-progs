;
; This program uses external function call 'printf' from libc32.
; To compile it just call fasm and you get object file in output.
; To link object file properly use gcc from i686 multilib package.
;
; fasm memory_segmentation.asm
; i686-linux-gnu-gcc memory_segmentation.o -o memory_segmentation
;

format ELF

section '.text' executable

extrn printf

public main                   ; entry point to be linked with libc
main:
  mov eax, tobeencrypted      ; argument for the 'print_data' routine
  call print_data
  
  call encrypt_data
  call print_encrypted_data
  
  call decrypt_data
  mov eax, unencrypteddata
  call print_data
  call exit

exit:
  mov eax, 1h
  mov ebx, 0h
  int 80h
  ret

print_data:
  push eax                ; place 'printf' argument on stack
  call printf
  pop eax                 ; clean stack
  ret

print_encrypted_data:
  xor ecx, ecx
  mov esi, encrypteddata
  mov ecx, tobeencrypted_len
  print:
    push ecx
    push esi
    call printf
    pop ebx
    inc esi
    pop ecx
  loop print
  mov edx, newline
  push edx
  call printf
  pop edx
  ret

section '.data' writable

  tobeencrypted db "This string has to be encrypted...", 0Ah, 0h
  tobeencrypted_len = $ - tobeencrypted
  newline db 0Ah, 0h

section '.bss' writable

  encryptionkey db 11101110b
  encrypteddata rb tobeencrypted_len

section '.extra' writable
  
  unencrypteddata rb tobeencrypted_len

section '.code' executable

decrypt_data:
  push ecx
  push eax

  xor ecx, ecx
  xor eax, eax
  mov esi, encrypteddata
  mov edi, unencrypteddata
  mov ecx, tobeencrypted_len
  mov al, [encryptionkey]
  unencrypt:
    movsb
    mov dl, byte [edi-1]
    xor dl, al
    mov [edi-1], dl
  loop unencrypt

  pop eax
  pop ecx
  ret

encrypt_data:
  push ecx
  push eax

  xor ecx, ecx
  xor eax, eax
  mov esi, tobeencrypted
  mov edi, encrypteddata
  mov ecx, tobeencrypted_len
  mov al, [encryptionkey]
  encrypt:
    movsb
    mov dl, byte [edi-1]
    xor dl, al
    mov [edi-1], dl
  loop encrypt

  pop eax
  pop ecx
  ret

