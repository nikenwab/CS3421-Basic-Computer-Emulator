#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "iMemory.h"
#include "CPU.h"

/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 5/28/2020
*  
*	Description : Instruction Memory module to handle creating and fetching for a CPU module
*/

// Initializes memory array
void iMemCreate(iMemory* iMem, int size) {

	// Initialize memory and store size
	iMem->memPtr = malloc(sizeof(uint32_t) * size);
	iMem->size = size;
	
}

// Resets all imemory to 0
void iMemReset(iMemory* iMem) {
	
	// Loop through memory
	for (int i = 0; i < iMem->size; i++) {
		
		// Set each position to 0
		iMem->memPtr[i] = 0;
		
	}
	
}

//Prints specified memory
void iMemDump(iMemory* iMem, unsigned char address, unsigned char count) {
	
	// Initialize variables and formatting
	unsigned char current = address & (1<<4);
	unsigned char row = current;
	printf("Addr     0     1     2     3     4     5     6     7\n");
	printf("0x%02X ", current);
	
	// Loop through memory that does not need to be printed
	while (current != address) {
		printf("      ");
		current++;
	}
	
	// Loop through memory and print
	for (int i = 0; i < count; i++) {
		
		// Formatting
		printf("%05X " , iMem->memPtr[address]);
		current++;
		address += sizeof(uint32_t);
		
		// Insert new line as necessary
		if (current%0x08 == 0x00) {
			
			row += 8;
			printf("\n0x%02X ", row);
			
		}
	}
	printf("\n\n");
}

// Sets specified memory
void iMemSet(iMemory* iMem, unsigned char address, char* fileName) {

	// Initialize variables and test file fro validity
	FILE* memIn = fopen(fileName, "r");
	if (memIn == NULL) {
		printf("\nERROR: File \"%s\" could not be opened!\n\n", fileName);
		return;
	}
	unsigned int current = 0;
	
	// Read in each word
	while (fscanf(memIn, "%X", &current) == 1) {
		
		// Store read word
		iMem->memPtr[address] = current;
		address += sizeof(uint32_t);
	}
	
	// Close file
	fclose(memIn);

}

// Sets passed pointer to passed address from iMemory
void iMemFetch(iMemory* iMem, unsigned int address, unsigned int *returnNum) {
	
	*returnNum = iMem->memPtr[address*sizeof(uint32_t)];
	
}















