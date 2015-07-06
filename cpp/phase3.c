/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sourcechars.h"
#include "phase2.h"
#include "phase3.h"
#include "stringbuf.h"
#include "list.h"
#include "tokens.h"



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

int isPunctuator(int c){
	switch(c){
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
		case '&':
		case '*':
		case '+':
		case '-':
		case '~':
		case '!':
		case '/':
		case '%':
		case '<':
		case '>':
		case '^':
		case '|':
		case '=':
		case ',':
		case '#':
		case '.':
		case '?':
		case ':':
		case ';':
			return 1;
		default:
			return 0;
	};
}


/*
	( ) [ ] { }
	& * + - ~ !
	/ % < > ^ |
	= , # .
	? : ;
	->
	++ --
	<< >> <= >= == != && ||
	*= /= %= += -= &= ^= |=
	##
	<: :> <% %> %:
	<<= >>=
	...
	%:%:
*/


int punctuator_l1[][2] = {
	{ '(', PPTN_LPAREN },
	{ ')', PPTN_RPAREN },
	{ '[', PPTN_LBRACKET },
	{ ']', PPTN_RBRACKET },
	{ '{', PPTN_LBRACE },
	{ '}', PPTN_RBRACE },
	{ '.', PPTN_PERIOD },
	{ '&', PPTN_AMPERSAND },
	{ '*', PPTN_ASTERISK },
	{ '+', PPTN_PLUS },
	{ '-', PPTN_MINUS },
	{ '~', PPTN_TILDE },
	{ '!', PPTN_BANG },
	{ '/', PPTN_SLASH },
	{ '%', PPTN_PERCENT },
	{ '<', PPTN_LCHEVRON },
	{ '>', PPTN_RCHEVRON },
	{ '^', PPTN_CIRCUMFLEX },
	{ '|', PPTN_PIPE },
	{ '=', PPTN_EQUAL },
	{ ',', PPTN_COMMA },
	{ '#', PPTN_HASH },
	{ '?', PPTN_QUESTION },
	{ ':', PPTN_COLON },
	{ ';', PPTN_SEMICOLON },
	{ -1, -1},
};

int punctuator_l2[][3] = {
	{ '-', '>', PPTN_MINUS_RCHEVRON },
	{ '+', '+', PPTN_PLUS_PLUS },
	{ '-', '-', PPTN_MINUS_MINUS },
	{ '<', '<', PPTN_LCHEVRON_LCHEVRON },
	{ '>', '>', PPTN_RCHEVRON_RCHEVRON },
	{ '<', '=', PPTN_LCHEVRON_EQUAL },
	{ '>', '=', PPTN_RCHEVRON_EQUAL },
	{ '=', '=', PPTN_EQUAL_EQUAL },
	{ '!', '=', PPTN_BANG_EQUAL },
	{ '&', '&', PPTN_AMPERSAND_AMPERSAND },
	{ '|', '|', PPTN_PIPE_PIPE },
	{ '*', '=', PPTN_ASTERISK_EQUAL },
	{ '/', '=', PPTN_SLASH_EQUAL },
	{ '%', '=', PPTN_PERCENT_EQUAL },
	{ '+', '=', PPTN_PLUS_EQUAL },
	{ '-', '=', PPTN_MINUS_EQUAL },
	{ '&', '=', PPTN_AMPERSAND_EQUAL },
	{ '^', '=', PPTN_CIRCUMFLEX_EQUAL },
	{ '|', '=', PPTN_PIPE_EQUAL },
	{ '#', '#', PPTN_HASH_HASH },
	{ '<', ':', PPTN_LBRACKET },
	{ ':', '>', PPTN_RBRACKET },
	{ '<', '%', PPTN_LBRACE },
	{ '%', '>', PPTN_RBRACE },
	{ '%', ':', PPTN_HASH },
	{ -1, -1, -1},
};

