;	jmp start
;data1 db -1
;data2 dw -25
;start:
;	mov dx, 8
	
	;mov ax, 0
	;sub ax, dx ; 这里是一个负数
	;mov dx, ax
	
	;neg指令
;	neg dx

; 无符号除法
;	mov ax, 0x400 ; 1024
;	mov bl, 0xf0 ; 240
;	div bl ; al = 0x04  div 只能用于无符号除法不然结果不对

; 有符号除法
;	mov ax, 0x400
;	mov bl, 0xf0
;	idiv bl ; AL = 0xC0,这里相当于1024 / -16 因为用的idiv  0xf0 = -16
	
	mov ax, -6002
	cwd
	mov bx, -10
	idiv bx ; 这里直接除结果太大存不下到ah和al寄存器需要用 cwd进行扩展到DX和AX进行组合 扩展后高32位在dx 低32位在ax，最后商是在AX=600 余数是在DX=-2
	
times 510 - ($-$$) db 0

db 0x55,0xaa