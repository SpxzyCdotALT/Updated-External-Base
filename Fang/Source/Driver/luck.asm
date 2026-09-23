.code

; quicker than NtReadVirtualMemory & NtWriteVirtualMemory marginally.

Driver_ReadVirtualMemory PROC
	mov r10, rcx
	mov eax, 63
	syscall
	ret
Driver_ReadVirtualMemory ENDP

Driver_WriteVirtualMemory PROC
	mov r10, rcx
	mov eax, 58
	syscall
	ret
Driver_WriteVirtualMemory ENDP

END