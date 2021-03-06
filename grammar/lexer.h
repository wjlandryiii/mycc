/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef LEXER_H
#define LEXER_H

#include "defs.h"

enum TOK_NAMES {
	TOKNAME_INVALID = 0,
	TOKNAME_NONTERMINAL,
	TOKNAME_TERMINAL,
	TOKNAME_COLON,
	TOKNAME_PIPE,
	TOKNAME_SEMICOLON,
	TOKNAME_SIGMA,
	TOKNAME_EOF,
};

extern char INPUTSTRING[16*1024];

extern int nTOKENS;
extern int TOKENNAME[MAX_TOKENS];
extern int TOKENVALUE[MAX_TOKENS];

enum LEX_ERRORS {
	LEXERR_OK = 0,
	LEXERR_INPUT,
	LEXERR_NONTERMINAL,
	LEXERR_TERMINAL,
	LEXERR_SYMBOLTOOLONG,
	LEXERR_STARTCHAR,
};
extern int lexerErrorNumber;
extern int lexerErrorLineNumber;

int tokenize(void);

#endif
