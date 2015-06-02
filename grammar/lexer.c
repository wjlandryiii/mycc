/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include "lexer.h"
#include "symbols.h"
#include "stringtable.h"

char lexerInputBuffer[1024*16];
struct token tokenStream[1024];

int tokenStreamLength = 0;
int tokenizerLineNumber;

int lexerErrorNumber = LEXERR_OK;
int lexerErrorLineNumber;


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

	tokenStreamLength = 0;
	lexerErrorLineNumber = 1;
	in = lexerInputBuffer;

	while(name != TOKNAME_EOF){
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
					i = stringtableAddString(buf);
					if(name == TOKNAME_TERMINAL){
						symType = SYMTYPE_TERMINAL;
					} else {
						symType = SYMTYPE_NONTERMINAL;
					}
					i = setSymbol(i, symType);
					tokenStream[tokenStreamLength].symbol = i;
				}
			} else if(name == TOKNAME_SIGMA){
				tokenStream[tokenStreamLength].symbol = 0;
			} else {
				tokenStream[tokenStreamLength].symbol = -1;
			}
			tokenStream[tokenStreamLength].name = name;
			tokenStreamLength++;
			in += length;
		} else {
			return -1;
		}
	}
	return 0;
}
