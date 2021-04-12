
#include "Cache.h"

/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 6/16/2020
*
*	Description : Cache module to handle reads/writes from/to memory
*/

static bool cacheFetchDone = false;

// Resets Cache
void cacheReset(Cache* cache) {
	
	// Reset State and Cache Line Offset
	cache->State = OFF;
	cache->Cycle = IDLE;
	cache->clo = 0;
	cacheFetchDone = false;
	cache->cacheAddress		= 0;
	cache->cacheCount		= 0;
	cache->cacheDataPtr		= 0;
	cache->cacheDataDonePtr	= 0;
	cache->cacheWork	= 0;
	
	// Sets all memory to invalid
	for (int i = 0; i < 8; i++) {
		cache->dataFlag[i] = 'I';
	}
	
	// Reset current data and flags
	for (int i = 0; i < 8; i++) {
		cache->curData[i] = 0x00;
	}
	
	// Sets all memory to unwritten
	for (int i = 0; i < 8; i++) {
		cache->dataWritten[i] = false;
	}
	
}

// Prints Cache data
void cacheDump(Cache* cache) {
	
	// CLO
	printf("CLO        : 0x%02X\n", cache->clo);
	
	// Cache Data
	printf("cache data : ");
	for (int i = 0; i < 8; i++) {
		printf("0x%02x ", cache->curData[i]);
	}
	printf("\n");
	
	// Flags
	printf("Flags      :   ");
	for (int i = 0; i < 8; i++) {
		if (cache->dataWritten[i]) {
			printf("W    ");
		} else {
			printf("%c    ", cache->dataFlag[i]);
		}
	}
	printf("\n\n");

}


// Do cache work if needed
void cacheDoCycleWork(Cache* cache, Memory* mem) {

	// If Cache is enabled
	if (cache->State == ON) {
		
		// If cache is working
		if (cache->cacheWork > 0) {
			
			// Do work
			cache->cacheWork--;
			
			// If work done
			if (cache->cacheWork == 0) {
				
				// Set bool to done
				*cache->cacheDataDonePtr = true;
			}
		}
		// Complete fetch cycle after memory done
		else if (cache->Cycle == FETCH && cacheFetchDone) {
		
				
			// Return data from cache
			*cache->cacheDataPtr = cache->curData[cache->cacheAddress-(cache->clo*8)];
			*cache->cacheDataDonePtr = true;
			cacheFetchDone = false;
			cache->Cycle = IDLE;
			
			// Reset variables
			cache->cacheAddress		= 0;
			cache->cacheCount		= 0;
			cache->cacheDataPtr		= 0;
			
		// Complete store cycle after memory done
		} else if (cache->Cycle == STORE && cacheFetchDone) {
			
			// Write data
			cache->curData[cache->cacheAddress] = cache->cacheDataPtr[0];
			cache->dataWritten[cache->cacheAddress] = true;
			memStore(mem, (cache->clo*8), 8, &cache->curData[0], &cacheFetchDone);
			
			// Set finished
			*cache->cacheDataDonePtr = true;
			cacheFetchDone = false;
			cache->Cycle = IDLE;
			
			// Reset variables
			cache->cacheAddress		= 0;
			cache->cacheCount		= 0;
			cache->cacheDataPtr		= 0;
			
		}
	}
}

// Determine if cache needs to finish cycle work
bool cacheIsMoreCycleWorkNeeded(Cache* cache) {
	
	if (cacheFetchDone && (cache->Cycle == FETCH || cache->Cycle == STORE)) {
		
		return true;
		
	} else { 
	
		return false;
		
	}
}

// Read from cache
void cacheFetch(Cache* cache, Memory* mem, unsigned char address, unsigned char count, unsigned char *dataPtr, bool *cacheDonePtr) {
	
	
	// Force cache invalid command
	if (address == 0xFF) {
	
		// Return 0
		dataPtr[0] = 0;
		
		// Set data flags to invalid
		for (int i = 0; i < 8; i++) {
			cache->dataFlag[i] = 'I';
		}
		
		// Set data done bool
		*cacheDonePtr = true;
	
	// Otherwise load word
	} else {
		
		// Compute cache line offset of memory address
		int computedCLO = floor(address/8.0);
		
		// Check if data is in cache
		if (cache->clo == computedCLO && cache->dataFlag[ address - (cache->clo * 8)] == 'V' ) {
			
			// Return data from cache
			for (int i = 0; i < count; i++) {
				dataPtr[i] = cache->curData[ (address - (cache->clo * 8)) + i ];
			}
			
			*cacheDonePtr = true;
			
		} else {
			
			// Send data to memory for fetch
			cacheFetchDone = false;
			memStartFetch(mem, (computedCLO*0x08), 8, &cache->curData[0], &cacheFetchDone);
			
			// Set data flags to valid
			for (int i = 0; i < 8; i++) {
				cache->dataFlag[i] = 'V';
			}
			
			// Set cycle state
			cache->Cycle = FETCH;
			
			// Save data for after memory finishes
			cache->cacheAddress			= address;
			cache->cacheCount			= count;
			cache->cacheDataPtr			= dataPtr;
			cache->cacheDataDonePtr		= cacheDonePtr;
		}
	}
}

