/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>

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

char *LEXEMETYPE_STRINGS[] = {
	"Invalid",
	"WhiteSpace",
	"NewLine",
	"Number",
	"Identifier",
	"Character Constant",
	"String Literal",
	"Punctuation",
	NULL,
};



char *PUNCTUATION_STRINGS[] = {
	"(",
	")",
	",",
	";",
	"?",
	"[",
	"]",
	"{",
	"}",
	"~",
	"!",
	"!=",
	"%",
	"%:",
	"%=",
	"%>",
	"&",
	"&&",
	"&=",
	"*",
	"*=",
	"+",
	"++",
	"+=",
	"-",
	"--",
	"-=",
	"->",
	".",
	"...",
	"/",
	"/=",
	":",
	":>",
	"<",
	"<%",
	"<:",
	"<<",
	"<<=",
	"<=",
	"=",
	"==",
	">",
	">=",
	">>",
	">>=",
	"^",
	"^=",
	"|",
	"|=",
	"||",
	NULL,
};



enum CHARFLAGS {
	ENDOFINPUT	= 1 << 0,
	SOURCECHARSET	= 1 << 1,
	WHITESPACE	= 1 << 2,
	NEWLINE		= 1 << 3,
	PPNUMBERSTART	= 1 << 4,
	PPNUMBER	= 1 << 5,
	PPNUMBEREXP	= 1 << 6,
	PPNUMBEREXPSIGN	= 1 << 7,
	IDENTIFIERSTART	= 1 << 8,
	IDENTIFIER	= 1 << 9,
	CHARCONSTDELIM	= 1 << 10,
	CHARCONSTINV	= 1 << 11,
	SINGLECHARPUNC	= 1 << 12,
	DIGIT		= 1 << 13,
};