int punctuator_l3[][4] = {
	{ '<', '<', '=', PPTN_LCHEVRON_LCHEVRON_EQUAL },
	{ '>', '>', '=', PPTN_RCHEVRON_RCHEVRON_EQUAL },
	{ '.', '.', '.', PPTN_PERIOD_PERIOD_PERIOD },
	{ -1, -1, -1, -1},
};

int punctuator_l4[][5] = {
	{ '%', ':', '%', ':', PPTN_HASH_HASH },
	{ -1, -1, -1, -1, -1},
};

struct directive {
	char *lexeme;
	int name;
};

struct directive directives[] = {
	{"if", PPTN_IF},
	{"ifdef", PPTN_IFDEF},
	{"ifndef", PPTN_IFNDEF},
	{"elif", PPTN_ELIF},
	{"else", PPTN_ELSE},
	{"endif", PPTN_ENDIF},
	{"include", PPTN_INCLUDE},
	{"define", PPTN_DEFINE},
	{"undef", PPTN_UNDEF},
	{"line", PPTN_LINE},
	{"error", PPTN_ERROR},
	{"pragma", PPTN_PRAGMA},
	{NULL, 0},
};



static int lookAhead(struct list *input, int i){
	int count;
	struct source_char *sc;

	count = listItemCount(input);
	if(i < count){
		listItemAtIndex(input, i, (void**)&sc);
		return sc->c;
	} else {
		return -1;
	}
}

static char *charTokensToSZ(struct list *input){
	struct source_char *sc;
	int count;
	char *str;
	char *p;

	count = listItemCount(input);
	str = malloc(count+1);
	assert(str != NULL);

	p = str;
	while(!listDequeue(input, (void**)&sc)){
		// TODO: memory leak here?
		*p = sc->c;
		p++;
	}
	*p = '\0';
	return str;
}


static int consumeWhiteSpaceAndComments(struct list *input, char **str, int *newLine){
	struct source_char *sc;
	int la1, la2, la3, la4;
	struct list *whiteSpaceList;

	*newLine = 0;
	whiteSpaceList = newList();
	assert(whiteSpaceList != NULL);

	la1 = lookAhead(input, 0);
	la2 = lookAhead(input, 1);
	la3 = lookAhead(input, 2);
	la4 = lookAhead(input, 3);
	while(la1==' ' || la1=='\t' || la1=='\n' || (la1=='/' && (la2=='/' || la2=='*'))){
		if(la1 == ' ' || la1 == '\t' || la1 == '\n'){
			while(la1 != -1 && (la1 == ' ' || la1 == '\t' || la1 == '\n')){
				if(la1 == '\n'){
					*newLine = 1;
				}
				listDequeue(input, (void**)&sc);
				listEnqueue(whiteSpaceList, sc);
				la1 = lookAhead(input, 0);
			}
		} else if(la1 == '/' && la2 == '/'){
			listDequeue(input, (void**)&sc);
			sc->c = ' ';
			listEnqueue(whiteSpaceList, (void**)&sc);
			listDequeue(input, 0);
			la1 = lookAhead(input, 0);
			while(la1 != -1 && la1 != '\n'){
				listDequeue(input, 0);
				la1 = lookAhead(input, 0);
			}
			if(la1 == '\n'){
				listDequeue(input, (void**)&sc);
				listEnqueue(whiteSpaceList, sc);
				*newLine = 1;
			}
		} else if(la1 == '/' && la2 == '*'){
			listDequeue(input, (void**)&sc);
			sc->c = ' ';
			listEnqueue(whiteSpaceList, sc);
			listDequeue(input, 0);
			la1 = lookAhead(input, 0);
			la2 = lookAhead(input, 1);
			while(la1 != -1 && !(la1 == '*' && la2 == '/')){
				listDequeue(input, 0);
				la1 = lookAhead(input, 0);
				la2 = lookAhead(input, 1);
			}
			if(la1 == -1){
				fprintf(stderr, "source can not end in unmatched comment\n");
				exit(1);
			} else if(la1 == '*' && la2 == '/'){
				listDequeue(input, 0);
				listDequeue(input, 0);
			}
		}
		la1 = lookAhead(input, 0);
		la2 = lookAhead(input, 1);
		la3 = lookAhead(input, 2);
		la4 = lookAhead(input, 3);
	}

	*str = charTokensToSZ(whiteSpaceList);
	return 0;
}

