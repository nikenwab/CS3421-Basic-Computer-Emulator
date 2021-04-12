
#include "Clock.h"


/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 5/16/2020
*  
*	Description : Clock module to handle system ticks
*/

// Resets clock counter to zero
void clockReset(Clock* clock) {
	clock->count = 0;
}

// Adds input number to clock counter
void clockTick(Clock* clock, CPU* cpu, Memory* mem, iMemory* imem, Cache* cache) {

	// Initializes work variable for this cycle
	bool workToDo = true;
	
	// Sends start tick to Memory and CPU
	memStartTick(mem);
	cpuStartTick(cpu);
	
	// If there is more work to do then
	while (workToDo) {
		
		//Give Mem and CPU a chance to work during this cycle
		cacheDoCycleWork(cache, mem);
		cpuDoCycleWork(cpu, mem, imem, cache);
		memDoCycleWork(mem);
		
		// Determine if work is done for this cycle
		workToDo = cpuIsMoreCycleWorkNeeded(cpu) 	||
					memIsMoreCycleWorkNeeded(mem)	||
					cacheIsMoreCycleWorkNeeded(cache);
	}
	
	// Increment clock timer
	clock->count++;
	
}

// Prints info on clock counter
void clockDump(Clock* clock) {
	printf("Clock: %d\n", clock->count);
}