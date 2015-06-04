all: main grammar test_stringtable test_lexer test_symbols test_parser test_firstfollow

main: main.o firstfollow.o parser.o lexer.o stringtable.o symbols.o
	gcc -Wl,-stack_size -Wl,0x1000000 -g -o $@ $^

grammar: grammar.c
	gcc -g -o $@ $<

main.o: main.c stringtable.h symbols.h lexer.h parser.h firstfollow.h
	gcc -g -c -o $@ $<

test_firstfollow.o: test_firstfollow.c
	gcc -g -c -o $@ $<

firstfollow.o: firstfollow.c firstfollow.h symbols.h parser.h
	gcc -g -c -o $@ $<

parser.o: parser.c lexer.h symbols.h
	gcc -g -c -o $@ $<

symbols.o: symbols.c symbols.h
	gcc -g -c -o $@ $<

stringtable.o: stringtable.c stringtable.h
	gcc -g -c -o $@ $<

lexer.o: lexer.c lexer.h symbols.h stringtable.h
	gcc -g -c -o $@ $<

test_parser.o: test_parser.c lexer.h stringtable.h symbols.h parser.h
	gcc -g -c -o $@ $<

test_symbols.o: test_symbols.c
	gcc -g -c -o $@ $<

test_stringtable.o: test_stringtable.c stringtable.h
	gcc -g -c -o $@ $<

test_lexer.o: test_lexer.c lexer.h
	gcc -g -c -o $@ $<

test_firstfollow: test_firstfollow.o
	gcc -g -o $@ $^

test_parser: test_parser.o lexer.o stringtable.o symbols.o parser.o
	gcc -g -o $@ $^

test_symbols: test_symbols.o symbols.o
	gcc -g -o $@ $^

test_stringtable: stringtable.o test_stringtable.o
	gcc -g -o $@ $^

test_lexer: test_lexer.o lexer.o symbols.o stringtable.o
	gcc -g -o $@ $^

.PHONY: clean

clean:
	rm -rf *.dSYM
	rm -f *.o
	rm -f main
	rm -f grammar
	rm -f test_stringtable
	rm -f test_lexer
	rm -f test_symbols
