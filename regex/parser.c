/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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
		//printf("****  NEXT TOKEN: %c : %s $\n", lookAhead, &REGEXSTRING[inputIndex]);
	} else {
		//printf("****  NEXT TOKEN: $\n");
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
static int parse_nonmetachar(struct nonmetachar **nonmetacharOut);
static int parse_metachar(struct metachar **metacharOut);
static int parse_set(struct set **set);
static int parse_set_prime(struct set_prime **set_prime);

static int parse_re(struct re **reOut){
	struct re re;

	// TODO: find first sets
	//printf("<re> : <simple_re> <union>\n");
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
		//printf("<union_> : '|' <simple_re> <union_>\n");
		union_.rule = 1;
		nextToken();
		parse_simple_re(&union_.simple_re);
		parse_union(&union_.union_);
	//} else if(lookAhead != ')' && lookAhead != '\0'){
	} else {
		// TODO: find follow()
		//printf("<union_> : {}\n");
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
		//printf("\t<simple_re> : <basic_re> <concatenation>\n");
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
		//printf("\t<concatenation> : <basic_re> <concatenation>\n");
		concatenation.rule = 1;
		parse_basic_re(&concatenation.basic_re);
		parse_concatenation(&concatenation.concatenation);
	} else {
		//printf("\t<concatenation> : {}\n");
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

	//printf("\t<basic_re> : <elementary_re> <basic_re_prime>\n");
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
		//printf("\t<basic_re_prime> : '*' <basic_re_prime>\n");
		nextToken();
		parse_basic_re_prime(&basic_re_prime.basic_re_prime);
	} else if(lookAhead == '+'){
		basic_re_prime.rule = 2;
		//printf("\t<basic_re_prime> : '+' <basic_re_prime>\n");
		nextToken();
		parse_basic_re_prime(&basic_re_prime.basic_re_prime);
	} else if(lookAhead == '?'){
		basic_re_prime.rule = 3;
		//printf("\t<basic_re_prime> : '?' <basic_re_prime>\n");
		nextToken();
		parse_basic_re_prime(&basic_re_prime.basic_re_prime);
	} else {
		// sigma production
		basic_re_prime.rule = 4;
		//printf("\t<basic_re_prime> : {}\n");
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
		//printf("\t<elementary_re> : <group>\n");
		elementary_re.rule = 1;
		parse_group(&elementary_re.group);
	} else if(lookAhead == '.'){
		//printf("\t<elementary_re> : <any>\n");
		elementary_re.rule = 2;
		parse_any(&elementary_re.any);
	} else if(lookAhead == '$'){
		//printf("\t<elementary_re> : <eos>\n");
		elementary_re.rule = 3;
		parse_eos(&elementary_re.eos);
	} else if(!isMetaChar(lookAhead) || lookAhead == '\\'){
		//printf("\t<elementary_re> : <character>\n");
		elementary_re.rule = 4;
		parse_character(&elementary_re.character);
	} else if(lookAhead == '['){
		//printf("\t<elementary_re> : <set>\n");
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
		//printf("\t<group> : '(' <basic_re> ')'\n");
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
		//printf("\t<any> : '.'\n");
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
		//printf("\t<eof> : '$'\n");
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
		//printf("\t<character> : <nonmetachar>\n");
		character.rule = 1;
		parse_nonmetachar(&character.nonmetachar);
	} else if(lookAhead == '\\'){
		//printf("\t<character> : '\\' <metachar>\n");
		character.rule = 2;
		character.token = lookAhead;
		nextToken();
		if(isMetaChar(lookAhead)){
			parse_metachar(&character.metachar);
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

static int parse_nonmetachar(struct nonmetachar **nonmetacharOut){
	struct nonmetachar nonmetachar;

	if(('a' <= lookAhead && lookAhead <= 'z')
			|| ('A' <= lookAhead && lookAhead <= 'Z')
			|| ('0' <= lookAhead && lookAhead <= '9')){

		nonmetachar.rule = 1;
		nonmetachar.token = lookAhead;
		nextToken();
	} else {
		fprintf(stderr, "invalid nonmetachar: '\\x%02x'\n", lookAhead);
		exit(1);
	}

	if(nonmetacharOut){
		*nonmetacharOut = calloc(1, sizeof(struct nonmetachar));
		**nonmetacharOut = nonmetachar;
	}
	return 0;
}

static int parse_metachar(struct metachar **metacharOut){
	struct metachar metachar;

	if(isMetaChar(lookAhead)){
		metachar.rule = 1;
		metachar.token = lookAhead;
		nextToken();
	} else {
		fprintf(stderr, "invalid metachar: '\\x%02x'\n", lookAhead);
		exit(1);
	}

	if(metacharOut){
		*metacharOut = calloc(1, sizeof(struct metachar));
		**metacharOut = metachar;
	}
	return 0;
}

static int parse_set(struct set **setOut){
	struct set set;

	if(lookAhead == '['){
		set.rule = 1;
		nextToken();
		parse_set_prime(&set.set_prime);
		if(lookAhead == ']'){
			nextToken();
		} else {
			fprintf(stderr, "expected ']'\n");
			exit(1);
		}
	} else {
		fprintf(stderr, "expected '['\n");
		exit(1);
	}
	if(setOut){
		*setOut = calloc(1, sizeof(struct set));
		**setOut = set;
	}
	return 0;
}

static int parse_set_prime(struct set_prime **set_prime){

	return 0;
}



static int graphwalkNode = 0;

static int emitNode(char *firstField, ...);
static int emitEdge(int n1, int f, int n2);
static int graphwalk_re(struct re *re);
static int graphwalk_union(struct union_ *union_);
static int graphwalk_simple_re(struct simple_re *simple_re);
static int graphwalk_concatenation(struct concatenation *concatenation);
static int graphwalk_basic_re(struct basic_re *basic_re);
static int graphwalk_basic_re_prime(struct basic_re_prime *basic_re_prime);
static int graphwalk_elementary_re(struct elementary_re *elementary_re);
static int graphwalk_group(struct group *group);
static int graphwalk_any(struct any *any);
static int graphwalk_eos(struct eos *eos);
static int graphwalk_character(struct character *character);
static int graphwalk_set(struct set *set);
static int graphwalk_nonmetachar(struct nonmetachar *nonmetachar);
static int graphwalk_metachar(struct metachar *metachar);


static int emitNode(char *firstField, ...){
	int node = graphwalkNode++;
	char *s;
	int i;
	va_list args;

	printf("\tnode%d [ label = \"<f0> %s ", node, firstField);
	va_start(args, firstField);
	s = va_arg(args, char *);
	i = 1;
	while(s){
		printf(" | <f%d> %s", i++, s);
		s = va_arg(args, char *);
	}
	va_end(args);
	printf("\"]\n");
	return node;
}

static int emitLeafNode(char *s){
	int node = graphwalkNode++;
	printf("\tnode%d [ label = \"%s\"", node, s);
	printf(", shape = oval, fillcolor=green, style=filled];\n");
	return node;
}

static int emitEdge(int n1, int f, int n2){
	printf("\tnode%d:f%d -> node%d\n", n1, f, n2);
	return 0;
}


static int graphwalk_re(struct re *re){
	int node;
	int child0, child1;

	if(re->rule == 1){
		node = emitNode("simple_re", "union", 0);
		child0 = graphwalk_simple_re(re->simple_re);
		child1 = graphwalk_union(re->union_);
		emitEdge(node, 0, child0);
		emitEdge(node, 1, child1);
		return node;
	} else {
		fprintf(stderr, "invalid rule: re: %d\n", re->rule);
		exit(1);
	}
	return 0;
}
static int graphwalk_union(struct union_ *union_){
	int node;
	int simple_re_node;
	int union_node;
	int child0, child1, child2;

	if(union_->rule == 1){
		node = emitNode("\\|", "simple_re", "union", 0);	
		child0 = emitLeafNode("\\|");
		child1 = graphwalk_simple_re(union_->simple_re);
		child2 = graphwalk_union(union_->union_);
		emitEdge(node, 0, child0);
		emitEdge(node, 1, child1);
		emitEdge(node, 2, child2);
		return node;
	} else if(union_->rule == 2){
		node = emitNode("\\{\\}", 0);	
		return node;
	} else {
		fprintf(stderr, "invalid rule: union: %d\n", union_->rule);
		exit(1);
	}
	return 0;
}

static int graphwalk_simple_re(struct simple_re *simple_re){
	int node;
	int basic_re_node;
	int concatenation_node;

	if(simple_re->rule == 1){
		node = emitNode("basic_re", "concatenation", 0);
		basic_re_node = graphwalk_basic_re(simple_re->basic_re);
		concatenation_node = graphwalk_concatenation(simple_re->concatenation);
		emitEdge(node, 0, basic_re_node);
		emitEdge(node, 1, concatenation_node);
		return node;
	} else {
		fprintf(stderr, "invalid rule: simple_re: %d\n", simple_re->rule);
		exit(1);
	}
	return 0;
}

static int graphwalk_concatenation(struct concatenation *concatenation){
	int node;
	int basic_re_node;
	int concatenation_node;
	
	if(concatenation->rule == 1){
		node = emitNode("basic_re", "concatenation", 0);
		basic_re_node = graphwalk_basic_re(concatenation->basic_re);
		concatenation_node = graphwalk_concatenation(concatenation->concatenation);
		emitEdge(node, 0, basic_re_node);
		emitEdge(node, 1, concatenation_node);
		return node;
	} else if(concatenation->rule == 2){
		// SIGMA
		node = emitNode("\\{\\}", 0);
		return node;
	} else {
		fprintf(stderr, "invalid rule: concatenation: %d\n", concatenation->rule);
		exit(1);
	}
	return 0;
}

static int graphwalk_basic_re(struct basic_re *basic_re){
	int node;
	int elementary_re_node;
	int basic_re_prime;

	if(basic_re->rule == 1){
		node = emitNode("basic_re", "elementary_re", 0);
		elementary_re_node = graphwalk_elementary_re(basic_re->elementary_re);
		basic_re_prime = graphwalk_basic_re_prime(basic_re->basic_re_prime);
		emitEdge(node, 0, elementary_re_node);
		emitEdge(node, 1, basic_re_prime);
		return node;
	} else {
		fprintf(stderr, "invalid rule: basic_re: %d\n", basic_re->rule);
		exit(1);
	}
	return 0;
}

static int graphwalk_basic_re_prime(struct basic_re_prime *basic_re_prime){
	int node;
	int rule = basic_re_prime->rule;
	int child0;
	int child1;

	if(rule == 1){
		node = emitNode("'*'", "basic_re_prime", 0);
		child0 = emitLeafNode("*");
		child1 = graphwalk_basic_re_prime(basic_re_prime->basic_re_prime);
		emitEdge(node, 0, child0);
		emitEdge(node, 1, child1);
		return node;
	} else if(rule == 2){
		node = emitNode("'+'", "basic_re_prime", 0);
		child0 = emitLeafNode("+");
		child1 = graphwalk_basic_re_prime(basic_re_prime->basic_re_prime);
		emitEdge(node, 0, child0);
		emitEdge(node, 1, child1);
		return node;
	} else if(rule == 3){
		node = emitNode("'?'", "basic_re_prime", 0);
		child0 = emitLeafNode("?");
		child1 = graphwalk_basic_re_prime(basic_re_prime->basic_re_prime);
		emitEdge(node, 0, child0);
		emitEdge(node, 1, child1);
		return node;
	} else if(rule == 4){
		node = emitNode("\\{\\}", 0);
		return node;
	} else {
		fprintf(stderr, "invalid rule: basic_re_prime: %d\n", rule);
		exit(1);
	}
	return 0;
}

static int graphwalk_elementary_re(struct elementary_re *elementary_re){
	int node;
	int rule = elementary_re->rule;
	int child0;

	if(rule == 1){
		node = emitNode("\\<group\\>", 0);
		child0 = graphwalk_group(elementary_re->group);
		emitEdge(node, 0, child0);
		return node;
	} else if(rule == 2){
		node = emitNode("\\<any\\>", 0);
		child0 = graphwalk_any(elementary_re->any);
		emitEdge(node, 0, child0);
		return node;
	} else if(rule == 3){
		node = emitNode("\\<eos\\>", 0);
		child0 = graphwalk_eos(elementary_re->eos);
		emitEdge(node, 0, child0);
		return node;
	} else if(rule == 4){
		node = emitNode("\\<character\\>", 0);
		child0 = graphwalk_character(elementary_re->character);
		emitEdge(node, 0, child0);
		return node;
	} else if(rule == 5){
		node = emitNode("\\<set\\>");
		child0 = graphwalk_set(elementary_re->set);
		emitEdge(node, 0, child0);
		return node;
	} else {
		fprintf(stderr, "inavlid rule: elementary_re: %d\n", rule);
		exit(1);
	}
	return 0;
}

static int graphwalk_group(struct group *group){
	int node;
	int child0, child1, child2;

	int rule = group->rule;
	if(rule == 1){
		node = emitNode("'('", "\\<re\\>", "')'", 0);
		child0 = emitLeafNode("(");
		child1 = graphwalk_re(group->re);
		child2 = emitLeafNode(")");
		emitEdge(node, 0, child0);
		emitEdge(node, 1, child1);
		emitEdge(node, 2, child2);
		return node;
	} else {
		fprintf(stderr, "invalid rule: group: %d\n", rule);
		exit(1);
	}
	return 0;
}

static int graphwalk_any(struct any *any){
	int node;
	int rule = any->rule;

	if(rule == 1){
		node = emitLeafNode(".");
		return node;
	} else {
		fprintf(stderr, "invalid rule: any: %d\n", rule);
		exit(1);
	}
	return 0;
}

static int graphwalk_eos(struct eos *eos){
	int node;

	if(eos->rule == 1){
		node = emitNode("'$'", 0); // consumes '$'
		return node;
	} else {
		fprintf(stderr, "invalid rule: eos: %d\n", eos->rule);
	}
	return 0;
}

static int graphwalk_character(struct character *character){
	int node;
	int child0, child1;

	if(character->rule == 1){
		node = emitNode("\\<nonmetachar\\>", 0);
		child0 = graphwalk_nonmetachar(character->nonmetachar);
		emitEdge(node, 0, child0);
		return node;
	} else if(character->rule == 2){
		node = emitNode("\\\\", "\\<metachar\\>", 0);
		child0 = emitLeafNode("\\\\");
		child1 = graphwalk_metachar(character->metachar);
		emitEdge(node, 0, child0);
		emitEdge(node, 1, child1);
		return node;
	} else {
		fprintf(stderr, "invalid rule: character:%d\n", character->rule);
		exit(1);
	}
	return 0;
}

static int graphwalk_set(struct set *set){
	fprintf(stderr, "set not implemented!\n");
	exit(1);
	return 0;
}

static int graphwalk_nonmetachar(struct nonmetachar *nonmetachar){
	char buf[2];
	int node;

	if(nonmetachar->rule == 1){
		buf[0] = nonmetachar->token;
		buf[1] = '\0';
		node = emitLeafNode(buf);
		//node = emitNode("a-zA-Z0-9", 0);
		return node;
	} else {
		fprintf(stderr, "invalid nonmetachar rule: %d\n", nonmetachar->rule);
		exit(1);
	}
	return 0;
}

static int graphwalk_metachar(struct metachar *metachar){
	char buf[2];
	int node;

	if(metachar->rule == 1){
		buf[0] = metachar->token;
		buf[1] = '\0';
		node = emitLeafNode(buf);
		//node = emitNode("onlymeta", buf);
		return node;
	} else {
		fprintf(stderr, "invalid metachar rule: %d\n", metachar->rule);
		exit(1);
	}
	return 0;
}


int graphwalk(){
	graphwalkNode = 0;
	printf("digraph G {\n");
	printf("\tnode [shape=record]\n");
	graphwalk_re(PARSETREE);
	printf("}\n");
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
		//graphwalk_re(PARSETREE);
		graphwalk();
		return 0;
	} else {
		return result;
	}
}
