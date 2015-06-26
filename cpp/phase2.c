/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "phase1.h"
#include "phase2.h"


int phase2Init(struct phase2 *p2){
	p2->lookAhead[0] = phase1NextChar(p2->p1);
	p2->lookAhead[1] = phase1NextChar(p2->p1);
	p2->spliceCount = 0;
	return 0;
}

static int eat(struct phase2 *p2){
	int c;

	c = p2->lookAhead[0];
	p2->lookAhead[0] = p2->lookAhead[1];
	p2->lookAhead[1] = phase1NextChar(p2->p1);
	return c;
}

int phase2NextChar(struct phase2 *p2){
	if(p2->lookAhead[0] == '\\' && p2->lookAhead[1] == '\n'){
		p2->spliceCount += 1;
		eat(p2);
		eat(p2);
		return phase2NextChar(p2);
	} else if(p2->lookAhead[0] == '\n' && p2->spliceCount > 0){
		p2->spliceCount -= 1;
		return '\n';
	} else {
		return eat(p2);
	}
}


#ifdef TESTING

int main(int argc, char *argv[]){
	int c;
	struct phase1 p1;
	struct phase2 p2;

	p1.sourceFile = fopen("tests/shellcode.c", "r");
	phase1Init(&p1);

	p2.p1 = &p1;
	phase2Init(&p2);

	while((c = phase2NextChar(&p2)) != EOF){
		putchar(c);
	}
	return 0;
}

#endif
