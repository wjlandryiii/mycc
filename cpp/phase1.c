/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "phase1.h"

int phase1Init(struct phase1 *p){
	p->lookAhead[0] = fgetc(p->sourceFile);
	p->lookAhead[1] = fgetc(p->sourceFile);
	p->lookAhead[2] = fgetc(p->sourceFile);
	return 0;
}

static int eat(struct phase1 *p){
	int c;

	c = p->lookAhead[0];
	p->lookAhead[0] = p->lookAhead[1];
	p->lookAhead[1] = p->lookAhead[2];
	p->lookAhead[2] = fgetc(p->sourceFile);
	return c;
}

int phase1NextChar(struct phase1 *p){
	if(p->lookAhead[0] == '\r' && p->lookAhead[1] == '\n'){
		eat(p);
		return eat(p);
	} else if(p->lookAhead[0] == '\?' && p->lookAhead[1] == '\?'){
		if(p->lookAhead[2] == '='){
			eat(p); eat(p); eat(p);
			return '#';
		} else if(p->lookAhead[2] == '/'){
			eat(p); eat(p); eat(p);
			return '\\';
		} else if(p->lookAhead[2] == '\''){
			eat(p); eat(p); eat(p);
			return '^';
		} else if(p->lookAhead[2] == '('){
			eat(p); eat(p); eat(p);
			return '[';
		} else if(p->lookAhead[2] == ')'){
			eat(p); eat(p); eat(p);
			return ']';
		} else if(p->lookAhead[2] == '!'){
			eat(p); eat(p); eat(p);
			return '|';
		} else if(p->lookAhead[2] == '<'){
			eat(p); eat(p); eat(p);
			return '{';
		} else if(p->lookAhead[2] == '>'){
			eat(p); eat(p); eat(p);
			return '}';
		} else if(p->lookAhead[2] == '-'){
			eat(p); eat(p); eat(p);
			return '~';
		} else {
			return eat(p);
		}
	} else {
		return eat(p);
	}
}

#ifdef TESTING

int main(int argc, char *argv[]){
	struct phase1 phase1;
	int c;

	phase1.sourceFile = fopen("tests/trigraphs.c", "r");
	phase1Init(&phase1);

	while((c = phase1NextChar(&phase1)) != EOF){
		putchar(c);
	}
	return 0;
}

#endif
