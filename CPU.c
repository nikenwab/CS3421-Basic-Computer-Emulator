
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
		
		// Start switch for instruction
		switch (cpu->instruction) {
			
			// Load word Instruction
			case LW :
			
				// Set load word destination and target
				cpu->destination = 	(fetchInt >> 14) & 0b000111;
				cpu->target = 		(fetchInt >> 8) & 0b000000000111;
				
				// DEBUG Statement
				//printf("---Load data at address %02X into register %02X---\n", cpu->target, cpu->destination);
			
				// Transition to Memory Request State
				cpu->State = CPU_MEMREQ;
				break;
				
			// Store word Instruction
			case SW :
			
				// Set store word source and target
				cpu->source = 		(fetchInt >> 11) & 0b000000111;
				cpu->target = 		(fetchInt >> 8) & 0b000000000111;
				
				// DEBUG Statement
				//printf("---Store data from register %02X into address %02X---\n", cpu->source, cpu->target);
			
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
			
			
			// If Cache Disabled
			if (cache->State != ON) {
				
				// Send data to memory for fetch
				memStartFetch(mem, cpu->reg[cpu->target], 1, &cpu->reg[cpu->destination], &fetchDone);
			
			// If Cache Enabled
			} else if (cache->State == ON) {
				
				cacheFetch(cache, mem, cpu->reg[cpu->target], 1, &cpu->reg[cpu->destination], &fetchDone);
				
			}
				
				
		// If a store word instruction
		} else if (cpu->instruction == SW) {
			
			
			// If Cache Disabled
			if (cache->State != ON) {
				
				// Send data to memory for store
				memStartStore(mem, cpu->reg[cpu->target], 1, &cpu->reg[cpu->source], &fetchDone);
			
			// If Cache Enabled
			} else if (cache->State == ON) {
				
				cacheStore(cache, mem, cpu->reg[cpu->target], 1, &cpu->reg[cpu->source], &fetchDone);
				
			}
				
		}
		
		// Transition to Wait State
		cpu->State = CPU_WAIT;
		
		
	// If CPU is using cycles for instructions
	} else if (cpu->State == CPU_WORK) {
		
		// Use cycle
		cpu->cpuWork--;
		
		// If cycle work done
		if (cpu->cpuWork == 0) {
		
			// Transition to Idle state
			cpu->State = CPU_IDLE;
		
		}
		
	}
	
	//fetchDone == true ? printf("cpufetch done!\n") : printf("cpufetch not done!\n") ;
	
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