/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sourcechars.h"
#include "phase1.h"
#include "phase2.h"

#include "list.h"

#if 0
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
#endif

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

struct list *phase2(struct list *input){
	struct list *output;
	struct source_char *sc;
	int count;

	output = newList();
	assert(output != NULL);

	while((count = listItemCount(input)) > 0){
		if(2 <= count && lookAhead(input, 0) == '\\' && lookAhead(input, 1) == '\n'){
			listDequeue(input, 0);
			listDequeue(input, 0);
		} else if(2 <= count && lookAhead(input, 0) != '\n' && lookAhead(input, 1) == -1){
			fprintf(stderr, "Last line must end in '\\n'\n");
			exit(1);
		} else {
			listDequeue(input, (void**)&sc);
			listEnqueue(output, sc);
		}
	}
	return output;
}



#ifdef TESTING

#if 0
void test_phase2(void){
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
}
#endif

void test_phase2(void){
	struct list *source;
	struct list *p1;
	struct list *p2;

	source = sourceCharsFromFile("tests/trigraphs.c");
	p1 = phase1(source);
	p2 = phase2(p1);

	struct source_char *sc;
	while(listDequeue(p2, (void**)&sc) == 0){
		if(sc->c > 0){
			putchar(sc->c);
		}
	}
}

int main(int argc, char *argv[]){
	test_phase2();
	return 0;
}

#endif
