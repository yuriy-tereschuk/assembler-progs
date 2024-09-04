format ELF

section '.text' executable

define SYS_exit  1
define SYS_write 4
define SYS_read  3

define STDIN     0
define STDOUT    1
define STDERR    2
define exit_success 0

public _start

_start:

  mov eax, SYS_write
  mov ebx, STDOUT
  mov ecx, operation_message
  mov edx, operation_message_len
  int 80h

  ; operation
  read_again:         ; if buffer contains leading new line character
  mov eax, SYS_read
  mov ebx, STDIN
  mov ecx, operation
  mov edx, 1
  int 80h
  cmp byte [ecx], 0Ah ; check new line in output buffer
  je read_again       ; reped read if such happens

  mov al, [operation]
  sub al, 30h
  mov [operation], al

  call check_operation

  mov eax, SYS_write
  mov ebx, STDOUT
  mov ecx, request_values_msg
  mov edx, request_values_msg_len
  int 80h

  call ask_value
  push dword [ecx]

  call ask_value
  push dword [ecx]
  
  call ask_value
  push dword [ecx]

  pop eax
  pop ebx
  pop ecx

  sub al, 30h
  sub bl, 30h
  sub cl, 30h

  cmp byte [operation], 1h
  je to_max
  jl to_min

  to_max:
    call maxvalue
    jmp to_result
  to_min:
    call minvalue

  to_result:
  add [externum], 30h
;; Pring result
  mov eax, SYS_write
  mov ebx, STDOUT
  mov ecx, externum
  mov edx, 1
  int 80h
  
  call exit

exit:
    mov eax, SYS_exit
    mov ebx, exit_success
    int 80h
    ret

check_operation:
    mov dl, [operation]
    cmp edx, 0h
    je to_ret
    cmp edx, 1h
    je to_ret
    ; Invalid operation selected
    call invalid_operation
    ; Valid operation selected
    to_ret:
    ret

invalid_operation:
  mov eax, SYS_write
  mov ebx, STDOUT
  mov ecx, invalid_operation_msg
  mov edx, invalid_operation_msg_len
  int 80h
  call exit

ask_value:
  ask_again:
    mov eax, SYS_read
    mov ebx, STDIN
    mov ecx, value
    mov edx, 1
    int 80h
    cmp byte [ecx], 0Ah
    jz ask_again
    ret

maxvalue:
    mov [externum], al
    cmp [externum], bl
    jg @f
    mov [externum], bl
    @@:
    cmp [externum], cl
    jg @f
    mov [externum], cl
    @@:
    ret

minvalue:
    mov [externum], al
    cmp [externum], bl
    jl @f
    mov [externum], bl
    @@:
    cmp [externum], cl
    jl @f
    mov [externum], cl
    @@:
    ret

section '.data' writable
  invalid_operation_msg db "Ivaild operation selected, acceptable min:0 or max:1", 0xA, 0
  invalid_operation_msg_len = $ - invalid_operation_msg
  operation_message db "Select externum operation, for min:0, for max:1", 0xA, 0
  operation_message_len = $ - operation_message
  request_values_msg db "Input three values in range of 0-9:", 0xA, 0
  request_values_msg_len = $ - request_values_msg
  result_message db "Externum value:", 0xA, 0
  result_message_len = $ - result_message

section '.bss' writable
  externum  rb 1
  operation rb 1
  value     rb 1
  value_a   rd 1
  value_b   rd 1
  value_c   rd 1

