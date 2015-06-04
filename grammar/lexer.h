/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef LEXER_H
#define LEXER_H

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

enum LEX_ERRORS {
	LEXERR_OK = 0,
	LEXERR_INPUT,
	LEXERR_NONTERMINAL,
	LEXERR_TERMINAL,
	LEXERR_SYMBOLTOOLONG,
	LEXERR_STARTCHAR,
};

struct token {
	int name;
	int symbol;
	int terminalIndex;
	int nonterminalIndex;
};

extern char lexerInputBuffer[16*1024];
extern struct token tokenStream[1024];
extern int tokenStreamLength;
extern int lexerErrorNumber;
extern int lexerErrorLineNumber;

#define MAX_TOKENS (1024)
extern int nTOKENS;
extern int TOKENNAME[MAX_TOKENS];
extern int TOKENVALUE[MAX_TOKENS];

int tokenize(void);

#endif
