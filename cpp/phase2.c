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
