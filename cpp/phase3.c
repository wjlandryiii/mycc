/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "phase2.h"
#include "phase3.h"
#include "stringbuf.h"

#include "tokens.h"

enum LOOKBEHIND_TYPES {
	LB_DONTCARE,
	LB_HASH,
	LB_NEWLINE,
	LB_INCLUDE,
};

int phase3Init(struct phase3 *p3){
	p3->lb[0] = LB_DONTCARE;
	p3->lb[1] = LB_DONTCARE;
	p3->lb[2] = LB_DONTCARE;
	p3->la[0] = phase2NextChar(p3->p2);
	p3->la[1] = phase2NextChar(p3->p2);
	p3->la[2] = phase2NextChar(p3->p2);
	p3->la[3] = phase2NextChar(p3->p2);
	return 0;
}

static int eat(struct phase3 *p3){
	int c;
	c = p3->la[0];
	p3->la[0] = p3->la[1];
	p3->la[1] = p3->la[2];
	p3->la[2] = p3->la[3];
	p3->la[3] = phase2NextChar(p3->p2);
	return c;
}

/*
 * preprocessing-token:
 *	header-name
 *	identifier
 *	pp-number
 *	character-constant
 *	string-literal
 *	punctuator
 *	whitespace
 *	newline
 */

enum LEXEME_TYPES {
	LT_EMPTY,
	LT_HEADERNAME,
	LT_IDENTIFIER,
	LT_PPNUMBER,
	LT_CHARACTERCONSTANT,
	LT_STRINGLITERAL,
	LT_PUNCTUATOR,
	LT_WHITESPACE,
	LT_NEWLINE,
};

int isSourceChar(int c){
	if(c == '\n' || c == '\t'){
		return 1;
	} else if(' ' <= c && c <= '~'){
		return 1;
	} else {
		return 0;
	}
}
int isAlpha(int c){
	if('a' <= c && c <= 'z'){
		return 1;
	} else if('A' <= c && c <= 'Z'){
		return 1;
	} else {
		return 0;
	}
}
int isDigit(int c){
	if('0' <= c && c <= '9'){
		return 1;
	} else {
		return 0;
	}
}

struct pptoken phase3NextToken(struct phase3 *p3){
	//TODO: %:%: digraph  lookAhead must be 4;
#define la1 (p3->la[0])
#define la2 (p3->la[1])
#define la3 (p3->la[2])
#define la4 (p3->la[2])
	int type = 0;
	int name = 0;
	struct pptoken token;
	struct string_buf *sb;

	sb = newStringBuf();
	assert(sb != NULL);

