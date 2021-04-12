# Compilation specifics
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O1 
#-pedantic
LDFLAGS = -lm

default : cs3421_emul

# To build just the Dungeon executable, type "make Dungeon"
cs3421_emul: cs3421_emul.c CPU.o Memory.o Cache.o Clock.o iMemory.o
	${CC} ${CFLAGS} cs3421_emul.c CPU.o Memory.o Cache.o Clock.o iMemory.o -o cs3421_emul ${LDFLAGS}
	
#TestClock : TestClock.c Clock.o 
#	${CC} ${CFLAGS} TestClock.c Clock.o -o TestClock ${LDFLAGS}
	
#TestCPU : TestCPU.c CPU.o iMemory.o
#	${CC} ${CFLAGS} TestCPU.c CPU.o iMemory.o -o TestCPU ${LDFLAGS}
	
#TestMemory : TestMemory.c Memory.o CPU.h
#	${CC} ${CFLAGS} TestMemory.c Memory.o CPU.h -o TestMemory ${LDFLAGS}
	
#TestiMemory : TestiMemory.c iMemory.o CPU.o
#	${CC} ${CFLAGS} TestiMemory.c iMemory.o CPU.o -o TestiMemory ${LDFLAGS}
	
#TestMemCPU : TestMemCPU.c Memory.o CPU.o
#	${CC} ${CFLAGS} TestMemCPU.c Memory.o CPU.o -o TestMemCPU ${LDFLAGS}

run: cs3421_emul
	./cs3421_emul