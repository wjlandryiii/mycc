/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include "symbols.h"

int main(int argc, char *argv[]){
	struct symbol *symbol;
	int i;

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
	return 0;
}
