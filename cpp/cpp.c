/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "lexer.h"
#include "escstr.h"


struct source_line {
	struct source_line *next;
	char *s;
	int lineNumber;
};




int convertTrigraphs(char *s){
	int count;
	int c;
	char *p;

	count = 0;
	while(*s){
		if(count == 2){
			c = 0;
			if(*s == '='){
				c = '#';
			} else if(*s == '/'){
				c = '\\';
			} else if(*s == '`'){
				c = '^';
			} else if(*s == '('){
				c = '[';
			} else if(*s == ')'){
				c = ']';
			} else if(*s == '!'){
				c = '|';
			} else if(*s == '<'){
				c = '{';
			} else if(*s == '>'){
				c = '}';
			} else if(*s == '-'){
				c = '~';
			}
			if(c != 0){
				p = s - 2;
				*p = c;
				while(*p){
					p++;
					*p = *(p+2);
				}
			} else if(*s != '?'){
				count = 0;
			}
		} else {
			if(*s == '?'){
				count += 1;
			} else {
				count = 0;
			}
		}
		s++;
	}
	return 0;
}

void testTrigraph(){
	char source[] = "?"\
			 "??" "="\
			 "??" "/"\
			 "??" "`"\
			 "??" "("\
			 "??" ")"\
			 "??" "!"\
			 "??" "<"\
			 "??" ">"\
			 "??" "-";

	convertTrigraphs(source);
	printf("%s\n", source);
}



struct source_line *loadFile(char *filename){
	FILE *f;
	struct source_line *firstLine;
	struct source_line *line;
	struct source_line **pointer;
	char buff[1024];

	f = fopen(filename, "r");
	assert(f != NULL);

	firstLine = NULL;

	pointer = &firstLine;

	while(fgets(buff, sizeof(buff), f)){
		line = calloc(1, sizeof(*line));
		assert(line != NULL);
		*pointer = line;
		pointer = &line->next;

		line->s = malloc(strlen(buff)+1);
		assert(line->s != NULL);
		strcpy(line->s, buff);
	}

	return firstLine;
}


void printLines(struct source_line *line){
	while(line){
		printf("%s", line->s);
		line = line->next;
	}
}

struct token {
	struct token *next;
	int type;
	char *lexeme;
};

struct token *newToken(int type, char *lexeme, struct token *next){
	struct token *token = calloc(1, sizeof(*token));

	assert(token != 0);

	token->type = type;
	token->lexeme = lexeme;
	token->next = next;
	return token;
}


struct token *tokenizeFile(char *fileName){
	FILE *f = fopen(fileName, "r");

	assert(f != 0);

	int c;

	struct token *firstToken;
	struct token **ptoken;
	char buf[1024];
	int i;

	firstToken = 0;
	ptoken = &firstToken;
	while((c = fgetc(f)) != EOF){
		buf[0] = c;
		buf[1] = '\0';
		*ptoken = newToken(0, strdup(buf), 0);
		ptoken = &(*ptoken)->next;
	}
	return firstToken;
}

int outputTokens(struct token *token, FILE *f){
	while(token != 0){
		fputs(token->lexeme, f);
		token = token->next;
	}
	return 0;
}

void convertTrigraphsFromLines(struct source_line *line){
	while(line){
		convertTrigraphs(line->s);
		line = line->next;
	}
}


void spliceLines(struct source_line *line){
	int len;
	struct source_line *nextLine;
	while(line){
		len = strlen(line->s);
		if(len > 1){
			nextLine = line->next;
			while(nextLine){
				len = strlen(line->s);
				if(line->s[len-2] == '\\' && line->s[len-1] == '\n'){
					line->s = realloc(line->s, (len-1) + (strlen(nextLine->s)-1) + 1);
					assert(line->s != 0);
					line->s[len-2] = '\0';
					strcat(line->s, nextLine->s);
					free(nextLine->s);
					nextLine->s = strdup("\n");
					assert(nextLine->s != 0);
				}
				nextLine = nextLine->next;
			}
		}
		line = line->next;
	}
}

char *clipString(char *s, int length){
	char *clip;
	char *p;

	clip = malloc(length+1);
	assert(clip != 0);

	memset(clip, 0, length+1);

	p = clip;

	for(; length > 0; length--){
		*p++ = *s++;
	}
	*p = '\0';
	return clip;
}


struct token *tokenizeLine(char *s){
	int length;
	int type;
	struct token *firstToken;
	struct token **pToken;
	char *lexeme;

	firstToken = 0;
	pToken = &firstToken;

	while(!nextLexeme(s, &length, &type)){
		lexeme = clipString(s, length);
		*pToken = newToken(type, lexeme, 0);
		pToken = &((*pToken)->next);
		s += length;
	}
	return firstToken;
}


struct token *tokenizeLines(struct source_line *line){
	struct token *tokenList;
	struct token **pToken;

	tokenList = 0;
	pToken = &tokenList;

	while(line){
		*pToken = tokenizeLine(line->s);

		while(*pToken){
			pToken = &((*pToken)->next);
		}

		line = line->next;
	}
	return tokenList;
}

void printTokens(struct token *tokenList){
	while(tokenList){
		printf("Token: \"");
		puts_escaped(tokenList->lexeme, tokenList->lexeme+strlen(tokenList->lexeme));
		printf("\"\n");
		tokenList = tokenList->next;
	}
}

int main(int argc, char *argv[]){
	struct source_line *lines;
	struct token *tokenList;

	lines = loadFile("tests/test.c");
	convertTrigraphsFromLines(lines);
	spliceLines(lines);
	//printLines(lines);
	tokenList = tokenizeLines(lines);

	printTokens(tokenList);

	return 0;
}
