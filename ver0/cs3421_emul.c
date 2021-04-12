#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "CPU.h"
#include "Clock.h"
#include "Memory.h"
#include "iMemory.h"
#include "Cache.h"

/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 5/16/2020
*  
*	Description : Driver that instantiates CPU, Memory, and Clock modules 
*				to emulate a computer system.
*/

int main(int argc, char** argv) {
	// Initialize variables and devices
	char			*charPtr;
	char 			cmd[128];
	char			reg[128];
	int 			num1 = 00;
	unsigned char 	num2 = 0x00;
	unsigned char 	num3 = 0x00;
	CPU 			cpu;
	Memory			mem;
	iMemory			imem;
	Clock 			clock;
	Cache			cache;
	
	// Check for correct verbose
	if (argc < 2) {
		printf("\nUsage: cs3421_emul <data_file.txt>\n\n");
		return 1;
	}
	
	// Attempt to open file specified
	FILE* in = fopen(argv[1], "r");
	if (in == NULL) {
		printf("\nERROR: File \"%s\" could not be opened!\n\n", argv[1]);
		return 1;
	}
	
	// Loop through file and execute commands
	while (fscanf(in, "%64s", cmd) == 1) {
		
		// Clock command
		if (strcmp(cmd, "clock") == 0) {
			
			// Read command
			fscanf(in, "%64s", cmd);
			
			// Reset clock
			if (strcmp(cmd, "reset") == 0) {
				
				// Call reset
				clockReset(&clock);
			
			// Dump clock
			} else if (strcmp(cmd, "dump") == 0) {
				clockDump(&clock);
			
			// Tick clock
			} else if (strcmp(cmd, "tick") == 0) {
				
				// Read number of ticks
				fscanf(in, "%64s", cmd);
				num1 = strtol(cmd, &charPtr, 10);
				
				
				// Make CPU do work based on clock ticks
				for (int i = 0; i < num1; i++) {
					
					// Tick clock
					clockTick(&clock, &cpu, &mem, &imem, &cache);
					//printf("-----Tick %d-----\n", clock.count);
					
				}
			}
		
		// Memory command
		} else if (strcmp(cmd, "memory") == 0) {
			
			// Read command
			fscanf(in, "%64s", cmd);
			
			// Create Memory
			if (strcmp(cmd, "create") == 0) {
				
				// Read size of memory to create
				fscanf(in, "%64s", cmd);
				num1 = strtol(cmd, &charPtr, 16);
				
				// Call create
				memCreate(&mem, num1);
			
			// Reset Memory
			} else if (strcmp(cmd, "reset") == 0) {
				
				// Call reset
				memReset(&mem);
			
			// Dump Memory
			} else if (strcmp(cmd, "dump") == 0) {
				
				// Get dump start address
				fscanf(in, "%64s", cmd);
				num1 = strtol(cmd, &charPtr, 16);
				
				// Get num of dump bytes
				fscanf(in, "%64s", cmd);
				num2 = strtol(cmd, &charPtr, 16);
				
				// Call dump
				memDump(&mem, num1, num2);
			
			// Set Memory
			} else if (strcmp(cmd, "set") == 0) {
				
				// Get start address
				fscanf(in, "%64s", cmd);
				num1 = strtol(cmd, &charPtr, 16);
				
				// Get num of bytes to write
				fscanf(in, "%64s", cmd);
				num2 = strtol(cmd, &charPtr, 16);
				
				// Get first value to write
				fscanf(in, "%64s", cmd);
				num3 = strtol(cmd, &charPtr, 16);
				
				// Loop to set the rest of values
				for (int i = 0; i < num2; i++) {
					memSet(&mem, num1, 1, num3);
					num1++;
					
					// Stop scanning if done
					if (i+1 != num2) {
						fscanf(in, "%64s", cmd);
						num3 = strtol(cmd, &charPtr, 16);
					}
				}
			}
		
		// CPU command
		} else if (strcmp(cmd, "cpu") == 0) {
			
			// Read command
			fscanf(in, "%64s", cmd);
			
			// Reset CPU
			if (strcmp(cmd, "reset") == 0) {
				
				// Call reset
				cpuReset(&cpu);
			
			// Set CPU Register(s)
			} else if (strcmp(cmd, "set") == 0) {
				
				// Read which register (two instances to skip over "reg")
				fscanf(in, "%64s", cmd);
				fscanf(in, "%64s", cmd);
				strcpy(reg, cmd);
				
				// Set specified register
				fscanf(in, "%64s", cmd);
				
				// Call set reg
				cpuSetReg(&cpu, reg, strtol(cmd, &charPtr, 16));
			
			// Dump CPU
			} else if (strcmp(cmd, "dump") == 0) {
				
				// Call dump
				cpuDump(&cpu);
			}
		} else if (strcmp(cmd, "imemory") == 0) {
			
			// Read command
			fscanf(in, "%64s", cmd);
			
			// Reset iMemory
			if (strcmp(cmd, "reset") == 0) {
				
				// Call Reset
				iMemReset(&imem);
			
			} else if (strcmp(cmd, "create") == 0) {
				
				// Read size of memory to create
				fscanf(in, "%64s", cmd);
				num1 = strtol(cmd, &charPtr, 16);
				
				// Call create
				iMemCreate(&imem, num1);
				
			} else if (strcmp(cmd, "set") == 0) {
				
				// Get start address
				fscanf(in, "%64s", cmd);
				num1 = strtol(cmd, &charPtr, 16);
				
				// Read file
				fscanf(in, "%64s", cmd);
				fscanf(in, "%64s", cmd);
				
				//Set iMemory
				iMemSet(&imem, num1, cmd);
			}
		
		// Cache Command
		} else if (strcmp(cmd, "cache") == 0) {
			
			// Read command
			fscanf(in, "%64s", cmd);
			
			// Reset Cache
			if (strcmp(cmd, "reset") == 0) {
			
				// Call Reset
				cacheReset(&cache);
			
			// Enable Cache
			} else if (strcmp(cmd, "on") == 0) {
				
				// Set State
				cache.State = ON;
				
			// Disable Cache
			} else if (strcmp(cmd, "off") == 0) {
				
				// Set State
				cache.State = OFF;
			
			// Dump Cache
			}  else if (strcmp(cmd, "dump") == 0) {
				
				// Call Dump
				cacheDump(&cache);
				
			}
		}
	}
	
	// Close file
	fclose(in);
	return 0;
}