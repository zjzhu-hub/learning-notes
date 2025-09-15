;start:
		; 计算378除以37的结果
;		mov ax, 378 ;被除数
;		mov bl, 37	;除数
;		div bl ;AL=商（10），AH=余数（8）
	
;current:
;		times 510-(current-start) db 0
;		db 0x55,0xaa
		
		
		
; 高位运算
;start:
		; 计算65535除以10的结果
;		mov dx, 0
;		mov ax, 65535 ;被除数 ax是16位的 最大就是65535
;		mov bx, 10	;除数
;		div bx ;AX=商（6553），DX=余数（5）
		
		;div bl AL=商（6553），AH=余数（5） 这里al寄存器是8位的无法容纳6553 只能到255，所以要改成上面那行 div bx
		
		;上面这些指令是 将dx+ax的结果合并形成一个32位的二进制数就是 0000，0000，0000，0000，1111，1111，1111，1111 除以 bx 的内容，ax=商（6553），DX=余数（5）
		
	
;current:
;		times 510-(current-start) db 0
;		db 0x55,0xaa

; xor（异或）
;start:
		; 计算65535除以10的结果
;		xor dx, dx ;当这条指令执行后 dx的内容也是0，自己和自己的异或 意味着2个二进制异或bit相同 也就是全部bit都是0 用来覆盖dx的内容 所以dx=0 等价于mov dx，0，这里为什么要用异或 xor？ 因为他2个操作数都是寄存器 因此执行起来比较快 比mov dx，0 要快
;		mov ax, 65535 ;被除数 ax是16位的 最大就是65535
;		mov bx, 10	;除数
;		div bx ;AX=商（6553），DX=余数（5）

;current:
;		times 510-(current-start) db 0
;		db 0x55,0xaa
		
		
; add
start:
		;在屏幕上显示数字65535，要显示65535需要先分解每一个数位
		mov ax, 65535
		xor dx, dx		; 等价于mov dx, 0
		mov bx, 10
		div bx			; AX=商（6553），dx=余数（5）其实是存在dl低8位的
		
		add dl, 0x30 	;将数字转换为对应的字符0x30+1-9 都可以，这里为什么加dl呢？因为dx16位 要取低位的dl
		
		;要访问数据必须使用数据段寄存器ds，所以下面这一段是把ds寄存器清0
		mov cx, 0 ; 这里是吧0放到cx
		mov ds, cx ; 这里是把cx放到ds 因为 ds不接收立即数 所以需要借用cx寄存器
		
		mov [0x7c00+buffer], dl ; 吧dl的字符编码放到buffer的第一个字节。 物理地址 = DS << 4 + offset = 0 << 4 + 0x7c00 + buffer = 0x7c00 + buffer 相当于buffer的第一个字节
		
		
		xor dx, dx ; 异或操作进行清零因为dx余数已经被放到buffer的第一个字节
		div bx ; 继续进行除法运算 这个时候 AX=商(6553), DX=余数（0） BX=除数（10）所以= 6553 / 10 =  655（商） 3 = 余数
		add dl, 0x30 ;把bl结果 进行转化为字符
		mov [0x7c00+buffer+1], dl ; 吧dl内容放到buffer第二个字节 
		
		xor dx, dx
		div bx ; 继续进行除法运算 这个时候 AX=商(655), DX=余数（0） BX=除数（10）所以= 655 / 10 =  65（商） 5 = 余数
		add dl, 0x30 
		mov [0x7c00+buffer+2], dl 
		
		xor dx, dx
		div bx ; 继续进行除法运算 这个时候 AX=商(65), DX=余数（0） BX=除数（10）所以= 65 / 10 =  6（商） 5 = 余数
		add dl, 0x30 
		mov [0x7c00+buffer+3], dl 
		
		xor dx, dx
		div bx ; 继续进行除法运算 这个时候 AX=商(6), DX=余数（0） BX=除数（10）所以= 65 / 10 =  0（商） 6 = 余数
		add dl, 0x30 
		mov [0x7c00+buffer+4], dl 
		
;		mov al, [0x7c00+buffer+4] ;把buffer的第五个字节传到al寄存器
		
;		mov cx, 0xb800 ; 这个是显存地址的开始 和上面一样需要中转传输 不能直接mov ds,0 
;		mov ds,cx
		
;		mov [0x00], al ; 吧al寄存器里面的字传输的显存的偏移地址0x00开始处 这里是0xb800的偏移地址
;		mov byte [0x01], 0x2f ; 写入颜色绿色背景 亮白色 这个是立即数看不出内存大小 所以需要byte修饰
		
		; 这里因为ds段寄存器已经被改为显存的地址 所以需要改回来 改为磁盘引导扇区地址
;		mov cx, 0
;		mov ds, cx
		
;		mov al, [0x7c00+buffer+3]
		
		; 写入显存还需要把段地址设置为显存地址 0xb800
;		mov cx,  0xb800
;		mov ds, cx
		
;		mov [0x02], al
;		mov byte[0x03], 0x2f
		
		; 上面代码太复杂，需要用ds段寄存器反复切换，这里引入es扩展的段寄存器
		mov cx, 0xb800
		mov es, cx
		
		; 这里如果没有特别指明 是默认使用ds段寄存器的地址
		mov al, [0x7c00+buffer+4]
		mov [es:0x00], al ; 这里es: 叫段超越前缀，如果没有这个默认是ds:0x00 使用了es: 则使用es扩展段寄存器
		mov byte [es:0x01], 0x2f
		
		mov al, [0x7c00+buffer+3]
		mov [es:0x02], al
		mov byte [es:0x03], 0x2f
		
		mov al, [0x7c00+buffer+2]
		mov [es:0x04], al
		mov byte [es:0x05], 0x2f
		
		mov al, [0x7c00+buffer+1]
		mov [es:0x06], al
		mov byte [es:0x07], 0x2f
		
		mov al, [0x7c00+buffer]
		mov [es:0x08], al
		mov byte [es:0x09], 0x2f
		
		
again:
	jmp again ; 自己跳到自己 防止程序跑飞

buffer	db 0, 0, 0, 0, 0 ;开辟5个字节的空间 这里位指令的冒号一般省略，这里buffer其实是表示第一个字节的地址 0x7c00+buffer 
		
current:
		times 510-(current-start) db 0
		db 0x55,0xaa