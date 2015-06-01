/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef SYMBOLS_H
#define SYMBOLS_H

enum SYM_TYPE {
	SYMTYPE_UNKNOWN = 0,
	SYMTYPE_NONTERMINAL,
	SYMTYPE_TERMINAL,
};

struct symbol {
	int type;
	int string;
};


extern struct symbol symbolList[1024];
extern int symbolCount;

int lookupSymbol(int string);
int setSymbol(int string, int type);

#endif
