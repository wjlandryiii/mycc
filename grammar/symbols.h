/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "defs.h"

enum SYM_TYPE {
	SYMTYPE_UNKNOWN = 0,
	SYMTYPE_NONTERMINAL,
	SYMTYPE_TERMINAL,
	SYMTYPE_SIGMA,
};

struct symbol {
	int type;
	int string;
};


extern struct symbol symbolList[1024];
extern int symbolCount;

extern int terminals[128];
extern int terminalCount;
extern int nonterminals[128];
extern int nonterminalCount;


int lookupSymbol(int string);
int setSymbol(int string, int type);
int insertTerminal(int stringIndex);
int insertNonterminal(int stringIndex);


#define SYMBOLTYPE_TERMINAL (1)
#define SYMBOLTYPE_NONTERMINAL (2)

extern int nSYMBOLS;
extern int SYMBOL[MAX_SYMBOLS];
extern int SYMBOLTYPE[MAX_SYMBOLS];

int insertSymbol(int string, int type);


#endif
