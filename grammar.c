/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

/*
 * This is to work over the grammar to make it LL(1), 
 * or something a human can make LL(1).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct position {
	char *fileName;
	int lineNumber;
	int charNumber;
};


enum TOK_TYPES {
	TOK_INVALID = 0,
	TOK_NONTERMINAL,
	TOK_TERMINAL,
	TOK_PRODUCE,
	TOK_OPENBRACKET,
	TOK_CLOSEBRACKET,
	TOK_PIPE,
	TOK_SEMICOLON,
	TOK_EOF,
};

char *token_type_strings[] = {
	"INVALID",
	"NON-TERMINAL",
	"TERMINAL",
	"PRODUCE (\"::=\")",
	"OPENBRACKET ('[')",
	"CLOSEBRACKET (']')",
	"PIPE ('|')",
	"SEMICOLON (';')",
	"EOF",
};

struct token {
	int type;
	char *start;
	char *end;
	struct position pos;
};

int next_token(struct position *pos, char *s, struct token *token){
	for(;;){
		while(*s == ' ' || *s == '\t' || *s == '\n'){
			if(pos){
				if(*s == '\n'){
					pos->lineNumber += 1;
					pos->charNumber = 0;
				} else {
					pos->charNumber++;
				}
			}
			s++;
		}
		if(*s == '#'){
			while(*s != '\n'){
				s++;
				if(pos){
					pos->charNumber++;
				}
			}
			s++;
			if(pos){
				pos->lineNumber += 1;
				pos->charNumber = 0;
			}
		} else {
			break;
		}
	}

	token->start = s;
	if(pos){
		token->pos = *pos;
	}

	if(*s == '<'){
		// Non-terminal
		s++;
		while(isalnum(*s) || *s == '-'){
			s++;
		}
		if(*s == '>'){
			s++;
			token->end = s;
			token->type = TOK_NONTERMINAL;
			if(pos){
				pos->charNumber += token->end - token->start;
			}
			return 0;
		} else {
			if(pos){
				fprintf(stderr, "Line: %d:%d\n", pos->lineNumber, pos->charNumber);
			}
			fprintf(stderr, "missing '>'\n");
			exit(1);
		}
	} else if(*s == '"'){
		// Terminal
		s++;
		while(*s != '"' && *s != '\n'){
			s++;
		}
		if(*s == '"'){
			s++;
			token->end = s;
			token->type = TOK_TERMINAL;
			if(pos){
				pos->charNumber += token->end - token->start;
			}
			return 0;
		} else {
			if(pos){
				fprintf(stderr, "Line: %d:%d\n", pos->lineNumber, pos->charNumber);
			}
			fprintf(stderr, "missing '\"'\n");
			exit(1);
		}
	} else if(*s == '\''){
		// Terminal
		s++;
		while(*s != '\'' && *s != '\n'){
			s++;
		}
		if(*s == '\''){
			s++;
			token->end = s;
			token->type = TOK_TERMINAL;
			if(pos){
				pos->charNumber += token->end - token->start;
			}
			return 0;
		} else {
			if(pos){
				fprintf(stderr, "Line: %d:%d\n", pos->lineNumber, pos->charNumber);
			}
			fprintf(stderr, "missing '\\''\n");
			exit(1);
		}
	} else if(*s == ':'){
		s++;
		if(*s == ':'){
			s++;
			if(*s == '='){
				s++;
				token->end = s;
				token->type = TOK_PRODUCE;
				return 0;
				if(pos){
					pos->charNumber += token->end - token->start;
				}
			} else {
				if(pos){
					fprintf(stderr, "Line: %d:%d\n", pos->lineNumber, pos->charNumber);
				}
				fprintf(stderr, "expected ::=\n");
				exit(1);
			}
		} else {
			if(pos){
				fprintf(stderr, "Line: %d:%d\n", pos->lineNumber, pos->charNumber);
			}
			fprintf(stderr, "expected ::=\n");
			exit(1);
		}
	} else if(*s == '|'){
		s++;
		token->end = s;
		token->type = TOK_PIPE;
		if(pos){
			pos->charNumber += token->end - token->start;
		}
		return 0;
	} else if(*s == '['){
		s++;
		token->end = s;
		token->type = TOK_OPENBRACKET;
		if(pos){
			pos->charNumber += token->end - token->start;
		}
		return 0;
	} else if(*s == ']'){
		s++;
		token->end = s;
		token->type = TOK_CLOSEBRACKET;
		if(pos){
			pos->charNumber += token->end - token->start;
		}
		return 0;
	} else if(*s == ';'){
		s++;
		token->end = s;
		token->type = TOK_SEMICOLON;
		if(pos){
			pos->charNumber += token->end - token->start;
		}
		return 0;
	} else if(*s == '\0'){
		//s++;
		token->end = s;
		token->type = TOK_EOF;
		if(pos){
			pos->charNumber += token->end - token->start;
		}
		return 0;
	} else {
		return -1;
	}
	fprintf(stderr, "HOW DID I GET HERE?\n");
	if(pos){
		fprintf(stderr, "Line: %d:%d\n", pos->lineNumber, pos->charNumber);
	}
	fprintf(stderr, "input: _%s_\n", s);
	return -1;
}

void print_token(struct token *token){
	char *s;

	printf("%-20s: (%3d,%2d) _", token_type_strings[token->type], token->pos.lineNumber, token->pos.charNumber);
	for(s = token->start; s != token->end; s++){
		if(*s == '\t'){
			putchar('\\');
			putchar('t');
		} else if(*s == '\n'){
			putchar('\\');
			putchar('n');
		} else {
			putchar(*s);
		}
	}
	printf("_\n");
}

int test_tokenizer(char *s, char *t){
	int len;
	struct token token;

	if(next_token(NULL, s, &token)){
		printf("Failed to tokenize: _%s_\n", s);
		return -1;
	}
	len = token.end - token.start;
	if(len != strlen(t) || strncmp(token.start, t, len)){
		printf("Token is unexpected: ");
		putchar('_');
		fwrite(token.start, 1, len, stdout);
		putchar('_');
		printf(" != _%s_\n", t);
		printf("%d != %lu\n", len, strlen(t));
	}
	return 0;

}

void test_tokens(void){
	// TOK_NONTERMINAL,
	test_tokenizer(" <hello-world> ", "<hello-world>");
	// TOK_TERMINAL,
	test_tokenizer(" \"hello world\" ", "\"hello world\"");
	test_tokenizer(" \'hello world\' ", "\'hello world\'");
	// TOK_PRODUCE,
	test_tokenizer(" ::= ", "::=");
	// TOK_OPENBRACKET,
	test_tokenizer(" [ ", "[");
	// TOK_CLOSEBRACKET,
	test_tokenizer(" ] ", "]");
	// TOK_PIPE,
	test_tokenizer(" | ", "|");
	// TOK_SEMICOLON,
	test_tokenizer(" ; ", ";");
	// TOK_EOF,
	test_tokenizer("  ", "");

	// comments
	test_tokenizer("# this is a test comment <asdf> [ ] ::= ; \"moooo\"\n <hello-world> ::=", "<hello-world>");
}

char inputFileContents[16*1024];
int inputFileSize = 0;
struct token tokens[4*1024]; // ~ 753
int tokenCount = 0;
int parseTokenIndex = 0;


int tokenize_file(char *filename){
	FILE *f;
	int c;

	f = fopen(filename, "r");
	while((c = getc(f)) != EOF){
		inputFileContents[inputFileSize++] = c;
		if(inputFileSize >= sizeof(inputFileContents)){
			fprintf(stderr, "input file too big\n");
			exit(1);
		}
	}
	inputFileContents[inputFileSize++] = '\0';

	char *s = inputFileContents;

	struct position pos;

	pos.fileName = filename;
	pos.lineNumber = 1;
	pos.charNumber = 0;

	while(tokenCount < 1024){
		if(next_token(&pos, s, &tokens[tokenCount])){
			fprintf(stderr, "ending tokenizing too soon\n");
			fprintf(stderr, "input: %.20s\n", s);
			exit(1);
		}
		s = tokens[tokenCount].end;
		if(tokens[tokenCount].type == TOK_EOF)
			break;
		tokenCount++;
	}
	
	return 0;
}

void print_tokens(){
	int i;
	for(i = 0; i < tokenCount; i++){
		print_token(&tokens[i]);
	}
	printf("Token count: %d\n", tokenCount);
}

char stringTable[4*1024];
char *stringTableTail = stringTable;

char *addString(char *s){
	char *p;
	p = stringTableTail;
	strcpy(p, s);
	stringTableTail += strlen(s)+1;
	return p;
}

char *nonTerminalList[128];
int nonTerminalCount = 0;
void addNonTerminal(char *s){
	char *p = addString(s);
	nonTerminalList[nonTerminalCount++] = p;
}

void printAllNonTerminals(){
	int i;
	for(i = 0; i < nonTerminalCount; i++){
		printf("%s\n", nonTerminalList[i]);
	}
	printf("Count: %d\n", nonTerminalCount);
}


struct non_terminal {
	struct token *token;
};

struct non_terminal *non_terminal(){
	struct non_terminal *nt;
	if(tokens[parseTokenIndex].type == TOK_NONTERMINAL){
		nt = malloc(sizeof(struct non_terminal));
		nt->token = &tokens[parseTokenIndex];
		parseTokenIndex++;
		return nt;
	} else {
		fprintf(stderr, "Expected non-terminal\n");
		fprintf(stderr, "line: %d:%d\n", tokens[parseTokenIndex].pos.lineNumber, tokens[parseTokenIndex].pos.charNumber);
		return NULL;
	}
}

struct terminal {
	struct token *token;
};

struct terminal *terminal(){
	struct terminal *t;
	if(tokens[parseTokenIndex].type == TOK_TERMINAL){
		t = malloc(sizeof(struct terminal));
		t->token = &tokens[parseTokenIndex];
		parseTokenIndex++;
		return t;
	} else {
		fprintf(stderr, "Expected terminal\n");
		fprintf(stderr, "line: %d:%d\n", tokens[parseTokenIndex].pos.lineNumber, tokens[parseTokenIndex].pos.charNumber);
		return NULL;
	}
}


void parse(){
	parseTokenIndex = 0;

	while(tokens[parseTokenIndex].type == TOK_NONTERMINAL){
		tokens[parseTokenIndex].start++;
		tokens[parseTokenIndex].end--;
		*tokens[parseTokenIndex].end = '\0';
		addNonTerminal(tokens[parseTokenIndex].start);
		parseTokenIndex++;

		if(tokens[parseTokenIndex].type == TOK_PRODUCE){
			do {
				parseTokenIndex++;
				while(tokens[parseTokenIndex].type == TOK_NONTERMINAL
						|| tokens[parseTokenIndex].type == TOK_TERMINAL
						|| tokens[parseTokenIndex].type == TOK_OPENBRACKET
						|| tokens[parseTokenIndex].type == TOK_CLOSEBRACKET){
					parseTokenIndex++;
				}
			} while(tokens[parseTokenIndex].type == TOK_PIPE);
			if(tokens[parseTokenIndex].type == TOK_SEMICOLON){
				parseTokenIndex++;
			} else {
				fprintf(stderr, "Expected \";\"\n");
				fprintf(stderr, "Got: %s\n", token_type_strings[tokens[parseTokenIndex].type]);
				fprintf(stderr, "line: %d:%d\n", tokens[parseTokenIndex].pos.lineNumber, tokens[parseTokenIndex].pos.charNumber);
				return;
			}
		} else {
			fprintf(stderr, "Expected \"::=\"\n");
			fprintf(stderr, "Got: %s\n", token_type_strings[tokens[parseTokenIndex].type]);
			fprintf(stderr, "line: %d:%d\n", tokens[parseTokenIndex].pos.lineNumber, tokens[parseTokenIndex].pos.charNumber);
			return;
		}
	}
	if(tokens[parseTokenIndex].type == TOK_EOF){
		return;
	} else {
		fprintf(stderr, "Expected EOF\n");
		fprintf(stderr, "Got: %s\n", token_type_strings[tokens[parseTokenIndex].type]);
		fprintf(stderr, "line: %d:%d\n", tokens[parseTokenIndex].pos.lineNumber, tokens[parseTokenIndex].pos.charNumber);
		return;
	}
}


int main(int argc, char *argv[]){
	//test_tokens();
	tokenize_file("grammar.txt");
	//print_tokens();
	parse();
	printAllNonTerminals();
	
	return 0;
}
