all: regex

set.o: set.c set.h
	gcc -g -c -o $@ $<

dfa.o: dfa.c dfa.h set.h
	gcc -g -c -o $@ $<

regex.o: regex.c set.h dfa.h
	gcc -g -c -o $@ $<

regex: regex.o set.o dfa.o
	gcc -g -o $@ $^

.PHONY: clean

clean:
	rm -f *.o
	rm -f regex
