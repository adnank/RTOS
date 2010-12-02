CFLAGS= -g -Wall
CC= gcc

RTX_all: RTX Keyboard CRT

RTX: K_Primitives.o primitives.o atomic.o cci.o init.o main.o rtx_iproc.o terminate.o userProcesses.o wallclock.o initialize.o
	gcc -o RTX K_Primitives.o primitives.o atomic.o cci.o init.o main.o rtx_iproc.o terminate.o userProcesses.o wallclock.o initialize.o

CRT: crt.c
	gcc -o CRT crt.c

Keyboard: keyboard.c
	gcc -o Keyboard keyboard.c

K_Primitives.o: rtx.h

primitives.o: rtx.h

atomic.o: rtx.h

cci.o: rtx.h

init.o: rtx.h

main.o: rtx.h

rtx_iproc.o: rtx.h

terminate.o: rtx.h

userProcesses.o: rtx.h

wallclock.o: rtx.h

initialize.o: rtx.h
