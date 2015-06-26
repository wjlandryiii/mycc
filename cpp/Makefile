all: cpp test_lexer

escstr.o: escstr.c escstr.h
	gcc -g -c -o $@ $<

lexer.o: lexer.c lexer.h
	gcc -g -c -o $@ $<

cpp.o: cpp.c lexer.h
	gcc -g -c -o $@ $<



cpp: cpp.o lexer.o escstr.o
	gcc -g -o $@ $^


test_lexer.o: lexer.c escstr.h
	gcc -DTESTING -g -c -o $@ $<

test_lexer: test_lexer.o escstr.o
	gcc -DTESTING -g -o $@ $^
