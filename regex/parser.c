/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

char REGEXSTRING[MAX_REGEXSTRING];
struct re *PARSETREE = NULL;

static int lookAhead;
static int inputIndex;

static int nextToken(){
	lookAhead = REGEXSTRING[inputIndex++];
	if(lookAhead < 0){
		fprintf(stderr, "only supports ASCII\n");
		exit(1);
	} else if(lookAhead != '\0'){
		printf("****  NEXT TOKEN: %c : %s $\n", lookAhead, &REGEXSTRING[inputIndex]);
	} else {
		printf("****  NEXT TOKEN: $\n");
	}

	return 0;
}

static int isMetaChar(int c){
	switch(c){
		case '\\':
		case '\"':
		case '.':
		case '^':
		case '$':
		case '[':
		case ']':
		case '*':
		case '+':
		case '?':
		case '{':
		case '}':
		case '|':
		case '/':
			return 1;
		default:
			return 0;
	}
}


static int parse_re(struct re **reOut);
static int parse_union(struct union_ **unionOut);
static int parse_simple_re(struct simple_re **simple_reOut);
static int parse_concatenation(struct concatenation **concatenation);
static int parse_basic_re(struct basic_re **basic_re);
static int parse_basic_re_prime(struct basic_re_prime **basic_re_prime);
static int parse_elementary_re(struct elementary_re **elementary_re);
static int parse_group(struct group **group);
static int parse_any(struct any **any);
static int parse_eos(struct eos **eos);
static int parse_character(struct character **character);
static int parse_set(struct set **set);

static int parse_re(struct re **reOut){
	struct re re;

	// TODO: find first sets
	printf("<re> : <simple_re> <union>\n");
	re.rule = 1;
	parse_simple_re(&re.simple_re);
	parse_union(&re.union_);

	if(reOut != 0){
		*reOut = calloc(1, sizeof(struct re));
		if(*reOut != 0){
			**reOut = re;
		} else {
			// TODO: error code
			fprintf(stderr, "memory error in parse_re()\n");
			exit(1);
			//return -1;
		}
	}
	return 0;
}

static int parse_union(struct union_ **unionOut){
	int result;
	struct union_ union_;

	if(lookAhead == '|'){
		printf("<union_> : '|' <simple_re> <union_>\n");
		union_.rule = 1;
		nextToken();
		parse_simple_re(&union_.simple_re);
		parse_union(&union_.union_);
	//} else if(lookAhead != ')' && lookAhead != '\0'){
	} else {
		// TODO: find follow()
		printf("<union_> : {}\n");
		union_.rule = 2;
	}

	if(unionOut){
		*unionOut = calloc(1, sizeof(struct union_));
		if(*unionOut != 0){
			**unionOut = union_;
		} else {
			fprintf(stderr, "memory error in parse_union()\n");
			exit(1);
		}
	}
	return 0;
}

static int parse_simple_re(struct simple_re **simpleReOut){
	struct simple_re simple_re = {0};

	// TODO: compute FIRST set of <concatenation>
	if(lookAhead != ')' && lookAhead != '|' && lookAhead != '\0'){
		printf("\t<simple_re> : <basic_re> <concatenation>\n");
		simple_re.rule = 1;
		parse_basic_re(&simple_re.basic_re);
		parse_concatenation(&simple_re.concatenation);
	} else {
		fprintf(stderr, "invalid simple_re\n");
		exit(1);
	}

	if(simpleReOut){
		*simpleReOut = calloc(1, sizeof(struct simple_re));
		if(*simpleReOut){
			**simpleReOut = simple_re;
		} else {
			fprintf(stderr, "memory error at parse_simple_re()\n");
			exit(1);
		}
	}
	return 0;
}

static int parse_concatenation(struct concatenation **concatenationOut){
	struct concatenation concatenation = {0};

	// TODO: compute FIRST set of <basic_re>
	if(lookAhead != ')' && lookAhead != '|' && lookAhead != '\0'){ 
		printf("\t<concatenation> : <basic_re> <concatenation>\n");
		concatenation.rule = 1;
		parse_basic_re(&concatenation.basic_re);
		parse_concatenation(&concatenation.concatenation);
	} else {
		printf("\t<concatenation> : {}\n");
		concatenation.rule = 2;
	}

	if(concatenationOut){
		*concatenationOut = calloc(1, sizeof(struct concatenation));
		if(*concatenationOut){
			**concatenationOut = concatenation;
		} else {
			fprintf(stderr, "memory error at parse_concatenation()\n");
			exit(1);
		}
	}
	return 0;
}

static int parse_basic_re(struct basic_re **basicReOut){
	struct basic_re basic_re;

	printf("\t<basic_re> : <elementary_re> <basic_re_prime>\n");
	basic_re.rule = 1;
	parse_elementary_re(&basic_re.elementary_re);
	parse_basic_re_prime(&basic_re.basic_re_prime);

	if(basicReOut){
		*basicReOut = calloc(1, sizeof(struct basic_re));
		if(*basicReOut){
			**basicReOut = basic_re;
		} else {
			fprintf(stderr, "memory error at parse_basic_re()\n");
			exit(1);
		}
	}
	return 0;
}

