all: regex

list.o: list.c list.h
	gcc -g -c -o $@ $<

set.o: set.c set.h
	gcc -g -c -o $@ $<

ast.o: ast.c ast.h list.h set.h
	gcc -g -c -o $@ $<

dfa.o: dfa.c dfa.h set.h list.h
	gcc -g -c -o $@ $<

regex.o: regex.c set.h dfa.h list.h ast.h
	gcc -g -c -o $@ $<

regex: regex.o ast.o dfa.o set.o list.o
	gcc -g -o $@ $^

.PHONY: clean

clean:
	rm -f *.o
	rm -f regex