static int consumeLexeme(struct list *input, int hotForHeaderName, char **str, int *name){
	struct source_char *sc;
	int la1, la2, la3, la4;
	struct list *lexemeCharList;
	int i;

	lexemeCharList = newList();
	assert(lexemeCharList != NULL);

	la1 = lookAhead(input, 0);
	la2 = lookAhead(input, 1);
	la3 = lookAhead(input, 2);
	la4 = lookAhead(input, 3);

	if(la1 == -1){
		*name = PPTN_EOF;
		listDequeue(input, (void**)&sc);
		listEnqueue(lexemeCharList, sc);
	} else if(hotForHeaderName && (la1 == '\"' || la1 == '<')){
		*name = PPTN_HEADERNAME;
		int delimiter;
		delimiter = la1 == '<' ? '>' : '\"';
		listDequeue(input, (void**)&sc);
		listEnqueue(lexemeCharList, sc);
		la1 = lookAhead(input, 0);
		while(la1 != -1 && la1 != delimiter && la1 != '\n'){
			listDequeue(input, (void**)&sc);
			listEnqueue(lexemeCharList, sc);
			la1 = lookAhead(input, 0);
		}
		if(la1 == delimiter){
			listDequeue(input, (void**)&sc);
			listEnqueue(lexemeCharList, sc);
		} else {
			fprintf(stderr, "header name expected delimiter: %c\n", delimiter);
			exit(1);
		}
	} else if((isAlpha(la1) && !(la1 == 'L' && (la2 == '\"' || la2 == '\''))) || la1 == '_'){
		*name = PPTN_IDENTIFIER;
		while(isAlpha(la1) || isDigit(la1) || la1 == '_'){
			listDequeue(input, (void**)&sc);
			listEnqueue(lexemeCharList, sc);
			la1 = lookAhead(input, 0);
		}
	} else if(isDigit(la1) || (la1 == '.' && isDigit(la2))){
		*name = PPTN_PPNUMBER;
		while(isDigit(la1) || isAlpha(la1) || la1 == '.' || la1 == '_'){
			if((la1=='e' || la1=='E' || la1=='p' || la1=='P') && (la2=='+' || la2=='-')){
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
			} else {
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
			}
			la1 = lookAhead(input, 0);
			la2 = lookAhead(input, 1);
		}
	} else if(la1 == '\'' || (la1 == 'L' && la2 == '\'')){
		*name = PPTN_CHARACTERCONSTANT;
		listDequeue(input, (void**)&sc);
		listEnqueue(lexemeCharList, sc);
		la1 = lookAhead(input, 0);
		la2 = lookAhead(input, 1);
		while(la1 != -1 && la1 != '\n' && la1 != '\''){
			if(la1 == '\\' && (la2 == '\'' || la2 == '\\')){
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
			} else {
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
			}
			la1 = lookAhead(input, 0);
			la2 = lookAhead(input, 1);
		}
		if(la1 == '\''){
			listDequeue(input, (void**)&sc);
			listEnqueue(lexemeCharList, sc);
		} else {
			fprintf(stderr, "Expected \' at end of character literal\n");
			exit(1);
		}
	} else if(la1 == '\"' || (la1 == 'L' && la2 == '\"')){
		*name = PPTN_STRINGLITERAL;
		listDequeue(input, (void**)&sc);
		listEnqueue(lexemeCharList, sc);
		la1 = lookAhead(input, 0);
		la2 = lookAhead(input, 1);
		while(la1 != -1 && la1 != '\n' && la1 != '\"'){
			if(la1 == '\\' && (la2 == '\"' || la2 == '\\')){
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
			} else {
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
			}
			la1 = lookAhead(input, 0);
			la2 = lookAhead(input, 1);
		}
		if(la1 == '\"'){
			listDequeue(input, (void**)&sc);
			listEnqueue(lexemeCharList, sc);
		} else {
			fprintf(stderr, "Expected \" at end of string literal\n");
			exit(1);
		}
	} else if(isPunctuator(la1)){
		*name = -1;
		i = 0;
		while(*name < 0 && punctuator_l4[i][0] > 0){
			if(la1 == punctuator_l4[i][0]
					&& la2 == punctuator_l4[i][1]
					&& la3 == punctuator_l4[i][2]
					&& la4 == punctuator_l4[i][3]){
				*name = punctuator_l4[i][4];
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				break;
			}
			i += 1;
		}

		i = 0;
		while(*name < 0 && punctuator_l3[i][0] > 0){
			if(la1 == punctuator_l3[i][0]
					&& la2 == punctuator_l3[i][1]
					&& la3 == punctuator_l3[i][2]){
				*name = punctuator_l3[i][3];
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				break;
			}
			i += 1;
		}

		i = 0;
		while(*name < 0 && punctuator_l2[i][0] > 0){
			if(la1 == punctuator_l2[i][0]
					&& la2 == punctuator_l2[i][1]){
				*name = punctuator_l2[i][2];
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				break;
			}
			i += 1;
		}

		i = 0;
		while(*name < 0 && punctuator_l1[i][0] > 0){
			if(la1 == punctuator_l1[i][0]){
				*name = punctuator_l1[i][1];
				listDequeue(input, (void**)&sc);
				listEnqueue(lexemeCharList, sc);
				break;
			}
			i += 1;
		}

		if(*name < 0){
			fprintf(stderr, "unknown punctuator: %c\n", la1);
		}
	} else { // Other
		*name = PPTN_OTHER;
		listDequeue(input, (void**)&sc);
		listEnqueue(lexemeCharList, sc);
	}

	*str = charTokensToSZ(lexemeCharList);

	return 0;
}


