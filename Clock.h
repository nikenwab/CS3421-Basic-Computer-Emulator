
/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 5/16/2020
*  
*	Description : Clock header file for Clock module
*/

#ifndef CLOCK_H
#define CLOCK_H

#include <stdio.h>

#include "CPU.h"
#include "Memory.h"

typedef struct {
	unsigned char count;	// Count of tick pulses
} Clock;

void clockReset(Clock* clock);
void clockTick(Clock* clock, CPU* cpu, Memory* mem, iMemory* imem, Cache* cache);
void clockDump(Clock* clock);

#endif