all: regex

regex.o: regex.c parser.h
	gcc -g -c -o $@ $<

parser.o: parser.c parser.h
	gcc -g -c -o $@ $<

regex: regex.o parser.o
	gcc -g -o $@ $^
