/*
 * Copyright 2015 Joseph Landry
 */

#ifndef NFA_H
#define NFA_H


#define NFAEPSILON (128)

struct transition {
	int from;
	int to;
	char symbols[256];
};

struct nfa {
	int startingState;
	int acceptingState;
	int states;
	struct transition transitions[1024];
	int nTransitions;
};

struct nfa elementaryOneCharacter(char c);
struct nfa elementaryAny();
struct nfa elementarySet(char *set);

struct nfa nfaUnion(struct nfa *nfaA, struct nfa *nfaB);
struct nfa nfaConcatenation(struct nfa *nfaA, struct nfa *nfaB);
struct nfa nfaStar(struct nfa *nfaA);
int graphNFA(const struct nfa *nfa);

#endif