static int parse_basic_re_prime(struct basic_re_prime **basicRePrimeOut){
	struct basic_re_prime basic_re_prime = {0};

	if(lookAhead == '*'){
		basic_re_prime.rule = 1;
		printf("\t<basic_re_prime> : '*' <basic_re_prime>\n");
		nextToken();
		parse_basic_re_prime(&basic_re_prime.basic_re_prime);
	} else if(lookAhead == '+'){
		basic_re_prime.rule = 2;
		printf("\t<basic_re_prime> : '+' <basic_re_prime>\n");
		nextToken();
		parse_basic_re_prime(&basic_re_prime.basic_re_prime);
	} else if(lookAhead == '?'){
		basic_re_prime.rule = 3;
		printf("\t<basic_re_prime> : '?' <basic_re_prime>\n");
		nextToken();
		parse_basic_re_prime(&basic_re_prime.basic_re_prime);
	} else {
		// sigma production
		basic_re_prime.rule = 4;
		printf("\t<basic_re_prime> : {}\n");
		;
	}

	if(basicRePrimeOut){
		*basicRePrimeOut = calloc(1, sizeof(struct basic_re_prime));
		if(*basicRePrimeOut){
			**basicRePrimeOut = basic_re_prime;
		} else {
			fprintf(stderr, "memory error\n");
			exit(1);
		}
	}
	return 0;
}

static int parse_elementary_re(struct elementary_re **elementaryReOut){
	struct elementary_re elementary_re = {0};

	if(lookAhead == '('){
		printf("\t<elementary_re> : <group>\n");
		elementary_re.rule = 1;
		parse_group(&elementary_re.group);
	} else if(lookAhead == '.'){
		printf("\t<elementary_re> : <any>\n");
		elementary_re.rule = 2;
		parse_any(&elementary_re.any);
	} else if(lookAhead == '$'){
		printf("\t<elementary_re> : <eos>\n");
		elementary_re.rule = 3;
		parse_eos(&elementary_re.eos);
	} else if(!isMetaChar(lookAhead) || lookAhead == '\\'){
		printf("\t<elementary_re> : <character>\n");
		elementary_re.rule = 4;
		parse_character(&elementary_re.character);
	} else if(lookAhead == '['){
		printf("\t<elementary_re> : <set>\n");
		elementary_re.rule = 5;
		parse_set(&elementary_re.set);
	} else {
		fprintf(stderr, "invalid elementary_re\n");
		exit(1);
	}

	if(elementaryReOut){
		*elementaryReOut = calloc(1, sizeof(struct elementary_re));
		**elementaryReOut = elementary_re;
	}
	return 0;
}

static int parse_group(struct group **groupOut){
	struct group group = {0};

	if(lookAhead == '('){
		printf("\t<group> : '(' <basic_re> ')'\n");
		group.rule = 1;
		nextToken();
		parse_re(&group.re);
		if(lookAhead == ')'){
			nextToken();
		} else {
			fprintf(stderr, "invalid group (2)\n");
			exit(1);
		}
	} else {
		fprintf(stderr, "invalid group (1)\n");
		exit(1);
	}

	if(groupOut){
		*groupOut = calloc(1, sizeof(struct group));
		**groupOut = group;
	}
	return 0;
}

static int parse_any(struct any **anyOut){
	struct any any;

	if(lookAhead == '.'){
		any.rule = 1;
		printf("\t<any> : '.'\n");
		nextToken();
	} else {
		fprintf(stderr, "invalid any\n");
		exit(1);
	}

	if(anyOut){
		*anyOut = calloc(1, sizeof(struct any));
		**anyOut = any;
	}
	return 0;
}

static int parse_eos(struct eos **eosOut){
	struct eos eos = {0};

	if(lookAhead == '$'){
		eos.rule = 1;
		printf("\t<eof> : '$'\n");
		nextToken();
	} else {
		fprintf(stderr, "invalid eos\n");
		exit(1);
	}

	*eosOut = calloc(1, sizeof(struct eos));
	**eosOut = eos;
	return 0;
}

static int parse_character(struct character **characterOut){
	struct character character;

	if(!isMetaChar(lookAhead)){
		printf("\t<character> : <nonmetachar>\n");
		nextToken();
	} else if(lookAhead == '\\'){
		printf("\t<character> : '\\' <metachar>\n");
		nextToken();
		if(isMetaChar(lookAhead)){
			nextToken();
		} else {
			fprintf(stderr, "invalid character (2)\n");
			exit(1);
		}
	} else {
		fprintf(stderr, "invalid character (1)\n");
		exit(1);
	}

	*characterOut = calloc(1, sizeof(struct character));
	**characterOut = character;
	return 0;
}

static int parse_set(struct set **set){
	printf("<set> : ???\n");
	fprintf(stderr, "not implemented: set\n");
	exit(1);
}


