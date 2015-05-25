/*
 * Copyright 2015 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum TOK_CLASSES {
	TOK_CLASS_INVALID,
	TOK_CLASS_CONSTANT,
	TOK_CLASS_OPERATOR,
	TOK_CLASS_EOF,
};

enum TOK_OPERATORS {
	TOK_OP_INVALID,
	TOK_OP_ADDITION,
	TOK_OP_SUBTRACTION,
	TOK_OP_MULTIPLICATION,
	TOK_OP_DIVISION,
};

struct token {
	int class;
	int operation;
	int type;
	int constant;
	char *start;
	char *end;
};

char inputString[128];
struct token tokens[128];
int tokenCount = 0;
int parseTokenIndex = 0;

int nextToken(char *s, struct token *token){
	while(*s == ' ' || *s == '\t'){
		s++;
	}
	token->start = s;
	if(isdigit(*s)){
		token->constant = *s - '0';
		s++;
		while(isdigit(*s)){
			token->constant = token->constant * 10 + *s - '0';
			s++;
		}
		token->end = s;
		token->class = TOK_CLASS_CONSTANT;
		return 0;
	} else if(*s == '+'){
		s++;
		token->end = s;
		token->class = TOK_CLASS_OPERATOR;
		token->operation = TOK_OP_ADDITION;
		return 0;
	} else if(*s == '-'){
		s++;
		token->end = s;
		token->class = TOK_CLASS_OPERATOR;
		token->operation = TOK_OP_SUBTRACTION;
		return 0;
	} else if(*s == '*'){
		s++;
		token->end = s;
		token->class = TOK_CLASS_OPERATOR;
		token->operation = TOK_OP_MULTIPLICATION;
		return 0;
	} else if(*s == '/'){
		s++;
		token->end = s;
		token->class = TOK_CLASS_OPERATOR;
		token->operation = TOK_OP_DIVISION;
		return 0;
	} else if(*s == '\0'){
		token->end = s;
		token->class = TOK_CLASS_EOF;
		return 0;
	} else {
		return -1;
	}
}

int tokenize(){
	char *p = inputString;

	while(1){
		if(nextToken(p, &tokens[tokenCount])){
			fprintf(stderr, "invalid token: %.10s...\n", p);
			exit(1);
		}
		if(tokens[tokenCount].class == TOK_CLASS_EOF){
			break;
		} else {
			p = tokens[tokenCount].end;
			tokenCount++;
		}
	}
	return 0;
}

void printToken(struct token *token){
	char *p;

	printf("%3d: _", token->class);
	for(p = token->start; p < token->end; p++){
		putchar(*p);
	}
	printf("_\n");
}

void printTokens(void){
	int i;

	for(i = 0; i < tokenCount; i++){
		printToken(&tokens[i]);
	}
}

int precidence(int op){
	switch(op){
		case TOK_OP_ADDITION:
		case TOK_OP_SUBTRACTION:
			return 50;
		case TOK_OP_MULTIPLICATION:
		case TOK_OP_DIVISION:
			return 60;
		default:
			return 0;
	}
}


struct token *queue[128];
int queueHead = 0;
int queueTail = 0;
struct token *stack[128];
int stackHead = 0;

void shuntingYard(void){
	int i;

	for(i = 0; i < tokenCount; i++){
		if(tokens[parseTokenIndex].class == TOK_CLASS_CONSTANT){
			queue[queueTail++] = &tokens[parseTokenIndex];
			parseTokenIndex++;
		} else if(tokens[parseTokenIndex].class == TOK_CLASS_OPERATOR){
			while(stackHead > 0 && precidence(stack[stackHead - 1]->operation) > precidence(tokens[parseTokenIndex].operation)){
				queue[queueTail++] = stack[--stackHead];
			}
			stack[stackHead++] = &tokens[parseTokenIndex];
			parseTokenIndex++;
		}
	}
	while(stackHead > 0){
		queue[queueTail++] = stack[--stackHead];
	}
}

void printQueue(void){
	int i;

	for(i = 0; i < queueTail; i++){
		printToken(queue[i]);
	}
}


int solveStack[128];
int solveStackHead = 0;

void solve(void){
	int op;
	int a, b, c;
	int i;

	for(i = 0; i < queueTail; i++){
		if(queue[i]->class == TOK_CLASS_CONSTANT){
			solveStack[solveStackHead++] = queue[i]->constant;
		} else if(queue[i]->class == TOK_CLASS_OPERATOR){
			op = queue[i]->operation;
			if(op == TOK_OP_ADDITION){
				a = solveStack[--solveStackHead];
				b = solveStack[--solveStackHead];
				c = b + a;
				solveStack[solveStackHead++] = c;
			} else if(op == TOK_OP_SUBTRACTION){
				a = solveStack[--solveStackHead];
				b = solveStack[--solveStackHead];
				c = b - a;
				solveStack[solveStackHead++] = c;
			} else if(op == TOK_OP_MULTIPLICATION){
				a = solveStack[--solveStackHead];
				b = solveStack[--solveStackHead];
				c = b * a;
				solveStack[solveStackHead++] = c;
			} else if(op == TOK_OP_DIVISION){
				a = solveStack[--solveStackHead];
				b = solveStack[--solveStackHead];
				c = b / a;
				solveStack[solveStackHead++] = c;
			} else {
				fprintf(stderr, "invalid operation: %d\n", op);
				exit(1);
			}
		}
	}
	printf("stackHead: %d\n", solveStackHead);
	printf("Solution: %d\n", solveStack[0]);
}


int main(int argc, char *argv[]){
	strcpy(inputString, "5 + 10 * 2 - 44");
	tokenize();
	//printTokens();
	shuntingYard();
	//printQueue();
	solve();
}
