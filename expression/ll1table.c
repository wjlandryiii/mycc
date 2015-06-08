/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

/*
 * ABANDONED!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRINGDATA (4*1024)
char STRINGDATA[MAX_STRINGDATA];
int nSTRINGDATA;

#define MAX_STRINGS (1024)
char *STRING[MAX_STRINGS];
int nSTRINGS;

enum {
	SYMBOLTYPE_INVALID = 0,
	SYMBOLTYPE_NONTERMINAL,
	SYMBOLTYPE_TERMINAL,
};
#define MAX_SYMBOLS (1024)
int SYMBOL[MAX_SYMBOLS];
int SYMBOLTYPE[MAX_SYMBOLS];
int nSYMBOLS;
int TERMINALINDEX[MAX_SYMBOLS];
int NONTERMINALINDEX[MAX_SYMBOLS];

#define MAX_RULES (1024)
#define MAX_RULE_SIZE (16)
int RULE[MAX_RULES][MAX_RULE_SIZE];
int RULENAME[MAX_RULES];
int RULESIZE[MAX_RULES];
int nRULES;

int PARSETABLE[MAX_SYMBOLS][MAX_SYMBOLS];

#if 0
int nextToken(FILE *f){
	int c;
	int i;
	char lexeme[128];

	while((c = getc(f)) == ' ' || c == '\t'){
		;
	}

	if(c == '\n'){
		strcpy(lexeme, "\n");
		return TOKENNAME_EOF;
	} else if(c == ':') {
		strcpy(lexeme, ":");
		return TOKENNAME_COLON;
	} else if(c == '{'){
		c = getc(f);
		if(c == '}'){
			return TOKENNAME_EPSILON;
		} else {
			fprintf(stderr, "Token {} error; '{' must be followed by '}'.\n");
			exit(1);
		}
	} else if(c == '\''){
		i = 0;
		lexeme[i++] = '\'';

		c = getc(f);
		for(;;){
			if(c != '\n' && c != ' ' && c != '\t'){
				lexeme[i++];
				if(c == '\''){
					lexeme[i] = '\0';
					return TOKENNAME_TERMINAL;
				} else if(i >= sizeof(lexeme) - 1){
					fprintf(stderr, "Token too large\n");
					exit(1);
				} else {
					c = getc(f);
				}
			} else {
				fprintf(stderr, "Terminal can't contain white space\n");
				exit(1);
			}
		}
	} else if(c == '\"'){
		i = 0;
		lexeme[i++] = '\"';

		c = getc(f);
		for(;;){
			if(c != '\n' && c != ' ' && c != '\t'){
				lexeme[i++];
				if(c == '\"'){
					lexeme[i] = '\0';
					return TOKENNAME_TERMINAL;
				} else if(i >= sizeof(lexeme) - 1){
					fprintf(stderr, "Token too large\n");
					exit(1);
				} else {
					c = getc(f);
				}
			} else {
				fprintf(stderr, "Terminal can't contain white space\n");
				exit(1);
			}
		}
	} else if(c == '<'){
		i = 0;
		lexeme[i++] = '<';

		c = getc(f);
		for(;;){
			if(c != '\n' && c != ' ' && c != '\t'){
				lexeme[i++];
				if(c == '>'){
					lexeme[i] = '\0';
					return TOKENNAME_NONTERMINAL;
				} else if(i >= sizeof(lexeme) - 1){
					fprintf(stderr, "Token too large\n");
					exit(1);
				} else {
					c = getc(f);
				}
			} else {
				fprintf(stderr, "Terminal can't contain white space\n");
				exit(1);
			}
		}
	} else if('A' <= c && c <= 'Z'){
		
	}

}
#endif


int loadParser(){
	char command[128];
	FILE *f;
       
	f = fopen("ll1.txt", "r");
	if(f == NULL){
		return -1;
	}

	while(fscanf(f, "%s", command) == 1){
		printf("COMMAND: %s\n", command);

		if(strcmp(command, "TERMINAL") == 0){
			char buf[128];
			if(fscanf(f, "%s", buf) == 1){
				if(buf[0] == '\'' && buf[strlen(buf)-1] == '\''){
					buf[strlen(buf)-1] = '\0';
					buf[0] = '\0';
					printf("TERMINAL: '%s'\n", &buf[1]);
				} else {
					printf("TERMINAL: #%s#\n", buf);
				}
			} else {
				printf("DERP\n");
			}
		} else if(strcmp(command, "NONTERMINAL") == 0){

		} else if(strcmp(command, "STARTSYMBOL") == 0){

		} else if(strcmp(command, "EOFSYMBOL") == 0){

		} else if(strcmp(command, "RULE") == 0){

		} else if(strcmp(command, "PARSETABLE") == 0){

		} else if(strcmp(command, "INPUTSTRING") == 0){

		} else {
			printf("Unknown command: %s\n", command);
		}

		while(!feof(f) && getc(f) != '\n' ){
			;
		}
	};
	return 0;
}


#if 0
void parse(){
	int a;
	int X;
	int stack[1024];
	int stackIndex;
	int w[1024];
	int wIndex;

	stackIndex = -1;
	stack[++stackIndex] = -1;
	stack[++stackIndex] = 0;

	wIndex = 0;
	w[0] = 0;

	while(stack[stackIndex] != -1){
		if(stack[stackIndex] == w[wIndex]){
			stackIndex -= 1;
			wIndex += 1;
		} else if(isTerminal(stack[stackIndex])){
			fprintf(stderr, "error\n");
			exit(1);
		} else if(M[stack[stackIndex], w[wIndex]] < 0){
			fprintf(stderr, "error2\n");
			exit(1);
		} else if(M[stack[stackIndex], w[wIndex]] < nRULES){
			int rule = M[stack[stackIndex], w[wIndex]];
			emit(rule);
			stackIndex -= 1;
			for(i = RULESIZE[rule] - 1; i >= 0; i++){
				stack[stackIndex++] = RULE[rule][i];
			}

		} else {
			fprintf(stderr, "error3\n");
			exit(1);
		}
	}
}
#endif

int main(int argc, char *argv[]){
	loadParser();
	return 0;
}
