# Makefile for snaktionary

COMPILER_TYPE= gnu

CC= gcc

PROG= proj
SRCS= proj.c game.c stbi_png.c vbe.c video_gr.c timer.c kbd.c kbd_asm.S mouse.c rtc.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS} 
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

CPPFLAGS += -D PROJ

.include <bsd.gcc.mk>
.include <bsd.prog.mk>
