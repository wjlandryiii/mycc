all: regex

regex.o: regex.c parser.h nfa.h
	gcc -g -c -o $@ $<

parser.o: parser.c parser.h
	gcc -g -c -o $@ $<

nfa.o: nfa.c nfa.h
	gcc -g -c -o $@ $<

regex: regex.o parser.o nfa.o
	gcc -g -o $@ $^
