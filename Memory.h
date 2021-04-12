#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 5/16/2020
*  
*	Description : Memory header file for Memory module
*/

typedef struct {
	
	unsigned char 	*memPtr;		// Memory pointer
	int 			size;			// Memory Size
	
	bool 			*memDone;		// Memory done bool
	unsigned char 	*memDataPtr;	// Memory Data Pointer
	unsigned char 	memAddress;		// Memory address placeholder
	unsigned char 	memCount;		// Memory count placeholder
	
	unsigned char 	memWait;		// Memory wait counter
	bool			memWaitDone;	// Memory Wait done boolean
	
	// Enumeration for Memory States
	enum			memState {MEM_IDLE, MEM_WAIT, MEM_FETCH, MEM_STORE, MOVE_DATA} State;
	
} Memory;

// Memory tools
void memCreate(Memory* mem, int size);
void memReset(Memory* mem);
void memDump(Memory* mem, unsigned char address, unsigned char count);
void memSet(Memory* mem, unsigned char address, unsigned char count, unsigned char value, ...);

// Memory to CPU
void memStartFetch(Memory* mem, unsigned int address, unsigned int count, unsigned char *dataPtr, bool *memDonePtr);
void memStartStore(Memory* mem, unsigned int address, unsigned int count, unsigned char *dataPtr, bool *memDonePtr);

// Memory to Cache
void memFetch(Memory* mem, unsigned int address, unsigned char count, unsigned char *answerPtr, bool *fetchDonePtr);
void memStore(Memory* mem, unsigned int address, unsigned char count, unsigned char *answerPtr, bool *fetchDonePtr);

// Memory work
void memStartTick(Memory* mem);
void memDoCycleWork(Memory* mem);
bool memIsMoreCycleWorkNeeded(Memory* mem);

#endif