	if(la1 == -1){
		type = LT_EMPTY;
		type = PPTN_EOF;
	} else if((la1 == '\"' || la1 == '<')
			&& (p3->lb[2] == LB_NEWLINE
				&& p3->lb[1] == LB_HASH
				&& p3->lb[0] == LB_INCLUDE)){
		int delim;
		type = LT_HEADERNAME;
		name = PPTN_HEADERNAME;
		delim = la1 == '\"' ? '\"' : '>';
		stringBufAppendChar(sb, eat(p3));
		while(isSourceChar(la1) && la1 != '\n' && la1 != delim){
			stringBufAppendChar(sb, eat(p3));
		}
		if(la1 == delim){
			stringBufAppendChar(sb, eat(p3));
		} else {
			fprintf(stderr, "invalid header-name token: "
					"expected \"FILENAME\" or <filename>\n");
			exit(1);
		}
	} else if((isAlpha(la1) || la1 == '_') && !(la1 == 'L' && la1 == '\"')){
		// TODO: preprocessing keywords
		type = LT_IDENTIFIER;
		name = PPTN_IDENTIFIER;
		stringBufAppendChar(sb, eat(p3));
		while(isAlpha(la1) || isDigit(la1) || la1 == '_'){
			stringBufAppendChar(sb, eat(p3));
		}
		if(p3->lb[0] == LB_HASH && p3->lb[1] == LB_NEWLINE){
			if(stringBufCompareSZ(sb, "if") == 0){
				name = PPTN_IF;
			} else if(stringBufCompareSZ(sb, "elif") == 0){
				name = PPTN_ELIF;
			} else if(stringBufCompareSZ(sb, "else") == 0){
				name = PPTN_ELSE;
			} else if(stringBufCompareSZ(sb, "line") == 0){
				name = PPTN_LINE;
			} else if(stringBufCompareSZ(sb, "endif") == 0){
				name = PPTN_ENDIF;
			} else if(stringBufCompareSZ(sb, "error") == 0){
				name = PPTN_ERROR;
			} else if(stringBufCompareSZ(sb, "ifdef") == 0){
				name = PPTN_IFDEF;
			} else if(stringBufCompareSZ(sb, "define") == 0){
				name = PPTN_DEFINE;
			} else if(stringBufCompareSZ(sb, "ifndef") == 0){
				name = PPTN_IFNDEF;
			} else if(stringBufCompareSZ(sb, "pragma") == 0){
				name = PPTN_PRAGMA;
			} else if(stringBufCompareSZ(sb, "include") == 0){
				name = PPTN_INCLUDE;
			}
		}
	} else if(isDigit(la1) || (la1 == '.' && isDigit(la2))){
		type = LT_PPNUMBER;
		name = PPTN_PPNUMBER;
		while(isDigit(la1) || isAlpha(la1) || la1 == '.'){
			if((la1 == 'e' || la1 == 'E' || la1 == 'p' || la1 == 'P')
					&& ( la2 == '-' || la2 == '+')){
				stringBufAppendChar(sb, eat(p3));
				stringBufAppendChar(sb, eat(p3));
			} else {
				stringBufAppendChar(sb, eat(p3));
			}
		}
	} else if(la1 == '\'' || (la1 == 'L' && la2 == '\'')){
		type = LT_CHARACTERCONSTANT;
		name = PPTN_CHARACTERCONSTANT;
		if(la1 == '\''){
			stringBufAppendChar(sb, eat(p3));
		} else {
			stringBufAppendChar(sb, eat(p3));
			stringBufAppendChar(sb, eat(p3));
		}
		while(isSourceChar(la1) && la1 != '\'' && la1 != '\n'){
			if(la1 == '\\' && la2 == '\\'){
				stringBufAppendChar(sb, eat(p3));
				stringBufAppendChar(sb, eat(p3));
			} else if(la1 == '\\' && la2 == '\''){
				stringBufAppendChar(sb, eat(p3));
				stringBufAppendChar(sb, eat(p3));
			} else {
				stringBufAppendChar(sb, eat(p3));
			}
		}
		if(la1 == '\''){
			stringBufAppendChar(sb, eat(p3));
		} else {
			fprintf(stderr, "invalid string-literal token: expected \'CHAR\'\n");
			exit(1);
		}
	} else if(la1 == '\"' || (la1 == 'L' && la2 == '\"')){
		type = LT_STRINGLITERAL;
		name = PPTN_STRINGLITERAL;
		if(la1 == '\"'){
			stringBufAppendChar(sb, eat(p3));
		} else {
			stringBufAppendChar(sb, eat(p3));
			stringBufAppendChar(sb, eat(p3));
		}
		while(isSourceChar(la1) && la1 != '\"' && la1 != '\n'){
			if(la1 == '\\' && la2 == '\\'){
				stringBufAppendChar(sb, eat(p3));
				stringBufAppendChar(sb, eat(p3));
			} else if(la1 == '\\' && la2 == '\"'){
				stringBufAppendChar(sb, eat(p3));
				stringBufAppendChar(sb, eat(p3));
			} else {
				stringBufAppendChar(sb, eat(p3));
			}
		}
		if(la1 == '\"'){
			stringBufAppendChar(sb, eat(p3));
		} else {
			fprintf(stderr, "invalid string-literal token: expected \"STRING\"\n");
			exit(1);
		}
	} else if(la1 == '!' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_NE_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '!'){
		type = LT_PUNCTUATOR;
		name = PPTN_EXLAMATIONMARK;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '#' && la2 == '#'){
		type = LT_PUNCTUATOR;
		name = PPTN_GLUE_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '#'){
		type = LT_PUNCTUATOR;
		name = PPTN_HASH;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '&' && la2 == '&'){
		type = LT_PUNCTUATOR;
		name = PPTN_AND_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '&' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_AND_ASSIGN,
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '&'){
		type = LT_PUNCTUATOR;
		name = PPTN_AMPERSAND;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '%' && la2 == ':' && la3 == '%' && la4 == ':'){
		type = LT_PUNCTUATOR;
		name = PPTN_GLUE_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '%' && la2 == ':'){
		type = LT_PUNCTUATOR;
		name = PPTN_HASH;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '%' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_MOD_ASSIGN;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '%' && la2 == '>'){
		// DIGRAPH
		type = LT_PUNCTUATOR;
		name = PPTN_RBRACE;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '%'){
		type = LT_PUNCTUATOR;
		name = PPTN_PERCENT;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '('){
		type = LT_PUNCTUATOR;
		name = PPTN_LPAREN;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == ')'){
		type = LT_PUNCTUATOR;
		name = PPTN_RPAREN;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '*' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_MUL_ASSIGN;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '*'){
		type = LT_PUNCTUATOR;
		name = PPTN_ASTERISK;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '+' && la2 == '+'){
		type = LT_PUNCTUATOR;
		name = PPTN_INC_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '+' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_ADD_ASSIGN;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '+'){
		type = LT_PUNCTUATOR;
		name = PPTN_PLUS_SIGN;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == ','){
		type = LT_PUNCTUATOR;
		name = PPTN_COMMA;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '-' && la2 == '-'){
		type = LT_PUNCTUATOR;
		name = PPTN_DEC_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '-' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_SUB_ASSIGN;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '-' && la2 == '>'){
		type = LT_PUNCTUATOR;
		name = PPTN_PTR_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '-'){
		type = LT_PUNCTUATOR;
		name = PPTN_MINUS_SIGN;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '.' && la2 == '.' && la3 == '.'){
		type = LT_PUNCTUATOR;
		name = PPTN_ELLIPSIS;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '.'){
		type = LT_PUNCTUATOR;
		name = PPTN_PERIOD;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '/' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_DIV_ASSIGN;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '/' && la2 == '/'){
		type = LT_WHITESPACE;
		name = PPTN_WHITESPACE;
		while(la1 > 0 && la1 != '\n'){
			eat(p3);
		}
		stringBufAppendChar(sb, ' ');
	} else if(la1 == '/' && la2 == '*'){
		type = LT_WHITESPACE;
		name = PPTN_WHITESPACE;
		while(la1 > 0 && (la1 != '*' || la2 != '/')){
			eat(p3);
		}
		eat(p3);
		eat(p3);
		stringBufAppendChar(sb, ' ');
		while(la1 == ' ' || la1 == '\t'){
			stringBufAppendChar(sb, eat(p3));
		}
	} else if(la1 == '/'){
		type = LT_PUNCTUATOR;
		name = PPTN_SLASH;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == ':' && la2 == '>'){
		type = LT_PUNCTUATOR;
		name = PPTN_RBRACKET;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == ':'){
		type = LT_PUNCTUATOR;
		name = PPTN_COLON;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == ';'){
		type = LT_PUNCTUATOR;
		name = PPTN_SEMICOLON;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '<' && la2 == '<' && la3 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_SL_ASSIGN;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '<' && la2 == '<'){
		type = LT_PUNCTUATOR;
		name = PPTN_SL_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '<' && la2 == '%'){
		type = LT_PUNCTUATOR;
		name = PPTN_LBRACE;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '<' && la2 == ':'){
		type = LT_PUNCTUATOR;
		name = PPTN_LBRACKET;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '<' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_LE_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '<'){
		type = LT_PUNCTUATOR;
		name = PPTN_LCHEVRON;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '=' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_EQ_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_EQUAL,
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '>' && la2 == '>' && la3 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_SR_ASSIGN;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '>' && la2 == '>'){
		type = LT_PUNCTUATOR;
		name = PPTN_SR_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '>' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_GE_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '>'){
		type = LT_PUNCTUATOR;
		name = PPTN_RCHEVRON;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '?'){
		type = LT_PUNCTUATOR;
		name = PPTN_QUESTION_MARK;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '['){
		type = LT_PUNCTUATOR;
		name = PPTN_LBRACKET;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == ']'){
		type = LT_PUNCTUATOR;
		name = PPTN_RBRACKET;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '^' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_XOR_ASSIGN;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '^'){
		type = LT_PUNCTUATOR;
		name = PPTN_CIRCUMFLEX;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '{'){
		type = LT_PUNCTUATOR;
		name = PPTN_LBRACE;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '|' && la2 == '='){
		type = LT_PUNCTUATOR;
		name = PPTN_OR_ASSIGN;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '|' && la2 == '|'){
		type = LT_PUNCTUATOR;
		name = PPTN_OR_OP;
		stringBufAppendChar(sb, eat(p3));
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '|'){
		type = LT_PUNCTUATOR;
		name = PPTN_PIPE;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '}'){
		type = LT_PUNCTUATOR;
		name = PPTN_RBRACE;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == '~'){
		type = LT_PUNCTUATOR;
		name = PPTN_TILDE;
		stringBufAppendChar(sb, eat(p3));
	} else if(la1 == ' ' || la1 == '\t'){
		type = LT_WHITESPACE;
		name = PPTN_WHITESPACE;
		while(la1 == ' ' || la1 == '\t'){
			stringBufAppendChar(sb, eat(p3));
		}
	} else if(la1 == '\n'){
		type = LT_NEWLINE;
		name = PPTN_NEWLINE;
		stringBufAppendChar(sb, eat(p3));
	} else if(isSourceChar(la1)){
		fprintf(stderr, "unknown lexeme with startchar: %c\n", la1);
		exit(1);
	} else {
		fprintf(stderr, "Character not in source character set: %02x\n", la1);
		exit(1);
	}



