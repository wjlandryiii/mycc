all: cpp test_lexer test_phase1 test_phase2 test_phase3 test_stringbuf


phase1.o: phase1.c phase1.h
	gcc -g -c -o $@ $<

test_phase1.o: phase1.c phase1.h
	gcc -DTESTING -g -c -o $@ $<

phase2.o: phase2.c phase2.h phase1.h
	gcc -g -c -o $@ $<

test_phase2.o: phase2.c phase1.h phase1.h
	gcc -DTESTING -g -c -o $@ $<

phase3.o: phase3.c phase3.h phase2.h phase1.h stringbuf.h
	gcc -g -c -o $@ $<

test_phase3.o: phase3.c phase3.h phase2.h phase1.h stringbuf.h
	gcc -DTESTING -g -c -o $@ $<

stringbuf.o: stringbuf.c stringbuf.h
	gcc -g -c -o $@ $<

test_stringbuf.o: stringbuf.c stringbuf.h
	gcc -DTESTING -g -c -o $@ $<


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

test_phase1: test_phase1.o
	gcc -g -o $@ $^

test_phase2: test_phase2.o phase1.o
	gcc -g -o $@ $^

test_phase3: test_phase3.o phase2.o phase1.o stringbuf.o
	gcc -g -o $@ $^

test_stringbuf: test_stringbuf.o
	gcc -g -o $@ $^


.PHONY: clean

clean:
	rm -f *.o
	rm -f cpp
	rm -f test_*
	rm -rf *.dSYM