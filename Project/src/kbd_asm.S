#include "i8042.h"
.file "game.c"
.global _kbd_asm_handler

.text
_kbd_asm_handler:
	xor %eax, %eax
	inb $STAT_REG, %al
	movzx %al, %eax
	movl %eax, %ebx
	movl $3, %ecx
cicle:
	rcl $1, %ebx
	jc end
	loop cicle
	test $OBF, %eax
	jz end
	xor %eax, %eax
	inb $OUT_BUF, %al
	movzx %al, %eax
	movl %eax, _g_scancode
end:
	ret