	token.type = type;
	token.name = name;
	token.lexeme = stringBufToSZ(sb);

	if(type != LT_WHITESPACE){
		p3->lb[2] = p3->lb[1];
		p3->lb[1] = p3->lb[0];
		if(type == LT_PUNCTUATOR && token.lexeme[0] == '#' && token.lexeme[1] == '\0'){
			p3->lb[0] = LB_HASH;
		} else if(type == LT_NEWLINE){
			p3->lb[0] = LB_NEWLINE;
		} else if(type == LT_IDENTIFIER && strcmp(token.lexeme, "include") == 0){
			p3->lb[0] = LB_INCLUDE;
		} else {
			p3->lb[0] = LB_DONTCARE;
		}
	}

	return token;

#undef la1
#undef la2
#undef la3
}


#ifdef TESTING

#include "phase1.h"
#include "escstr.h"

int main(int argc, char *argv[]){
	struct phase1 p1;
	struct phase2 p2;
	struct phase3 p3;
	struct pptoken token;

	//p1.sourceFile = fopen("tests/helloworld.c", "r");
	p1.sourceFile = fopen("phase3.c", "r");
	phase1Init(&p1);

	p2.p1 = &p1;
	phase2Init(&p2);

	p3.p2 = &p2;
	phase3Init(&p3);

	token = phase3NextToken(&p3);
	while(token.type != LT_EMPTY){
		printPPToken(token);
		token = phase3NextToken(&p3);
	}
	return 0;
}
#endif
