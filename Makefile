all: lexer parser grammar/grammar shuntingyard

lexer: lexer.c
	gcc -g -o lexer lexer.c

parser: parser.c
	gcc -g -o parser parser.c

grammar/grammar: grammar/grammar.c
	gcc -g -o $@ $<

shuntingyard: shuntingyard.c
	gcc -g -o $@ $<
