

#include "Memory.h"

/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 5/31/2020
*  
*	Description : Memory module to handle creating and fetching for a CPU module
*/

// Initializes memory array
void memCreate(Memory* mem, int size) {
	
	// Initialize memory and store size
	mem->memPtr = malloc(sizeof(unsigned char*) * size);
	mem->size = size;
}

// Sets all memory in array to 0
void memReset(Memory* mem) {
	
	// Loop through memory
	for (int i = 0; i < mem->size; i++) {
		mem->memPtr[i] = 0;
	}
	
	// Set initial State to idle and reset wait timer
	mem->State = MEM_IDLE;
	mem->memWait = 0;
}

// Prints specified memory
void memDump(Memory* mem, unsigned char address, unsigned char count) {
	
	// Initialize variables and formatting
	unsigned char current = 0x00;
	int row = 0;
	printf("Addr 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	printf("0x00 ");
	
	// Loop through memory that does not need to be printed
	while (current != address) {
		
		// Formatting
		printf("   ");
		current++;
		
		// Insert new line as necessary
		if (current > 16) {
			printf("\n0x%d0 ", current/0x0F);
			row++;
		}
	}
	
	// Loop through memory and print
	for (int i = 0; i < count; i++) {
		
		printf("%02X " ,mem->memPtr[address]);
		address += sizeof(unsigned char);
		
		// Insert new line as necessary
		if ((address - (row*16)) % 0x0F == 0x00 && i != count - 2) {
			printf("%02X " ,mem->memPtr[address]);
			address += sizeof(unsigned char);
			i++;
			printf("\n");
			printf("0x%d0 ", address/0x0F);
			row++;
		}
	}
	printf("\n\n");
}

// Sets specified memory to input value/s
void memSet(Memory* mem, unsigned char address, unsigned char count, unsigned char value, ...) {
	
	// Initialize variables
	mem->memPtr[address] = value;
	va_list vals;
	va_start(vals, value);

	// Loop to set memory
	for (int i = 0; i < count-1; i++) {
		address += sizeof(unsigned char);
		mem->memPtr[address] = va_arg(vals, int);
	}
}

// Used to save variables used in a fetch command
void memStartFetch(Memory* mem, unsigned int address, unsigned int count, unsigned char *dataPtr, bool *memDonePtr) {
	
	// Sets memory to Fetch State
	mem->State = MEM_FETCH;
	
	// Save variables used for fetch command
	mem->memAddress = address;
	mem->memCount 	= count;
	mem->memDataPtr = dataPtr;
	mem->memDone 	= memDonePtr;
	
}

// Used to save variables used in a store command
void memStartStore(Memory* mem, unsigned int address, unsigned int count, unsigned char *dataPtr, bool *memDonePtr) {
	
	// Sets memory to Store State
	mem->State = MEM_STORE;
	
	// Save variables used for fetch command
	mem->memAddress = address;
	mem->memCount 	= count;
	mem->memDataPtr = dataPtr;
	mem->memDone 	= memDonePtr;
}

// Starts tick in memory device
void memStartTick(Memory* mem) {

	// If in wait State
	if (mem->State == MEM_WAIT) {
		
		// Increment wait timer
		mem->memWait++;
		
		// if wait timer is done
		if (mem->memWait == 5) {
			
			// Set State to Moving data
			mem->State = MOVE_DATA;
			
			// Reset wait timer
			mem->memWait = 0;
		}
	}
}

// Performs cycle work on memory device
void memDoCycleWork(Memory* mem) {
	
	// If in Fetch State
	if (mem->State == MEM_FETCH) {
		
		// Fetches data
		for (int i = 0; i < mem->memCount; i++) {
			
			// Sets passed register to value at memory address
			mem->memDataPtr[i] = mem->memPtr[mem->memAddress + i];
		}
		
		// Reset variables
		mem->memAddress = 0;
		mem->memCount 	= 0;
		mem->memDataPtr = 0;
	
		// Transition State to wait and increment wait timer
		mem->State = MEM_WAIT;
		mem->memWait++;
	
	// If in Store State
	} else if (mem->State == MEM_STORE) {
		
		// Stores data
		for (int i = 0; i < mem->memCount; i++) {
			
			// Set data at memory address to passed value
			mem->memPtr[mem->memAddress+i] = mem->memDataPtr[i];
		}
		
		// Reset variables
		mem->memAddress = 0;
		mem->memCount 	= 0;
		mem->memDataPtr = 0;
		
		// Transition State to wait and increment wait timer
		mem->State = MEM_WAIT;
		mem->memWait++;
		
	}
	
	// If wait is done and ready to move data
	if (mem->State == MOVE_DATA) {
		
		// Copy memory back to caller
		memcpy(mem->memPtr, mem->memPtr+mem->memAddress, mem->memCount);
		
		// Tell caller operation is done and reset State
		*mem->memDone = true;
		mem->State = MEM_IDLE;
	}
}

// Determine is more work is needed this cycle
bool memIsMoreCycleWorkNeeded(Memory* mem) {
	
	// If in wait or idle no more work is needed
	if (mem->State == MEM_WAIT || mem->State == MEM_IDLE) {
		
		return false;
	
	// If not then signal for more work
	} else {
		
		return true;
		
	}
}

// Pass data from specified address to passed pointer
void memFetch(Memory* mem, unsigned int address, unsigned char count, unsigned char *answerPtr, bool *fetchDonePtr) {
	
	// Loop for each byte
	for (int i = 0; i < count; i++) {
		
		// Set passed pointer to location in data memory
		answerPtr[i] = mem->memPtr[address + i];
	}
	
	// Set data done bool
	*fetchDonePtr = true;
	
}

// Store passed data into memory at specified location
void memStore(Memory* mem, unsigned int address, unsigned char count, unsigned char *answerPtr, bool *fetchDonePtr) {
	
	// Loop for each byte
	for (int i = 0; i < count; i++) {
		
		// Set data memory at passed address to passed data
		mem->memPtr[address + i] = answerPtr[i];
	}
	
	// Set data done bool
	*fetchDonePtr = true;
	
}







