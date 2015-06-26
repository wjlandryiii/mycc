/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct pattern {
	int ignore;
	char *name;
	char *patternString;
};

struct pattern patterns[128];
int patternCount = 0;

int addPattern(int ignore, char *name, char *pattern){
	patterns[patternCount].ignore = ignore;
	patterns[patternCount].name = strdup(name);
	patterns[patternCount].patternString = strdup(pattern);
	patternCount += 1;
	return 0;
}

int load(FILE *f){
	char line[1024];
	int lineNumber;
	char *name;
	char *pattern;
	int ignore;
	char *p;

	lineNumber = 0;
	while(fgets(line, sizeof(line), f)){
		lineNumber += 1;
		ignore = 0;
		if(line[0] == '#' || line[0] == '\n'){
			continue;
		} else {
			name = line;
			if(*name == '-'){
				ignore = 1;
				name++;
			}
			p = name;
			while(*p && *p != ':'){
				p++;
			}
			if(*p != ':'){
				fprintf(stderr, "missing ':' on line %d\n", lineNumber);
			}
			*p++ = '\0';
			while(*p && (*p == ' ' || *p == '\t')){
				p++;
			}
			pattern = p;
			while(*p && *p != '\n'){
				p++;
			}
			*p = '\0';
			addPattern(ignore, name, pattern);
		}
	}
	return 0;
}

void printPattern(struct pattern *p){
	printf("%c %s : %s\n", p->ignore ? '-' : ' ', p->name, p->patternString);
}

void printPatterns(){
	int i;

	for(i = 0; i < patternCount; i++){
		printPattern(&patterns[i]);
	}
}

void test(){
	FILE *f;
	int i;

	f = fopen("tokens.txt", "r");
	load(f);
	printPatterns();
}

int main(int argc, char *argv[]){
	test();
	return 0;
}
