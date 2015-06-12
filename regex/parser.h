/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef PARSER_H
#define PARSER_H

struct re;
struct union_;
struct simple_re;
struct concatenation;
struct basic_re;
struct basic_re_prime;
struct elementary_re;
struct group;
struct any;
struct eos;
struct character;
struct set;
struct set_prime;
struct set_items;
struct nonmetachar;
struct metachar;



struct re {
	int rule;
	struct simple_re *simple_re;
	struct union_ *union_;
};

struct union_ {
	int rule;
	struct simple_re *simple_re;
	struct union_ *union_;
};

struct simple_re {
	int rule;
	struct basic_re *basic_re;
	struct concatenation *concatenation;
};

struct concatenation {
	int rule;
	struct basic_re *basic_re;
	struct concatenation *concatenation;
};

struct basic_re {
	int rule;
	struct elementary_re *elementary_re;
	struct basic_re_prime *basic_re_prime;
};

struct basic_re_prime {
	int rule;
	struct basic_re_prime *basic_re_prime;
};

struct elementary_re {
	int rule;
	struct group *group;
	struct any *any;
	struct eos *eos;
	struct character *character;
	struct set *set;
};

struct group {
	int rule;
	struct re *re;
};

struct any {
	int rule;
};

struct eos {
	int rule;
};

struct character {
	int rule;
	struct nonmetachar *nonmetachar;
	struct metachar *metachar;
	int token;
};

struct set {
	int rule;
	struct set_prime *set_prime;
};

struct set_prime {
	int rule;
	struct set_items *set_items;
};

struct set_items {
	int rule;
};

struct nonmetachar {
	int rule;
	char token;
};

struct metachar {
	int rule;
	char token;
};


#define MAX_REGEXSTRING (1024)
extern char REGEXSTRING[MAX_REGEXSTRING];
extern struct re *PARSETREE;

int parse();

#endif
