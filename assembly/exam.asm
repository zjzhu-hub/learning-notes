mov ax, 0x30	;将立即数传送到AX寄存器
mov dx, 0xc0		
add ax, dx

times 502 db 0 ; db 位指令 重复输出位指令502 502个字节 因为一个扇区是512字节 不然不认

db 0x55 ;末尾必须是 55 和 aa 不然主引导扇区不认
db 0xAA 