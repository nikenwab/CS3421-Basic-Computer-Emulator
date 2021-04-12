
#include "CPU.h"

/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 5/30/2020
*  
*	Description : CPU module to handle storing values and completing cycles
*/

// Variables to store memory fetches
static 	unsigned int	fetchInt;
static 	bool			fetchDone = false;
static 	bool			cacheFetchDone = false;

// Resets registers and program counter to zero
void cpuReset(CPU* cpu) {
	
	// Set initial State
	cpu->State = CPU_IDLE;
	
	// Loop through registers
	for (int i = 0; i < 8; i++) {
		cpu->reg[i] = 0x00;
	}
	// Reset program counter and tick counter
	cpu->PC = 0x00;
	cpu->TC = 0x00;
}

// Sets specified register to input value
void cpuSetReg(CPU* cpu, char reg[], unsigned char value) {
	// If chain to determine register to set
	if (strcmp(reg, "PC") == 0) {
		cpu->PC = 0x00;
		cpu->State = CPU_FETCH;
	} else if (strcmp(reg, "RA") == 0) {
		cpu->reg[0] = value;
	} else if (strcmp(reg, "RB") == 0) {
		cpu->reg[1] = value;
	} else if (strcmp(reg, "RC") == 0) {
		cpu->reg[2] = value;
	} else if (strcmp(reg, "RD") == 0) {
		cpu->reg[3] = value;
	} else if (strcmp(reg, "RE") == 0) {
		cpu->reg[4] = value;
	} else if (strcmp(reg, "RF") == 0) {
		cpu->reg[5] = value;
	} else if (strcmp(reg, "RG") == 0) {
		cpu->reg[6] = value;
	} else if (strcmp(reg, "RH") == 0) {
		cpu->reg[7] = value;
	// Print error message if register does not exist
	} else {
		printf("%s is not a register!\n", reg);
	}
}

// Receive tick to determine State
void cpuStartTick(CPU* cpu) {
	
	// Increment Tick Counter
	if (cpu->State != CPU_HALT) {
		
		cpu->TC++;
	
	}
	
	// If in idle State
	if (cpu->State == CPU_IDLE) {
		
		// Transition to Fetch State
		cpu->State = CPU_FETCH;
		fetchDone = false;
		
	}
	
}

