/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include "symbols.h"

int main(int argc, char *argv[]){
	struct symbol *symbol;
	int i;

	if(symbolList[0].type != SYMTYPE_SIGMA){
		printf("Symbol SIGMA not in list\n");
	}
	if(symbolList[0].string != 0){
		printf("invalid SIGMA string\n");
	}


	i = setSymbol(444, SYMTYPE_NONTERMINAL);
	if(i != lookupSymbol(444)){
		printf("Symbol not found\n");
	}

	symbol = symbolList + i;

	if(symbol->type != SYMTYPE_NONTERMINAL){
		printf("Stored symbol type incorrectly\n");
	}
	if(symbol->string != 444){
		printf("Stored symbol string incorrectly\n");
	}
	
	if(symbolList[0].type != SYMTYPE_SIGMA){
		printf("Symbol SIGMA not in list\n");
	}
	if(symbolList[0].string != 0){
		printf("invalid SIGMA string\n");
	}
	return 0;
}
