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

char stringTable[4*1024];
int stringTableBytesUsed = 0;
char *strings[1024];
int stringCount;

int lookupString(char *s, int len){
	int i;

	for(i = 0; i < stringCount; i++){
		if(strlen(strings[i]) == len){
			if(strncmp(strings[i], s, len) == 0){
				return i;
			}
		}
	}
	return -1;
}

char *addString(char *s, int len){
	int i;

	i = lookupString(s, len);
	if(i >= 0){
		return strings[i];
	} else {
		if(stringTableBytesUsed + len + 1 < sizeof(stringTable)){
			if(stringCount + 1 < sizeof(strings) / sizeof(strings[0])){
				strings[stringCount++] = &stringTable[stringTableBytesUsed];
				memcpy(&stringTable[stringTableBytesUsed], s, len);
				stringTableBytesUsed += len;
				stringTable[stringTableBytesUsed] = '\0';
				stringTableBytesUsed++;
				return strings[stringCount-1];
			} else {
				fprintf(stderr, "strings is full\n");
				exit(1);
			}
		} else {
			fprintf(stderr, "stringTable is full\n");
			exit(1);
		}
	}
}

struct symbol {
	char *name;
};

struct symbol symbols[128];
int symbolCount = 0;

int lookupSymbol(char *s){
	int i;
	for(i = 0; i < symbolCount; i++){
		if(strcmp(symbols[i].name, s) == 0){
			return i;
		}
	}
	return -1;
}

int addSymbol(char *s){
	int i;

	i = lookupSymbol(s);
	if(i < 0){
		struct symbol symbol;
		symbol.name = s;
		i = symbolCount;
		symbols[symbolCount++] = symbol;
	}
	return i;
}


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
	TOK_PIPE,
	TOK_SEMICOLON,
	TOK_EOF,
};

char *token_type_strings[] = {
	"INVALID",
	"NON-TERMINAL",
	"TERMINAL",
	"PRODUCE (':')",
	"PIPE ('|')",
	"SEMICOLON (';')",
	"EOF",
};

