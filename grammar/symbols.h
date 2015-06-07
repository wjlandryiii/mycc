/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "defs.h"

#define SYMBOLTYPE_TERMINAL (1)
#define SYMBOLTYPE_NONTERMINAL (2)

extern int nSYMBOLS;
extern int SYMBOL[MAX_SYMBOLS];
extern int SYMBOLTYPE[MAX_SYMBOLS];

int insertSymbol(int string, int type);
int lookupSymbol(int string);

#endif
