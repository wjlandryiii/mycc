/*
 * Copyright 2015 Joseph Landry ALl Rights Reserved
 */

#include "symbols.h"

struct symbol symbolList[1024] = { {SYMTYPE_SIGMA, 0}, {0,0} };
int symbolCount = 1;

int lookupSymbol(int string){
	int i;

	for(i = 0; i < symbolCount; i++){
		if(symbolList[i].string == string){
			return i;
		}
	}
	return -1;
}

int setSymbol(int string, int type){
	int i;

	i = lookupSymbol(string);
	if(i >= 0){
		symbolList[i].type = type;
		return i;
	} else if(symbolCount < sizeof(symbolList) / sizeof(symbolList[0])){
		symbolList[symbolCount].type = type;
		symbolList[symbolCount].string = string;
		return symbolCount++;
	} else {
		return -1;
	}
}