struct token {
	int type;
	int symbol;
	char *str;
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
			token->str = addString(token->start, token->end - token->start);
			token->symbol = addSymbol(token->str);
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
			token->str = addString(token->start, token->end - token->start);
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
			token->str = addString(token->start, token->end - token->start);
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
		token->end = s;
		token->type = TOK_PRODUCE;
		token->str = ":";
		if(pos){
			pos->charNumber += token->end - token->start;
		}
		return 0;
	} else if(*s == '|'){
		s++;
		token->end = s;
		token->type = TOK_PIPE;
		token->str = "|";
		if(pos){
			pos->charNumber += token->end - token->start;
		}
		return 0;
	} else if(*s == ';'){
		s++;
		token->end = s;
		token->type = TOK_SEMICOLON;
		token->str = ";";
		if(pos){
			pos->charNumber += token->end - token->start;
		}
		return 0;
	} else if(*s == '\0'){
		//s++;
		token->end = s;
		token->type = TOK_EOF;
		token->str = "";
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
	test_tokenizer(" : ", ":");
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



struct production {
	char *s;
	int type;
	int symbol;
	struct production *next;
};

char *term(){
	int type = tokens[parseTokenIndex].type;
	char *s = tokens[parseTokenIndex].str;

	if(type == TOK_NONTERMINAL || type == TOK_TERMINAL){
		parseTokenIndex++;
		return s;
	} else {
		fprintf(stderr, "term(): expected nonterminal or terminal\n");
		exit(1);
	}
}

struct production *term_list(){
	int type = tokens[parseTokenIndex].type;

	struct production *p = malloc(sizeof(*p));

	if(type == TOK_NONTERMINAL || type == TOK_TERMINAL){
		p->s = term();
		p->type = type;
		p->symbol = tokens[parseTokenIndex].symbol;
		type = tokens[parseTokenIndex].type;
		if(type == TOK_NONTERMINAL || type == TOK_TERMINAL){
			p->next = term_list();
		} else {
			p->next = NULL;
		}
		return p;
	} else {
		fprintf(stderr, "term_list() expected terminal or nonterminal\n");
		fprintf(stderr, "line: %d:%d\n", tokens[parseTokenIndex].pos.lineNumber, tokens[parseTokenIndex].pos.charNumber);
		exit(1);
	}
}


struct productions {
	struct production *production;
	struct productions *next;
};


struct productions *productions(){
	int type = tokens[parseTokenIndex].type;
	struct productions *p = malloc(sizeof(struct productions));

	if(type == TOK_TERMINAL || type == TOK_NONTERMINAL){

		p->production = term_list();

		if(tokens[parseTokenIndex].type == TOK_PIPE){
			parseTokenIndex++;
			p->next = productions();
			return p;
		} else if(tokens[parseTokenIndex].type == TOK_SEMICOLON){
			parseTokenIndex++;
			p->next = NULL;
			return p;
		} else {
			fprintf(stderr, "how did i get here??\n");
			return NULL;
		}
	} else {
		fprintf(stderr, "productions() expected terminal or nonterminal\n");
		fprintf(stderr, "line: %d:%d\n", tokens[parseTokenIndex].pos.lineNumber, tokens[parseTokenIndex].pos.charNumber);
		exit(1);
	}
}



struct rule {
	char *rule_name;
	struct node *tail;
	struct productions *productions;
};

struct rule rules[128];
int ruleCount = 0;

void parse(){
	parseTokenIndex = 0;

	struct rule *rule;
	while(tokens[parseTokenIndex].type == TOK_NONTERMINAL){
		rule = &rules[ruleCount++];
		rule->rule_name = tokens[parseTokenIndex].str;
		parseTokenIndex++;

		if(tokens[parseTokenIndex].type == TOK_PRODUCE){
			parseTokenIndex++;
			rule->productions = productions();
		} else {
			fprintf(stderr, "Expected \":\"\n");
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

void print_rules(){
	int i;
	for(i = 0; i < ruleCount; i++){
		printf("%s\n", rules[i].rule_name);
		struct productions *ps = rules[i].productions;
		if(ps != NULL){
			printf("\t: ");
			struct production *p = ps->production;
			while(p != NULL){
				printf("%s", p->s);
				p = p->next;
				if(p != NULL){
					printf(" ");
				} else {
					printf("\n");
				}
			}
			ps = ps->next;
			while(ps != NULL){
				printf("\t| ");
				struct production *p = ps->production;
				while(p != NULL){
					printf("%s", p->s);
					p = p->next;
					if(p != NULL){
						printf(" ");
					} else {
						printf("\n");
					}
				}
				ps = ps->next;
			}
		}
		printf("\t;\n\n");
	}
}

void find_orphan_symbols(){
	int i, j;

	for(i = 0; i < symbolCount; i++){
		for(j = 0; j < ruleCount; j++){
			if(strcmp(symbols[i].name, rules[j].rule_name) == 0){
				break;
			}
		}
		if(j == ruleCount){
			printf("ORPHAN SYMBOL: %s\n", symbols[i].name);
		}
	}
}

void print_all_terminals(){
	int i, j;
	char *terminals[128];
	int terminalCount = 0;

	for(i = 0; i < ruleCount; i++){
		struct productions *ps = rules[i].productions;

		while(ps != NULL){
			struct production *p;

			p = ps->production;
			while(p != NULL){
				if(p->type == TOK_TERMINAL){
					for(j = 0; j < terminalCount; j++){
						if(terminals[j] == p->s){
							break;
						}
					}
					if(j == terminalCount){
						terminals[terminalCount++] = p->s;
					}
				}
				p = p->next;
			}
			ps = ps->next;
		}
	}

	for(i = 0; i < terminalCount; i++){
		printf("TERMINAL: %s\n", terminals[i]);
	}
}


void find_direct_left_recursion(){
	int i, j;

	for(i = 0; i < ruleCount; i++){
		char *ruleName = rules[i].rule_name;
		struct productions *ps = rules[i].productions;

		while(ps != NULL){
			struct production *p = ps->production;

			if(p != NULL && p->type == TOK_NONTERMINAL && strcmp(p->s, ruleName)){
				printf("DIRECT LEFT RECURSION: %s\n", ruleName);
				break;
			}
			ps = ps->next;
		}
	}
}


int main(int argc, char *argv[]){
	//test_tokens();
	tokenize_file("grammar.txt");
	//print_tokens();
	parse();

	//print_rules();
	//find_orphan_symbols();
	print_all_terminals();
	//find_direct_left_recursion();
	return 0;
}
