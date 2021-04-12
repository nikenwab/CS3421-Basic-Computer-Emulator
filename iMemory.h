#ifndef IMEMORY_H
#define IMEMORY_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 5/16/2020
*  
*	Description : Instruction memory header for instruction memory module
*/

typedef struct {
	uint32_t *memPtr;	// Memory Pointer
	int size;				// Memory Size
} iMemory;

void iMemCreate(iMemory* iMem, int size);
void iMemReset(iMemory* iMem);
void iMemDump(iMemory* iMem, unsigned char address, unsigned char count);
void iMemSet(iMemory* iMem, unsigned char address, char* fileName);
void iMemFetch(iMemory* iMem, unsigned int address, unsigned int *returnNum); 

#endif