static const int charbits[128] = {
/* 0x00: ^@   */	ENDOFINPUT,
/* 0x01: ^A   */	0,
/* 0x02: ^B   */	0,
/* 0x03: ^C   */	0,
/* 0x04: ^D   */	0,
/* 0x05: ^E   */	0,
/* 0x06: ^F   */	0,
/* 0x07: '\a' */	0,
/* 0x08: '\b' */	0,
/* 0x09: '\t' */	SOURCECHARSET | WHITESPACE,
/* 0x0a: '\n' */	SOURCECHARSET | NEWLINE | CHARCONSTINV,
/* 0x0b: '\v' */	0,
/* 0x0c: '\f' */	0,
/* 0x0d: '\r' */	0,
/* 0x0e: ^N   */	0,
/* 0x0f: ^O   */	0,
/* 0x10: ^P   */	0,
/* 0x11: ^Q   */	0,
/* 0x12: ^R   */	0,
/* 0x13: ^S   */	0,
/* 0x14: ^T   */	0,
/* 0x15: ^U   */	0,
/* 0x16: ^V   */	0,
/* 0x17: ^W   */	0,
/* 0x18: ^X   */	0,
/* 0x19: ^Y   */	0,
/* 0x1a: ^Z   */	0,
/* 0x1b: ^[   */	0,
/* 0x1c: ^\   */	0,
/* 0x1d: ^]   */	0,
/* 0x1e: ^^   */	0,
/* 0x1f: ^_   */	0,
/* 0x20: ' '  */	SOURCECHARSET | WHITESPACE,
/* 0x21: '!'  */	SOURCECHARSET,
/* 0x22: '\"' */	SOURCECHARSET,
/* 0x23: '#'  */	SOURCECHARSET,
/* 0x24: '$'  */	SOURCECHARSET,
/* 0x25: '%'  */	SOURCECHARSET,
/* 0x26: '&'  */	SOURCECHARSET,
/* 0x27: '\'' */	SOURCECHARSET | CHARCONSTDELIM | CHARCONSTINV,
/* 0x28: '('  */	SOURCECHARSET,
/* 0x29: ')'  */	SOURCECHARSET,
/* 0x2a: '*'  */	SOURCECHARSET,
/* 0x2b: '+'  */	SOURCECHARSET | PPNUMBEREXPSIGN,
/* 0x2c: ','  */	SOURCECHARSET,
/* 0x2d: '-'  */	SOURCECHARSET | PPNUMBEREXPSIGN,
/* 0x2e: '.'  */	SOURCECHARSET | PPNUMBERSTART | PPNUMBER,
/* 0x2f: '/'  */	SOURCECHARSET,
/* 0x30: '0'  */	SOURCECHARSET | PPNUMBERSTART | PPNUMBER | IDENTIFIER | DIGIT,
/* 0x31: '1'  */	SOURCECHARSET | PPNUMBERSTART | PPNUMBER | IDENTIFIER | DIGIT,
/* 0x32: '2'  */	SOURCECHARSET | PPNUMBERSTART | PPNUMBER | IDENTIFIER | DIGIT,
/* 0x33: '3'  */	SOURCECHARSET | PPNUMBERSTART | PPNUMBER | IDENTIFIER | DIGIT,
/* 0x34: '4'  */	SOURCECHARSET | PPNUMBERSTART | PPNUMBER | IDENTIFIER | DIGIT,
/* 0x35: '5'  */	SOURCECHARSET | PPNUMBERSTART | PPNUMBER | IDENTIFIER | DIGIT,
/* 0x36: '6'  */	SOURCECHARSET | PPNUMBERSTART | PPNUMBER | IDENTIFIER | DIGIT,
/* 0x37: '7'  */	SOURCECHARSET | PPNUMBERSTART | PPNUMBER | IDENTIFIER | DIGIT,
/* 0x38: '8'  */	SOURCECHARSET | PPNUMBERSTART | PPNUMBER | IDENTIFIER | DIGIT,
/* 0x39: '9'  */	SOURCECHARSET | PPNUMBERSTART | PPNUMBER | IDENTIFIER | DIGIT,
/* 0x3a: ':'  */	SOURCECHARSET,
/* 0x3b: ';'  */	SOURCECHARSET,
/* 0x3c: '<'  */	SOURCECHARSET,
/* 0x3d: '='  */	SOURCECHARSET,
/* 0x3e: '>'  */	SOURCECHARSET,
/* 0x3f: '\?' */	SOURCECHARSET,
/* 0x40: '@'  */	SOURCECHARSET,
/* 0x41: 'A'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x42: 'B'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x43: 'C'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x44: 'D'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x45: 'E'  */	SOURCECHARSET | PPNUMBER | PPNUMBEREXP | IDENTIFIERSTART | IDENTIFIER,
/* 0x46: 'F'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x47: 'G'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x48: 'H'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x49: 'I'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x4a: 'J'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x4b: 'K'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x4c: 'L'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x4d: 'M'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x4e: 'N'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x4f: 'O'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x50: 'P'  */	SOURCECHARSET | PPNUMBER | PPNUMBEREXP | IDENTIFIERSTART | IDENTIFIER,
/* 0x51: 'Q'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x52: 'R'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x53: 'S'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x54: 'T'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x55: 'U'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x56: 'V'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x57: 'W'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x58: 'X'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x59: 'Y'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x5a: 'Z'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x5b: '['  */	SOURCECHARSET,
/* 0x5c: '\\' */	SOURCECHARSET | CHARCONSTINV,
/* 0x5d: ']'  */	SOURCECHARSET,
/* 0x5e: '^'  */	SOURCECHARSET,
/* 0x5f: '_'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x60: '`'  */	SOURCECHARSET,
/* 0x61: 'a'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x62: 'b'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x63: 'c'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x64: 'd'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x65: 'e'  */	SOURCECHARSET | PPNUMBER | PPNUMBEREXP | IDENTIFIERSTART | IDENTIFIER,
/* 0x66: 'f'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x67: 'g'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x68: 'h'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x69: 'i'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x6a: 'j'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x6b: 'k'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x6c: 'l'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x6d: 'm'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x6e: 'n'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x6f: 'o'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x70: 'p'  */	SOURCECHARSET | PPNUMBER | PPNUMBEREXP | IDENTIFIERSTART | IDENTIFIER,
/* 0x71: 'q'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x72: 'r'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x73: 's'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x74: 't'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x75: 'u'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x76: 'v'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x77: 'w'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x78: 'x'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x79: 'y'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x7a: 'z'  */	SOURCECHARSET | PPNUMBER | IDENTIFIERSTART | IDENTIFIER,
/* 0x7b: '{'  */	SOURCECHARSET,
/* 0x7c: '|'  */	SOURCECHARSET,
/* 0x7d: '}'  */	SOURCECHARSET,
/* 0x7e: '~'  */	SOURCECHARSET,
/* 0x7f: ^?   */	0,
};

