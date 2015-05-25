all: lexer parser grammar

lexer: lexer.c
	gcc -g -o lexer lexer.c

parser: parser.c
	gcc -g -o parser parser.c

grammar: grammar.c
	gcc -g -o $@ $<
