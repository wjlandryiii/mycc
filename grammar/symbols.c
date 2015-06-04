/*
 * Copyright 2015 Joseph Landry ALl Rights Reserved
 */

#include <assert.h>
#include "symbols.h"


int nSYMBOLS = 0;
int SYMBOL[MAX_SYMBOLS];
int SYMBOLTYPE[MAX_SYMBOLS];

int insertSymbol(int string, int type){
	int i;
	for(i = 0; i < nSYMBOLS; i++){
		if(SYMBOL[i] == string){
			return i;
		}
	}
	assert(nSYMBOLS < MAX_SYMBOLS);
	SYMBOL[nSYMBOLS] = string;
	SYMBOLTYPE[nSYMBOLS] = type;
	return nSYMBOLS++;
}