static int walk_re(struct re *re);
static int walk_union(struct union_ *union_);
static int walk_simple_re(struct simple_re *simple_re);
static int walk_concatenation(struct concatenation *concatenation);
static int walk_basic_re(struct basic_re *basic_re);
static int walk_basic_re_prime(struct basic_re_prime *basic_re_prime);
static int walk_elementary_re(struct elementary_re *elementary_re);
static int walk_group(struct group *group);
static int walk_any(struct any *any);
static int walk_eos(struct eos *eos);
static int walk_character(struct character *character);
static int walk_set(struct set *set);
static int walk_nonmetachar(struct nonmetachar *nonmetachar);
static int walk_metachar(struct metachar *metachar);

static int walk_re(struct re *re){
	if(re->rule == 1){
		walk_simple_re(re->simple_re);
		walk_union(re->union_);
	} else {
		fprintf(stderr, "invalid rule: re: %d\n", re->rule);
		exit(1);
	}
	return 0;
}
static int walk_union(struct union_ *union_){
	if(union_->rule == 1){
		walk_simple_re(union_->simple_re);
		walk_union(union_->union_);
	} else if(union_->rule == 2){
		; // SIGMA
	} else {
		fprintf(stderr, "invalid rule: union: %d\n", union_->rule);
		exit(1);
	}
	return 0;
}

static int walk_simple_re(struct simple_re *simple_re){
	int rule = simple_re->rule;

	if(simple_re->rule == 1){
		walk_basic_re(simple_re->basic_re);
		walk_concatenation(simple_re->concatenation);
	} else {
		fprintf(stderr, "invalid rule: simple_re: %d\n", rule);
		exit(1);
	}
	return 0;
}

static int walk_concatenation(struct concatenation *concatenation){
	int rule = concatenation->rule;
	if(rule == 1){
		walk_basic_re(concatenation->basic_re);
		walk_concatenation(concatenation->concatenation);
	} else if(rule == 2){
		; // SIGMA
	} else {
		fprintf(stderr, "invalid rule: concatenation: %d\n", rule);
		exit(1);
	}
	return 0;
}

static int walk_basic_re(struct basic_re *basic_re){
	int rule = basic_re-> rule;
	if(rule == 1){
		walk_elementary_re(basic_re->elementary_re);
		walk_basic_re_prime(basic_re->basic_re_prime);
	} else {
		fprintf(stderr, "invalid rule: basic_re: %d\n", rule);
		exit(1);
	}
	return 0;
}

static int walk_basic_re_prime(struct basic_re_prime *basic_re_prime){
	int rule = basic_re_prime->rule;

	if(rule == 1){
		walk_basic_re_prime(basic_re_prime->basic_re_prime);
	} else if(rule == 2){
		walk_basic_re_prime(basic_re_prime->basic_re_prime);
	} else if(rule == 3){
		walk_basic_re_prime(basic_re_prime->basic_re_prime);
	} else if(rule == 4){
		; // SIGMA
	} else {
		fprintf(stderr, "invalid rule: basic_re_prime: %d\n", rule);
		exit(1);
	}
	return 0;
}

static int walk_elementary_re(struct elementary_re *elementary_re){
	int rule = elementary_re->rule;

	if(rule == 1){
		walk_group(elementary_re->group);
	} else if(rule == 2){
		walk_any(elementary_re->any);
	} else if(rule == 3){
		walk_eos(elementary_re->eos);
	} else if(rule == 4){
		walk_character(elementary_re->character);
	} else if(rule == 5){
		walk_set(elementary_re->set);
	} else {
		fprintf(stderr, "inavlid rule: elementary_re: %d\n", rule);
		exit(1);
	}
	return 0;
}

static int walk_group(struct group *group){
	int rule = group->rule;
	if(rule == 1){
		walk_re(group->re);
	} else {
		fprintf(stderr, "invalid rule: group: %d\n", rule);
		exit(1);
	}
	return 0;
}

static int walk_any(struct any *any){
	int rule = any->rule;
	if(rule == 1){
		; // consumes '.'
	} else {
		fprintf(stderr, "invalid rule: any: %d\n", rule);
		exit(1);
	}
	return 0;
}

static int walk_eos(struct eos *eos){
	if(eos->rule == 1){
		; // consumes '$'
	} else {
		fprintf(stderr, "invalid rule: eos: %d\n", eos->rule);
	}
	return 0;
}

static int walk_character(struct character *character){
	return 0;
}

static int walk_set(struct set *set){
	return 0;
}

static int walk_nonmetachar(struct nonmetachar *nonmetachar){
	return 0;
}

static int walk_metachar(struct metachar *metachar){
	return 0;
}


int parse(){
	int result;
	struct re *re;

	inputIndex = 0;
	lookAhead = '\0';

	nextToken();

	result = parse_re(&re);
	if(result == 0){
		PARSETREE = re;
		walk_re(PARSETREE);
		return 0;
	} else {
		return result;
	}
}