static int charbitsTest(int c, int flags){
	if(0 <= c && c <= 127){
		return (charbits[c] & flags) == flags;
	} else {
		return 0;
	}
}

static int isEndOfInput(int c)		{ return charbitsTest(c, ENDOFINPUT); }
static int isSourceCharSet(int c)	{ return charbitsTest(c, SOURCECHARSET); }
static int isNewLine(int c)		{ return charbitsTest(c, NEWLINE); }
static int isWhiteSpace(int c)		{ return charbitsTest(c, WHITESPACE); }
static int isPPNumberStart(int c)	{ return charbitsTest(c, PPNUMBERSTART); }
static int isPPNumber(int c)		{ return charbitsTest(c, PPNUMBER); }
static int isPPNumberExp(int c)		{ return charbitsTest(c, PPNUMBEREXP); }
static int isPPNumberExpSign(int c)	{ return charbitsTest(c, PPNUMBEREXPSIGN); }
static int isIdentifierStart(int c)	{ return charbitsTest(c, IDENTIFIERSTART); }
static int isIdentifier(int c)		{ return charbitsTest(c, IDENTIFIER); }
static int isCharConstDelim(int c)	{ return charbitsTest(c, CHARCONSTDELIM); }
static int isCharConstInv(int c)	{ return charbitsTest(c, CHARCONSTINV); }
static int isDigit(int c)		{ return charbitsTest(c, DIGIT); }

static int isNumberBegining(int a, int b){
	if(a == '.'){
		if(isDigit(b)){
			return 1;
		} else {
			return 0;
		}
	} else if(isDigit(a)){
		return 1;
	} else {
		return 0;
	}
}