// Completes one cycle of work
void cpuDoCycleWork(CPU* cpu, Memory* mem, iMemory* iMem, Cache* cache) {
	
	
	// Fetch and Decode instruction
	if (cpu->State == CPU_FETCH) {
		
		// Call iMemory to fetch instruction at PC offset
		iMemFetch(iMem, cpu->PC, &fetchInt);
		
		// Transition to Decode State
		cpu->State = CPU_DECODE;
		
	// Decode current instruction
	} else if (cpu->State == CPU_DECODE) {

		//Get Instruction data
		cpu->instruction = 	(fetchInt >> 17) & 0b111;
		cpu->destination =	(fetchInt >> 14) & 0b000111;
		cpu->source		 =	(fetchInt >> 11) & 0b000000111;
		cpu->target		 =	(fetchInt >> 8)  & 0b000000000111;
		cpu->immediate	 =	(fetchInt & 0xFF);
		
		//printf("\n----Instruction : %03X %03X %03X %03X %06X----\n\n", cpu->instruction, cpu->destination, cpu->source, cpu->target, cpu->immediate);
		
		// Start switch for instruction
		switch (cpu->instruction) {
			
			// Load word Instruction
			case LW :
			
				// Set load word destination and target
				cpu->destination = 	(fetchInt >> 14) & 0b000111;
				cpu->target = 		(fetchInt >> 8) & 0b000000000111;
			
				// Transition to Memory Request State
				cpu->State = CPU_MEMREQ;
				break;
				
			// Store word Instruction
			case SW :
			
				// Set store word source and target
				cpu->source = 		(fetchInt >> 11) & 0b000000111;
				cpu->target = 		(fetchInt >> 8) & 0b000000000111;
			
				// Transition to Memory Request State
				cpu->State = CPU_MEMREQ;
				break;
				
			// Add Instruction
			case ADD :
			
				// Add Source and Target and store in Destination
				cpu->reg[cpu->destination] = cpu->reg[cpu->source] + cpu->reg[cpu->target];
				
				// Transition to Idle state
				cpu->PC++;
				cpu->State = CPU_IDLE;
				break;
				
			// Add Immediate Instruction
			case ADDI :
			
				// Add Source and Immediate and store in Destination
				cpu->reg[cpu->destination] = cpu->reg[cpu->source] + cpu->immediate;
				
				// Transition to Idle state
				cpu->PC++;
				cpu->State = CPU_IDLE;
				break;
				
			// Multiply Instruction
			case MUL : 
			
				// Multiply and store results
				cpu->reg[cpu->destination] = (unsigned int)(cpu->reg[cpu->source] >> 4) * (unsigned int)(cpu->reg[cpu->source] & 0b00001111);
				
				// Transition to Idle state
				cpu->PC++;
				cpu->State = CPU_WORK;
				cpu->cpuWork = 1;
				break;
			
			// Invert Instruction
			case INV : 
			
				// Invert Source and store in Destination
				cpu->reg[cpu->destination] = ~(cpu->reg[cpu->source]);
				
				// Transition to Idle state
				cpu->PC++;
				cpu->State = CPU_IDLE;
				break;
				
			case BRANCH :
			
				// Branch if Equal (BEQ)
				if (cpu->destination == 0x00) {
					
					// Compare Source reg word with Target reg word
					if (cpu->reg[cpu->source] == cpu->reg[cpu->target]) {
						
						// If true set PC to immediate and break
						cpu->PC = cpu->immediate;
						cpu->State = CPU_WORK;
						cpu->cpuWork = 1;
						break;	
						
					} else {
						
						// Increment PC and break
						cpu->PC++;
						cpu->State = CPU_IDLE;
						break;	
						
					}
					
				// Branch if Not Equal (BNEQ)
				} else if (cpu->destination == 0x01) {
					
					// Compare Source reg word with Target reg word
					if (cpu->reg[cpu->source] != cpu->reg[cpu->target]) {
						
						// If true set PC to immediate and break
						cpu->PC = cpu->immediate;
						cpu->State = CPU_WORK;
						cpu->cpuWork = 1;
						break;	
						
					} else {
						
						// Increment PC and break
						cpu->PC++;
						cpu->State = CPU_IDLE;
						break;	
						
					}
				
				// Branch if Less Than (BLT)
				} else if (cpu->destination == 0x02) {
				
					// Compare Source reg word with Target reg word
					if (cpu->reg[cpu->source] < cpu->reg[cpu->target]) {
						
						// If true set PC to immediate and break
						cpu->PC = cpu->immediate;
						cpu->State = CPU_WORK;
						cpu->cpuWork = 1;
						break;	
						
					} else {
						
						// Increment PC and break
						cpu->PC++;
						cpu->State = CPU_IDLE;
						break;	
						
					}
					
				}
				
			case HALT : 
			
				// Set CPU State to Halt
				cpu->PC++;
				cpu->State = CPU_HALT;
				break;
				
		}	
	
	// Call memory to request data
	} else if (cpu->State == CPU_MEMREQ) {
		
		// If a fetch word instruction
		if (cpu->instruction == LW) {
			
			// If Cache is enabled
			if (cache->State == ON) {
				
				// Force cache invalid command
				if (cpu->reg[cpu->target] == 0xFF) {
				
					// Return 0
					cpu->reg[cpu->destination] = 0;
					
					// Set data flags to invalid
					for (int i = 0; i < 8; i++) {
						cache->dataFlag[i] = 'I';
					}
				
				// Otherwise load word
				} else {
					
					// Compute cache line offset of memory address
					int computedCLO = floor(cpu->reg[cpu->target]/8.0);
					
					// Check if data is in cache
					if (cache->clo == computedCLO && cache->dataFlag[ cpu->reg[cpu->target] - (cache->clo * 8)] == 'V' ) {
						//printf("LW Cache hit!\n");
						
						// Return data from cache
						printf("set reg %02X to %02X from cache address %02X\n", cpu->destination, cache->curData[ cpu->reg[cpu->target] - (cache->clo*8) ], cpu->reg[cpu->target] - (cache->clo*8));
						cpu->reg[cpu->destination] = cache->curData[ cpu->reg[cpu->target] - (cache->clo * 8) ];
						// was cpu->reg[cpu->destination] = cache->curData[ cpu->reg[cpu->target] - (cache->clo * 8) ];
						
						fetchDone = true;
						
					} else {
						//printf("LW Cache miss!\n");
						
						// Send data to memory for fetch
						cacheFetchDone = false;
						memStartFetch(mem, (computedCLO*0x08), 8, &cache->curData[0], &cacheFetchDone);
						
						// Set data flags to valid
						for (int i = 0; i < 8; i++) {
							cache->dataFlag[i] = 'V';
						}
					}
				}
				
			// If Cache is disabled
			} else {
				
				// Send data to memory for fetch
				memStartFetch(mem, cpu->reg[cpu->target], 1, &cpu->reg[cpu->destination], &fetchDone);
				
			}
				
		// If a store word instruction
		} else if (cpu->instruction == SW) {
			
			// If Cache is Enabled
			if (cache->State == ON) {
				
				// If cache flush command
				if (cpu->reg[cpu->target] == 0xFF) {
					
					printf("Forced Cache Flush!\n");
				
					// Check for written data
					bool writtenData = false;
					for (int i = 0; i < 8; i++) {
						
						if (cache->dataFlag[i] == 'W') {
							writtenData = true;
						}
						
					}
					
					// If data was written
					if (writtenData) {
						
						// Write Cache Data to Data Memory
						for (int i = 0; i < 8; i++) {
							mem->memPtr[i] = cache->curData[i];
						}
						memStartFetch(mem, 0, 8, cache->curData, &cacheFetchDone);
						
					}
				
				// Otherwise Store word
				} else {
					
					// Compute cache line offset of memory address
					int computedCLO = floor(cpu->reg[cpu->target]/8.0);
					
					// Check for valid data
					bool validData = false;
					for (int i = 0; i < 8; i++) {
						if (cache->dataFlag[i] == 'V') {
							validData = true;
						}
					}
					
					// Check if data is in cache
					if (validData == false) {
						cache->clo = computedCLO;
					}
					
					// If Cache Hit
					if (cache->clo == computedCLO) {
						//printf("SW Cache hit!\n");
						
						// Store data to cache
						printf("Store %02X at cache/data address %02X\n", cpu->reg[cpu->source], cpu->reg[cpu->target]);
						cache->curData[cpu->reg[cpu->target]] = cpu->reg[cpu->source];
						cache->dataWritten[cpu->reg[cpu->target]] = true;
						
						fetchDone = true;
					
					// Else if Cache Miss
					} else {
						//printf("SW Cache miss!\n");
						
						// Check for written data
						bool anyWrittenData = false;
						for (int i = 0; i < 8; i++) {
							if (cache->dataWritten[i]) {
								anyWrittenData = true;
							}
						}
						
						// If there was any data written to cache
						if (anyWrittenData) {
							printf("Cache Flush!\n");
							
							// Send data to memory for store
							//for (int i = 0; i < 8; i++) {
							//	printf("Cache miss, setting memory address %02X to %02X\n", mem->memPtr[(cache->clo*0x08)+i], cache->curData[i]);
							//	mem->memPtr[(cache->clo*0x08)+i] = cache->curData[i];
							//}
							
							// Set data flags to valid
							for (int i = 0; i < 8; i++) {
								cache->dataFlag[i] = 'V';
							}
							
							// Get new memory to write to
							cache->clo = computedCLO;
							memStartFetch(mem, (computedCLO*0x08), 8, &cache->curData[0], &cacheFetchDone);
							
						} else {
							
							mem->memPtr[cpu->reg[cpu->target]] = cpu->reg[cpu->source];
							
							// Get new memory to write to
							cache->clo = computedCLO;
							memStartFetch(mem, (computedCLO*0x08), 8, &cache->curData[0], &cacheFetchDone);
						}
					}
				}
				
			// If Cache is disabled
			} else {
				
				// Send data to memory for store
				memStartStore(mem, cpu->reg[cpu->target], 1, &cpu->reg[cpu->source], &fetchDone);
				
			}
		}
		
		// Transition to Wait State
		cpu->State = CPU_WAIT;

	// If CPU is using cycles for instructions
	} else if (cpu->State == CPU_WORK) {
		
		// Use cycle
		cpu->cpuWork--;
		
		// If cycle work done
		if (cpu->cpuWork <= 0) {
		
			// Transition to Idle state
			cpu->State = CPU_IDLE;
		
		}
		
	// If Memory to Cache Fetch is complete
	} else if (cacheFetchDone) {
		if (cpu->instruction == LW) {
			
			//printf("Memory done, loading word!\n");
			
			// Return data from cache
			cpu->reg[cpu->destination] = cache->curData[cpu->reg[cpu->target]-(cache->clo*8)];
				
			cacheFetchDone = false;
			
		} else if (cpu->instruction == SW) {
			
			//printf("Memory done, storing word!\n");
			
			// Set written byte
			printf("Store %02X at cache/data address %02X\n", cpu->reg[cpu->source], cpu->reg[cpu->target]);
			cache->curData[cpu->reg[cpu->target]] = cpu->reg[cpu->source];
			cache->dataWritten[cpu->reg[cpu->target]] = true;
			
			cacheFetchDone = false;
			
			//printf("Value to set reg %02X to : %02X\n", cpu->reg[cpu->destination], cache->curData[cpu->reg[cpu->target]-(cache->clo*8)]);
			//cpu->reg[cpu->destination] = cache->curData[cpu->reg[cpu->target]-(cache->clo*8)];
			//fetchDone = true;
			//cacheFetchDone = false;
			
		}
		
		fetchDone = true;
		
	}
	
	// If memory request is finished and data recieved 
	if (fetchDone) {
		
		// Increment program counter and reset to Idle
		cpu->PC++;
		cpu->State = CPU_IDLE;
	}
	
}

// Determine is more work is needed this cycle based on State
bool cpuIsMoreCycleWorkNeeded(CPU* cpu) {
	
	// If in wait or idle signal work done this cycle
	if (cpu->State == CPU_WAIT || cpu->State == CPU_IDLE || cpu->State == CPU_HALT || cpu->State == CPU_WORK) {
		
		return false;
		
	// Otherwise signal for more work
	} else {
		
		return true;
		
	}
}

// Dumps info on each register
void cpuDump(CPU* cpu) {
	
	printf("PC: 0x%02X\n",  cpu->PC);
	printf("RA: 0x%02X\n",  cpu->reg[0]);
	printf("RB: 0x%02X\n",  cpu->reg[1]);
	printf("RC: 0x%02X\n",  cpu->reg[2]);
	printf("RD: 0x%02X\n",  cpu->reg[3]);
	printf("RE: 0x%02X\n",  cpu->reg[4]);
	printf("RF: 0x%02X\n",  cpu->reg[5]);
	printf("RG: 0x%02X\n",  cpu->reg[6]);
	printf("RH: 0x%02X\n",  cpu->reg[7]);
	printf("TC: %d\n\n",  cpu->TC);
	
}