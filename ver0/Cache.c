
#include "Cache.h"

/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 6/16/2020
*
*	Description : Cache module to handle reads/writes from/to memory
*/

// Resets Cache
void cacheReset(Cache* cache) {
	
	// Reset State and Cache Line Offset
	cache->State = OFF;
	cache->clo = 0;
	
	// Sets all memory to invalid
	for (int i = 0; i < 8; i++) {
		cache->dataFlag[i] = 'I';
	}
	
	// Reset current data and flags
	for (int i = 0; i < 8; i++) {
		cache->curData[i] = 0x00;
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