int nextLexeme(char *s, int *length, int *type){
	char *p = s;
	if(isEndOfInput(*p)){
		*length = 0;
		*type = LT_INV;
		return -1;
	} else if(isNewLine(*p)){
		*length = 1;
		*type = LT_NL;
		return 0;
	} else if(isWhiteSpace(*p)){
		while(isWhiteSpace(*p)){
			p++;
		}
		*length = p - s;
		*type = LT_WS;
		return 0;
	} else if(isNumberBegining(*p, *(p+1))){
		while(isPPNumber(*p)){
			if(isPPNumberExp(*p) && isPPNumberExpSign(*(p+1))){
				p += 2;
			} else {
				p++;
			}
		}
		*length = p - s;
		*type = LT_NUM;
		return 0;
	} else if(isIdentifierStart(*p)){
		while(isIdentifier(*p)){
			p++;
		}
		*length = p - s;
		*type = LT_ID;
		return 0;
	} else if(*p == '\''){
		p++;
		while(isSourceCharSet(*p) && *p != '\''){
			if(!(*p == '\\' && *(p+1) == '\'')){
				p++;
			} else {
				p += 2;
			}
		}
		if(*p == '\''){
			p++;
		} else {
			fprintf(stderr, "invalid token: %02x\n", *p);
			exit(1);
		}
		*length = p - s;
		*type = LT_CC;
		return 0;
	} else if(*p == '\"'){
		p++;
		while(isSourceCharSet(*p) && *p != '\"'){
			if(!(*p == '\\' && *(p+1) == '\"')){
				p++;
			} else {
				p += 2;
			}
		}
		if(*p == '\"'){
			p++;
		} else {
			fprintf(stderr, "invalid string literal token\n");
			exit(1);
		}
		*length = p - s;
		*type = LT_STRLIT;
		return 0;
	} else if(*p == '('){
		p++;
		*length = 1;
		*type = LT_PUNC;
		return 0;
	} else if(*p == ')'){
		p++;
		*length = 1;
		*type = LT_PUNC;
		return 0;
	} else if(*p == ','){
		p++;
		*length = 1;
		*type = LT_PUNC;
		return 0;
	} else if(*p == ';'){
		p++;
		*length = 1;
		*type = LT_PUNC;
		return 0;
	} else if(*p == '?'){
		p++;
		*length = 1;
		*type = LT_PUNC;
		return 0;
	} else if(*p == '['){
		p++;
		*length = 1;
		*type = LT_PUNC;
		return 0;
	} else if(*p == ']'){
		p++;
		*length = 1;
		*type = LT_PUNC;
		return 0;
	} else if(*p == '{'){
		p++;
		*length = 1;
		*type = LT_PUNC;
		return 0;
	} else if(*p == '}'){
		p++;
		*length = 1;
		*type = LT_PUNC;
		return 0;
	} else if(*p == '~'){
		p++;
		*length = 1;
		*type = LT_PUNC;
		return 0;
	} else if(*p == '!'){
		p++;
		if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '%'){
		p++;
		if(*p == ':'){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else if(*p == '>'){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '&'){
		p++;
		if(*p == '&'){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '*'){
		p++;
		if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '+'){
		p++;
		if(*p == '+'){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '-'){
		p++;
		if(*p == '-'){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else if(*p == '>'){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '.' && isDigit){
		p++;
		if(*p == '.' && *(p+1) == '.'){
			p += 2;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '/'){
		p++;
		if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == ':'){
		p++;
		if(*p == '>'){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '<'){
		p++;
		if(*p == '%'){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else if(*p == ':'){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else if(*p == '<'){
			p++;
			if(*p == '='){
				p++;
				*length = p - s;
				*type = LT_PUNC;
				return 0;
			} else {
				*length = p - s;
				*type = LT_PUNC;
				return 0;
			}
		} else if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '='){
		p++;
		if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '>'){
		p++;
		if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else if(*p == '>'){
			p++;
			if(*p == '='){
				p++;
				*length = p - s;
				*type = LT_PUNC;
				return 0;
			} else {
				*length = p - s;
				*type = LT_PUNC;
				return 0;
			}
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '^'){
		p++;
		if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else if(*p == '|'){
		p++;
		if(*p == '='){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else if(*p == '|'){
			p++;
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		} else {
			*length = p - s;
			*type = LT_PUNC;
			return 0;
		}
	} else {
		p++;
		*length = p - s;
		*type = LT_PUNC;
		return 0;

	}





	return 0;
}


#ifdef TESTING

#include <stdarg.h>
#include "escstr.h"


int test_nextLexeme(char *s, ...){
	int length;
	int type;
	int testType;
	va_list args;

	va_start(args, s);
	printf("[\n");
	while(!nextLexeme(s, &length, &type)){
		testType = va_arg(args, int);
		printf("\t{type:%d, typeName:\"%s\", lexeme:\"", type, LEXEMETYPE_STRINGS[type]);
		puts_escaped(s, s+length);
		printf("\"},\n");
		s += length;
		if(testType != type){
			printf("types don't match!\n");
			exit(1);
		}
	}
	printf("]\n");
	return 0;
}

int main(int argc, char *argv[]){
	int i;

	test_nextLexeme(" \t \t", LT_WS, LT_INV);
	test_nextLexeme("\n", LT_NL, LT_INV);
	test_nextLexeme("9329.23.2341e+43", LT_NUM, LT_INV);
	test_nextLexeme("FOOBAR", LT_ID, LT_INV);
	test_nextLexeme("'asd\\x22\\0sd'", LT_CC, LT_INV);
	test_nextLexeme("\"asdf\\\"fooot\"", LT_STRLIT, LT_INV);
	test_nextLexeme(" \t776ABC FOO", LT_WS,LT_NUM, LT_WS, LT_ID, LT_INV);

	i = 0;
	while(PUNCTUATION_STRINGS[i] != 0){
		test_nextLexeme(PUNCTUATION_STRINGS[i], LT_PUNC, LT_INV);
		i++;
	}

	test_nextLexeme(".45", LT_NUM);
	test_nextLexeme("FOO(10,24);\n", LT_ID, LT_PUNC, LT_NUM, LT_PUNC, LT_NUM,
			LT_PUNC, LT_PUNC, LT_NL, LT_INV);
	return 0;
}

#endif
