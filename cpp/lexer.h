/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef LEXER_H
#define LEXER_H

int nextLexeme(char *s, int *length, int *type);
enum LEXEMTYPES {
	LT_INV,
	LT_WS,
	LT_NL,
	LT_NUM,
	LT_ID,
	LT_CC,
	LT_STRLIT,
	LT_PUNC,
};

#endif
