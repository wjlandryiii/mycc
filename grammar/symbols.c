/*
 * Copyright 2015 Joseph Landry ALl Rights Reserved
 */

#include "symbols.h"

struct symbol symbolList[1024] = { {SYMTYPE_SIGMA, 0}, {0,0} };
int symbolCount = 1;

int terminals[128];
int terminalCount = 0;
int nonterminals[128];
int nonterminalCount = 0;

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

int insertTerminal(int stringIndex){
	int i;

	for(i = 0; i < terminalCount; i++){
		if(terminals[i] == stringIndex){
			return i;
		}
	}

	if(terminalCount < sizeof(terminals)/sizeof(*terminals)){
		terminals[terminalCount] = stringIndex;
		return terminalCount++;
	} else {
		return -1;
	}
}

int insertNonterminal(int stringIndex){
	int i;

	for(i = 0; i < nonterminalCount; i++){
		if(nonterminals[i] == stringIndex){
			return i;
		}
	}

	if(nonterminalCount < sizeof(nonterminals)/sizeof(*nonterminals)){
		nonterminals[nonterminalCount] = stringIndex;
		return nonterminalCount++;
	} else {
		return -1;
	}
}
