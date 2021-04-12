
/*
*	Author : Nathan I Kenwabikise
*	ID	  : nikenwab
*	Date   : 5/16/2020
*  
*	Description : Cache header file for Cache module
*/

#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "Memory.h"

typedef struct {
	
	unsigned char 	clo;			// Cache Line Offset
	unsigned char 	curData[8]; 	// Current data the cache is mirroring
	bool		 	dataWritten[8]; // Bool to determine if byte was written
	char			dataFlag[9];	// Array of Flags ( 'I': Invalid, 'V': Valid, 'W': Written )
	
	unsigned char 	cacheWork;
	
	unsigned int 	cacheAddress;
	unsigned char 	cacheCount;
	unsigned char 	*cacheDataPtr;
	bool			*cacheDataDonePtr;
	
	// On/Off "switch" for cache
	enum			cacheState	{OFF, ON} State;
	
	// Cache cycle state
	enum			cacheCycle	{FETCH, STORE, IDLE, WORK} Cycle;
	
} Cache;

void cacheReset(Cache* cache);
void cacheDump(Cache* cache);
void cacheFetch(Cache* cache, Memory* mem, unsigned char address, unsigned char count, unsigned char *dataPtr, bool *cacheDonePtr);
void cacheStore(Cache* cache, Memory* mem, unsigned char address, unsigned char count, unsigned char *dataPtr, bool *cacheDonePtr);
void cacheDoCycleWork(Cache* cache, Memory* mem);

bool cacheIsMoreCycleWorkNeeded(Cache* cache);

#endif