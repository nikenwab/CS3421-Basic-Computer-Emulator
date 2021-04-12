
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

typedef struct {
	
	unsigned char 	clo;			// Cache Line Offset
	unsigned char 	curData[8]; 	// Current data the cache is mirroring
	bool		 	dataWritten[8]; // Bool to determine if byte was written
	char			dataFlag[9];	// Array of Flags ( 'I': Invalid, 'V': Valid, 'W': Written )
	
	// On/Off "switch" for cache
	enum			cacheState	{ON, OFF} State;
	
} Cache;

void cacheReset(Cache* cache);
void cacheDump(Cache* cache);

#endif