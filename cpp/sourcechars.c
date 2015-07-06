/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sourcechars.h"
#include "list.h"


static int emitSourceChar(struct list *list, int c, char *fileName, int line, int column){
	struct source_char *sc;

	sc = calloc(1, sizeof(*sc));
	sc->c = c;
	sc->fileName = fileName;
	sc->lineNumber = line;
	sc->columnNumber = column;
	listEnqueue(list, sc);
	return 0;
}


struct list *sourceCharsFromFile(char *fileName){
	struct list *list;
	struct source_char *sc;
	FILE *f;
	int c;
	int lineNumber;
	int columnNumber;

	list = newList();
	assert(list != NULL);
	lineNumber = 0;
	columnNumber = 0;
	f = fopen(fileName, "r");
	assert(f != NULL);

	while((c = fgetc(f)) != EOF){
		emitSourceChar(list, c, fileName, lineNumber, columnNumber);

		if(c == '\n'){
			lineNumber += 1;
			columnNumber = 0;
		} else {
			columnNumber += 1;
		}
	}
	emitSourceChar(list, c, fileName, lineNumber, columnNumber);
	return list;
}


#ifdef TESTING

void test_sourceCharStreamFile(){
	struct list *list;
	struct source_char *sc;

	list = sourceCharsFromFile("tests/helloworld.c");

	while(listDequeue(list, (void**)&sc) == 0){
		if(sc->c > 0){
			printf("%c", sc->c);
		}
	}

}

int main(int argc, char *argv[]){
	test_sourceCharStreamFile();
	//test_SourceCharStreamString();
	return 0;
}

#endif
