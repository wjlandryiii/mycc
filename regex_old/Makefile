all: regex

regex.o: regex.c parser.h nfa.h
	gcc -g -c -o $@ $<

parser.o: parser.c parser.h
	gcc -g -c -o $@ $<

nfa.o: nfa.c nfa.h
	gcc -g -c -o $@ $<

ast.o: ast.c ast.h
	gcc -g -c -o $@ $<

regex: regex.o parser.o nfa.o ast.o
	gcc -g -o $@ $^

.PHONY: clean

clean:
	rm -f *.o
	rm regex
