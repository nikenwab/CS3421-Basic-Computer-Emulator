#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "Cache.h"
#include "Memory.h"
#include "iMemory.h"


/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 5/16/2020
*  
*	Description : CPU header file for CPU module
*/

typedef struct {
	
	unsigned char 	reg[8];			// CPU Registers
	unsigned int	TC;				// Tick Counter
	unsigned int 	PC;				// Program Counter
	int				destination;	// Instruction Destination
	int				source;			// Instruction Source
	int				target;			// Instruction Target
	int				immediate;		// Instruction Immediate
	int				cpuWork;		// CPU Work counter
	
	// Enumeration for CPU states
	enum 			cpuState {CPU_IDLE, CPU_FETCH, CPU_WAIT, CPU_DECODE, CPU_MEMREQ, CPU_HALT, CPU_WORK} State;
	
	// Enumeration for CPU Instructions
	enum			cpuInstr {ADD = 0x00, ADDI = 0x01, MUL = 0x02, INV = 0x03, BRANCH = 0x04, LW = 0x05, SW = 0x06, HALT = 0x07} instruction; 
	
} CPU;

void cpuReset(CPU* cpu);
void cpuSetReg(CPU* cpu, char reg[], unsigned char value);
void cpuDump(CPU* cpu);

void cpuStartTick(CPU* cpu);
void cpuDoCycleWork(CPU* cpu, Memory* mem, iMemory* iMem, Cache* cache);
bool cpuIsMoreCycleWorkNeeded(CPU* cpu);

#endif