struct list *phase3(struct list *input){
	struct source_char *sc;
	int la1, la2, la3, la4;
	struct list *output;
	int name;
	int i;
	int hotForDirective;
	int hotForInclude;

	output = newList();
	assert(output != NULL);

	// get whitespace and convert comments

	struct pptoken token;
	struct pptoken *ptoken;

	hotForDirective = 0;
	hotForInclude = 0;
	while(listItemCount(input) > 0){
		consumeWhiteSpaceAndComments(input, &token.whiteSpace, &token.hasNewLine);
		consumeLexeme(input, hotForInclude, &token.lexeme, &token.name);

		hotForInclude = 0;
		if(hotForDirective){
			if(token.name == PPTN_IDENTIFIER && !token.hasNewLine){
				for(i = 0; directives[i].lexeme; i++){
					if(strcmp(token.lexeme, directives[i].lexeme) == 0){
						token.name = directives[i].name;
						break;
					}
				}
				if(token.name == PPTN_INCLUDE){
					hotForInclude = 1;
				}
			}
			hotForDirective = 0;
		}

		if(token.name == PPTN_HASH && token.hasNewLine){
			hotForDirective = 1;
		}

		ptoken = calloc(1, sizeof(*ptoken));
		memcpy(ptoken, &token, sizeof(*ptoken));
		listEnqueue(output, ptoken);
	}

	return output;
}




#ifdef TESTING

#include "phase1.h"
#include "escstr.h"

void test_phase3(void){
	struct pptoken *token;
	struct list *source;
	struct list *p1;
	struct list *p2;
	struct list *p3;

	source = sourceCharsFromFile("tests/trigraphs.c");
	p1 = phase1(source);
	p2 = phase2(p1);
	p3 = phase3(p2);

	while(listDequeue(p3, (void**)&token) == 0){
		fputs(token->whiteSpace, stdout);
		if(token->name != PPTN_EOF){
			putchar('{');
			printf("\033[35m");
			fputs(token->lexeme, stdout);
			printf("\033[39m");
			putchar('}');
		} else {
			break;
		}
	}
}

int main(int argc, char *argv[]){
	test_phase3();
	return 0;
}
#endif
