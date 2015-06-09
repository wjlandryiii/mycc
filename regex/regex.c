/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

/*
<regex>
	: <term> | <regex
	| <term>
	;

<term>
	: <factor>
	| {}
	;

<factor>
	: <base> <factorP>
	;

<factorP>
	: '*' factorP
	|
	;

<base>
	: <char>
	| '\' <char>
	| "(" <regex> ")"
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *inputString = "(.)aaa(bbb)";
int inputIndex = 0;
int lookAhead;

int nextToken(){
	lookAhead = inputString[inputIndex++];
	if(lookAhead < 0){
		fprintf(stderr, "only supports ASCII\n");
		exit(1);
	}
	return 0;
}

int isMetaChar(int c){
	switch(c){
		case '\\':
		case '\"':
		case '.':
		case '^':
		case '$':
		case '[':
		case ']':
		case '*':
		case '+':
		case '?':
		case '{':
		case '}':
		case '|':
		case '/':
			return 1;
		default:
			return 0;
	}
}



void elementary_re();
void group();
void any();
void eos();
void character();
void set();


void elementary_re(){
	printf("elementary_re(): %d\n", inputIndex);

	if(lookAhead == '('){
		group();
	} else if(lookAhead == '.'){
		//<any>
		any();
	} else if(lookAhead == '$'){
		// eos
		eos();
	} else if(!isMetaChar(lookAhead) || lookAhead == '\\'){
		// char
		character();
	} else if(lookAhead == '['){
		// set
		set();
	} else {
		fprintf(stderr, "invalid elementary_re\n");
		exit(1);
	}
}

void group(){
	printf("group(): %d\n", inputIndex);

	if(lookAhead == '('){
		nextToken();
		elementary_re();
		if(lookAhead == ')'){
			nextToken();
		} else {
			fprintf(stderr, "invalid group (2)\n");
			exit(1);
		}
	} else {
		fprintf(stderr, "invalid group (1)\n");
		exit(1);
	}
}

void any(){
	printf("any(): %d\n", inputIndex);

	if(lookAhead == '.'){
		nextToken();
	} else {
		fprintf(stderr, "invalid any\n");
		exit(1);
	}
}

void eos(){
	printf("eos(): %d\n", inputIndex);

	if(lookAhead == '$'){
		nextToken();
	} else {
		fprintf(stderr, "invalid eos\n");
		exit(1);
	}
}

void character(){
	printf("character(): %d\n", inputIndex);

	if(!isMetaChar(lookAhead)){
		nextToken();
	} else if(lookAhead == '\\'){
		nextToken();
		if(isMetaChar(lookAhead)){
			nextToken();
		} else {
			fprintf(stderr, "invalid character (2)\n");
			exit(1);
		}
	} else {
		fprintf(stderr, "invalid character (1)\n");
		exit(1);
	}
}

void set(){
	printf("set(): %d\n", inputIndex);

	fprintf(stderr, "not implemented: set\n");
	exit(1);
}

int main(int argc, char *argv[]){
	nextToken();
	elementary_re();
	printf("%d\n", inputIndex);
}