// Store to cache
void cacheStore(Cache* cache, Memory* mem, unsigned char address, unsigned char count, unsigned char *dataPtr, bool *cacheDonePtr) {
	
	// If cache flush command
	if (address == 0xFF) {
	
		// Check for written data
		bool writtenData = false;
		for (int i = 0; i < 8; i++) {
			if (cache->dataWritten[i] == true) {
				writtenData = true;
			}
		}
		
		// If data was written
		if (writtenData) {

			// Reset Cache bool
			cacheFetchDone = false;
			
			// Start memory flush
			memStartStore(mem, (cache->clo*8), 8, &cache->curData[0], &cacheFetchDone);
			
			// Save variables for later
			cache->Cycle = STORE;
			cache->cacheAddress			= address;
			cache->cacheCount			= count;
			cache->cacheDataPtr			= dataPtr;
			cache->cacheDataDonePtr		= cacheDonePtr;
			
		// If no data written then finish
		} else {
			
			*cacheDonePtr = true;
			
		}
	
	// Otherwise Store word
	} else {
		
		// Compute cache line offset of memory address
		int computedCLO = floor(address/8.0);
		
		// Check for valid data
		bool validData = false;
		for (int i = 0; i < 8; i++) {
			if (cache->dataFlag[i] == 'V') {
				validData = true;
			}
		}
		
		// Check if data is in cache
		if (validData == false) {
			
			// Set new cache line offset if no valid data
			cache->clo = computedCLO;
		}
		
		// If Cache Hit
		if (cache->clo == computedCLO) {
			
			// Store data to cache
			cache->dataWritten[address - (cache->clo*8)] = true;
			cache->curData[address] = *dataPtr;
			cache->dataWritten[address] = true;
			
			// Store current memory
			memStore(mem, (cache->clo*8), 8, &cache->curData[0], &cacheFetchDone);
			
			// Set data done bool
			*cacheDonePtr = true;
		
		// Else if Cache Miss
		} else {
			
			// Check for written data
			bool anyWrittenData = false;
			for (int i = 0; i < 8; i++) {
				if (cache->dataWritten[i]) {
					anyWrittenData = true;
				}
			}
			
			// If there was any data written to cache
			if (anyWrittenData) {
				
				// Set data flags to valid
				for (int i = 0; i < 8; i++) {
					cache->dataFlag[i] = 'V';
				}
				
				// Get new memory to write to
				memStore(mem, (cache->clo*8), 8, &cache->curData[0], &cacheFetchDone);
				
				// Update cache and fetch memory
				cache->clo = computedCLO;
				memStartFetch(mem, (computedCLO*0x08), 8, &cache->curData[0], &cacheFetchDone);
				cache->dataWritten[address - (cache->clo*8)] = true;
				
				// Save details for when memory finishes
				cache->Cycle = STORE;
				cache->cacheAddress			= address;
				cache->cacheCount			= count;
				cache->cacheDataPtr			= dataPtr;
				cache->cacheDataDonePtr		= cacheDonePtr;
				
			// No data written so fetch new data
			} else {
				
				// Get new memory to write to
				cache->clo = computedCLO;
				memStore(mem, (cache->clo*8), 8, &cache->curData[0], &cacheFetchDone);
				memFetch(mem, (cache->clo*8), 8, &cache->curData[0], &cacheFetchDone);
				cacheFetchDone = false;
				
				// Start fetch
				memStartFetch(mem, (computedCLO*0x08), 8, &cache->curData[0], &cacheFetchDone);
				cache->dataWritten[address - (cache->clo*8)] = true;
				
				// Set new data to invalid
				for (int i = 0; i < 8; i++) {
					cache->dataFlag[i] = 'I';
				}
				
				// Save details for when memory finishes
				cache->Cycle = STORE;
				cache->cacheAddress			= address;
				cache->cacheCount			= count;
				cache->cacheDataPtr			= dataPtr;
				cache->cacheDataDonePtr		= cacheDonePtr;
			}
		}
	}
	
}