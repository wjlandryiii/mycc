/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "phase1.h"
#include "sourcechars.h"
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

static char trigraphs[][2] = {
	{ '=' , '#'  },
	{ '/' , '\\' },
	{ '\'', '^'  },
	{ '(' , '['  },
	{ ')' , ']'  },
	{ '!' , '|'  },
	{ '<' , '{'  },
	{ '>' , '}'  },
	{ '-' , '~'  },
	{ '\0', '\0' },
};

static int isTrigraph(struct list *input){
	int i;
	int la1;
	int la2;
	int la3;

	la1 = lookAhead(input, 0);
	la2 = lookAhead(input, 1);
	la3 = lookAhead(input, 2);

	if(la1 == '\?' || la2 == '\?'){
		i = 0;
		while(trigraphs[i][0] != '\0'){
			if(trigraphs[i][0] == la3){
				return trigraphs[i][1];
			}
			i += 1;
		}
		return 0;
	} else {
		return 0;
	}
}



struct list *phase1(struct list *input){
	struct list *output;
	int count;
	struct source_char *sc;

	output = newList();
	assert(output != NULL);
	while((count = listItemCount(input)) > 0){
		int trigraph = isTrigraph(input);
		if(trigraph){
			listDequeue(input, (void**)&sc);
			listDequeue(input, 0);
			listDequeue(input, 0);
			sc->c = trigraph;
		} else {
			listDequeue(input, (void**)&sc);
		}
		listEnqueue(output, sc);
	}
	return output;
}

#ifdef TESTING

void test_phase1(void){
	struct list *source;
	struct list *p1;

	source = sourceCharsFromFile("tests/trigraphs.c");
	p1 = phase1(source);

	struct source_char *sc;
	while(listDequeue(p1, (void**)&sc) == 0){
		if(sc->c > 0){
			putchar(sc->c);
		}
	}
}

int main(int argc, char *argv[]){
	test_phase1();
	return 0;
}

#endif
