start: 				;start: 标号
	mov ax, 0xb800	 ;段地址B800
	mov ds, ax	

; 这里不填默认是用ds段寄存器 所以就是0xb800开始 0xb801 0xb802
	mov byte [0x00], 0x41	;字符A的ASCII编码, byte表示字节不然不知道传输的长度， [0x00] 表示内存单元
	mov byte [0x01], 0x0c

	mov byte [0x02], 's'	;等同于 mov byte [0x02], 0x73
	mov byte [0x03], 0x04	;;黑底红字，无闪烁

	mov byte [0x04], 's'
	mov byte [0x05], 0x04

	mov byte [0x06], 'e'
	mov byte [0x07], 0x04

dest:
	mov byte [0x08], 'm'
	mov byte [0x09], 0x04

	mov byte [0x0a], 'b'
	mov byte [0x0b], 0x04

	mov byte [0x0c], 'l'
	mov byte [0x0d], 0x04

	mov byte [0x0e], 'y'
	mov byte [0x0f], 0x04

	mov byte [0x10], '.'
	mov byte [0x11], 0x04
	
;	mov bx, 0x7c00+again 寄存器跳转
	
	;jmp 0x0000:0x7c00

	;jmp 0x0000:0x7c5f ;0x7c00 + 0x5F 这个5F是看lst文件里面的物理地址 所以 = 0x7c5f 和上面功能是一样的 跳转到自己来执行
	
again:
;	jmp 0x0000:0x7c00+again ;标号 用于计算偏移 跳转
;	jmp bx ; 绝对间接近跳转，近跳转就是跳转到不太远的地方 也就是一个段的内部进行跳转，跳转的位置不是直接给出的是通过bx寄存器间接给出的 是一个绝对地址
	jmp near again ;相对偏移量进行跳转 EB 8位的相对偏移量 E9 16位表示 short 短跳转 -127-128 near 长跳转

	

	
	;times 510-0x5f db 0		;这里的0x5f 是编译出lst文件看到的偏移地址 所以按照这个填充0 一个扇区512字节 末尾2个固定字节55AA 所以510-偏移量
	
current:
		times 510-(current - start)  db 0
	
	db 0x55, 0xaa

;mov 目的操作数，			源操作数
;	寄存器，内存地址		寄存器，内存地址，立即数

; 目的操作数和源操作数 位必须相同