/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *inputString = "(.)+a(z|q)at?|a(bbbb)*";
int inputIndex = 0;
int lookAhead;

int nextToken(){
	lookAhead = inputString[inputIndex++];
	if(lookAhead < 0){
		fprintf(stderr, "only supports ASCII\n");
		exit(1);
	} else if(lookAhead != '\0'){
		printf("****  NEXT TOKEN: %c : %s $\n", lookAhead, &inputString[inputIndex]);
	} else {
		printf("****  NEXT TOKEN: $\n");
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

void parse_re();
void parse_union_();
void parse_simple_re();
void parse_concatenation();
void parse_basic_re();
void parse_basic_re_prime();
void parse_elementary_re();
void parse_group();
void parse_any();
void parse_eos();
void parse_character();
void parse_set();

void parse_re(){
	printf("<re> : <simple_re> <union_>\n");
	parse_simple_re();
	parse_union_();
}

void parse_union_(){
	if(lookAhead == '|'){
		printf("<union_> : '|' <simple_re> <union_>\n");
		nextToken();
		parse_simple_re();
		parse_union_();
	} else if(lookAhead != ')' && lookAhead != '\0'){
		printf("<union_> : {}\n");
	}
}

void parse_simple_re(){
	if(lookAhead != ')' && lookAhead != '|' && lookAhead != '\0'){ // TODO: compute FIRST set of <concatenation>
		printf("\t<simple_re> : <basic_re> <concatenation>\n");
		parse_basic_re();
		parse_concatenation();
	} else {
		fprintf(stderr, "invalid simple_re\n");
	}
}

void parse_concatenation(){
	if(lookAhead != ')' && lookAhead != '|' && lookAhead != '\0'){ // TODO: compute FIRST set of <basic_re>
		printf("\t<concatenation> : <basic_re> <concatenation>\n");
		parse_basic_re();
		parse_concatenation();
	} else {
		printf("\t<concatenation> : {}\n");
	}
}

void parse_basic_re(){
	printf("\t<basic_re> : <elementary_re> <basic_re_prime>\n");
	parse_elementary_re();
	parse_basic_re_prime();
}

void parse_basic_re_prime(){
	if(lookAhead == '*'){
		printf("\t<basic_re_prime> : '*' <basic_re_prime>\n");
		nextToken();
		parse_basic_re_prime();
	} else if(lookAhead == '+'){
		printf("\t<basic_re_prime> : '+' <basic_re_prime>\n");
		nextToken();
		parse_basic_re_prime();
	} else if(lookAhead == '?'){
		printf("\t<basic_re_prime> : '?' <basic_re_prime>\n");
		nextToken();
		parse_basic_re_prime();
	} else {
		// sigma production
		printf("\t<basic_re_prime> : {}\n");
		;
	}
}

void parse_elementary_re(){
	if(lookAhead == '('){
		printf("\t<elementary_re> : <group>\n");
		parse_group();
	} else if(lookAhead == '.'){
		printf("\t<elementary_re> : <any>\n");
		parse_any();
	} else if(lookAhead == '$'){
		printf("\t<elementary_re> : <eos>\n");
		parse_eos();
	} else if(!isMetaChar(lookAhead) || lookAhead == '\\'){
		printf("\t<elementary_re> : <character>\n");
		parse_character();
	} else if(lookAhead == '['){
		printf("\t<elementary_re> : <set>\n");
		parse_set();
	} else {
		fprintf(stderr, "invalid elementary_re\n");
		exit(1);
	}
}

void parse_group(){
	if(lookAhead == '('){
		printf("\t<group> : '(' <basic_re> ')'\n");
		nextToken();
		parse_re();
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

void parse_any(){
	if(lookAhead == '.'){
		printf("\t<any> : '.'\n");
		nextToken();
	} else {
		fprintf(stderr, "invalid any\n");
		exit(1);
	}
}

void parse_eos(){
	if(lookAhead == '$'){
		printf("\t<eof> : '$'\n");
		nextToken();
	} else {
		fprintf(stderr, "invalid eos\n");
		exit(1);
	}
}

void parse_character(){
	if(!isMetaChar(lookAhead)){
		printf("\t<character> : <nonmetachar>\n");
		nextToken();
	} else if(lookAhead == '\\'){
		printf("\t<character> : '\\' <metachar>\n");
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

void parse_set(){
	printf("<set> : ???\n");
	fprintf(stderr, "not implemented: set\n");
	exit(1);
}

int main(int argc, char *argv[]){
	nextToken();
	parse_re();
	printf("%d\n", inputIndex);
}
