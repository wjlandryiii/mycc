/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <string.h>
#include <assert.h>
#include "lexer.h"
#include "symbols.h"
#include "stringtable.h"




char INPUTSTRING[1024*16];

int tokenizerLineNumber;

int lexerErrorNumber = LEXERR_OK;
int lexerErrorLineNumber;


int nTOKENS = 0;
int TOKENNAME[MAX_TOKENS];
int TOKENVALUE[MAX_TOKENS];


static int appendToken(int name, int value){
	assert(nTOKENS < MAX_TOKENS);
	TOKENNAME[nTOKENS] = name;
	TOKENVALUE[nTOKENS] = value;
	return nTOKENS++;
}





static char *in;

static int skipWhiteSpaceAndComments(void){
	char *p = in;

	for(;;){
		while(*p == ' ' || *p == '\t' || *p == '\n'){
			if(*p == '\n'){
				lexerErrorLineNumber++;
			}
			p++;
		}
		if(*p == '#'){
			p++;
			while(*p != '\0' && *p != '\n'){
				p++;
			}
		} else {
			break;
		}
	}
	return p - in;
}

static int nextLexeme(int *nameOut, int *lengthOut){
	int name = TOKNAME_INVALID;
	char *p = in;

	if(*p == '<'){
		p++;
		while(*p != '>' && *p != '\0' && *p != '\n'){
			p++;
		}
		if(*p == '>'){
			p++;
			name = TOKNAME_NONTERMINAL;
		} else {
			lexerErrorNumber = LEXERR_NONTERMINAL;
			return -1;
		}
	} else if(*p == '"'){
		p++;
		if(*p == '"'){
			p++;
			name = TOKNAME_SIGMA;
		} else {
			while(*p != '"' && *p != '\0' && *p != '\n'){
				p++;
			}
			if(*p == '"'){
				p++;
				name = TOKNAME_TERMINAL;
			} else {
				lexerErrorNumber = LEXERR_TERMINAL;
				return -1;
			}
		}
	} else if(*p == '\''){
		p++;
		if(*p == '\''){
			p++;
			name = TOKNAME_SIGMA;
		} else {
			while(*p != '\'' && *p != '\0' && *p != '\n'){
				p++;
			}
			if(*p == '\''){
				p++;
				name = TOKNAME_TERMINAL;
			} else {
				lexerErrorNumber = LEXERR_TERMINAL;
				return -1;
			}
		}
	} else if(*p == ':'){
		p++;
		name = TOKNAME_COLON;
	} else if(*p == '|'){
		p++;
		name = TOKNAME_PIPE;
	} else if(*p == ';'){
		p++;
		name = TOKNAME_SEMICOLON;
	} else if(*p == '\0'){
		name = TOKNAME_EOF;
	} else {
		lexerErrorNumber = LEXERR_STARTCHAR;
		return -1;
	}

	*nameOut = name;
	*lengthOut = p - in;
	return 0;
}


int tokenize(void){
	int name = TOKNAME_INVALID;
	char buf[64];
	int length;
	int terminalIndex;
	int nonterminalIndex;
	int symbol;

	nTOKENS = 0;
	memset(TOKENNAME, 0, sizeof(TOKENNAME));
	memset(TOKENVALUE, 0, sizeof(TOKENNAME));

	lexerErrorLineNumber = 1;
	in = INPUTSTRING;

	while(name != TOKNAME_EOF){
		terminalIndex = -1;
		nonterminalIndex = -1;
		in += skipWhiteSpaceAndComments();
		if(!nextLexeme(&name, &length)){
			if(name == TOKNAME_TERMINAL || name == TOKNAME_NONTERMINAL){
				if(length >= 64){
					lexerErrorNumber = LEXERR_SYMBOLTOOLONG;
					return -1;
				} else {
					int symType;
					int i;

					for(i = 0; i < length - 2; i++){
						buf[i] = in[i+1];
					}
					buf[i] = 0;
					i = insertString(buf);
					if(name == TOKNAME_TERMINAL){
						symbol = insertSymbol(i, SYMBOLTYPE_TERMINAL);
					} else if(name == TOKNAME_NONTERMINAL){
						symbol = insertSymbol(i, SYMBOLTYPE_NONTERMINAL);
					} else {
						assert(0);
					}
				}
			}
			in += length;
			
			appendToken(name, symbol); 
		} else {
			return -1;
		}
	}
	return 0;
}
