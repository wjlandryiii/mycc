/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef TOKENS_H
#define TOKENS_H

struct pptoken {
	char *whiteSpace;
	int hasNewLine;
	char *lexeme;
	int type;
	int name;
};

enum PPTOKEN_NAMES {
/* EOF     */	PPTN_EOF,
/* <foo.h> */	PPTN_HEADERNAME,
/* ABCD    */	PPTN_IDENTIFIER,
/* 1234    */	PPTN_PPNUMBER,
/* 'A'     */	PPTN_CHARACTERCONSTANT,
/* "asdf"  */	PPTN_STRINGLITERAL,
/* if      */	PPTN_IF,
/* ifdef   */	PPTN_IFDEF,
/* ifndef  */	PPTN_IFNDEF,
/* elif    */	PPTN_ELIF,
/* else    */	PPTN_ELSE,
/* endif   */	PPTN_ENDIF,
/* include */	PPTN_INCLUDE,
/* define  */	PPTN_DEFINE,
/* undef   */	PPTN_UNDEF,
/* line    */	PPTN_LINE,
/* error   */	PPTN_ERROR,
/* pragma  */	PPTN_PRAGMA,
/* \n      */	PPTN_NEWLINE,
/* !=      */	PPTN_BANG_EQUAL,
/* !       */	PPTN_BANG,
/* ##      */	PPTN_HASH_HASH,
/* #       */   PPTN_HASH,
/* &&      */   PPTN_AMPERSAND_AMPERSAND,
/* &=      */   PPTN_AMPERSAND_EQUAL,
/* &       */	PPTN_AMPERSAND,
/* %=      */	PPTN_PERCENT_EQUAL,
/* %       */	PPTN_PERCENT,
/* (       */	PPTN_LPAREN,
/* )       */	PPTN_RPAREN,
/* *=      */	PPTN_ASTERISK_EQUAL,
/* *       */   PPTN_ASTERISK,
/* ++      */	PPTN_PLUS_PLUS,
/* +=      */	PPTN_PLUS_EQUAL,
/* +       */   PPTN_PLUS,
/* ,       */	PPTN_COMMA,
/* --      */	PPTN_MINUS_MINUS,
/* -=      */	PPTN_MINUS_EQUAL,
/* ->      */	PPTN_MINUS_RCHEVRON,
/* -       */	PPTN_MINUS,
/* ...     */	PPTN_PERIOD_PERIOD_PERIOD,
/* .       */	PPTN_PERIOD,
/* /=      */	PPTN_SLASH_EQUAL,
/* /       */	PPTN_SLASH,
/* :       */	PPTN_COLON,
/* ;       */	PPTN_SEMICOLON,
/* <<=     */	PPTN_LCHEVRON_LCHEVRON_EQUAL,
/* <<      */	PPTN_LCHEVRON_LCHEVRON,
/* <=      */	PPTN_LCHEVRON_EQUAL,
/* <       */	PPTN_LCHEVRON,
/* ==      */	PPTN_EQUAL_EQUAL,
/* =       */	PPTN_EQUAL,
/* >>=     */	PPTN_RCHEVRON_RCHEVRON_EQUAL,
/* >>      */	PPTN_RCHEVRON_RCHEVRON,
/* >=      */	PPTN_RCHEVRON_EQUAL,
/* >       */	PPTN_RCHEVRON,
/* ?       */	PPTN_QUESTION,
/* [       */	PPTN_LBRACKET,
/* ]       */	PPTN_RBRACKET,
/* ^=      */	PPTN_CIRCUMFLEX_EQUAL,
/* ^       */	PPTN_CIRCUMFLEX,
/* {       */	PPTN_LBRACE,
/* |=      */	PPTN_PIPE_EQUAL,
/* ||      */	PPTN_PIPE_PIPE,
/* |       */	PPTN_PIPE,
/* }       */	PPTN_RBRACE,
/* ~       */	PPTN_TILDE,
/* @\?     */	PPTN_OTHER,
		PPTN_NTOKENS,
};

/*
 * preprocessing-token:
 *	header-name
 *	identifier
 *	pp-number
 *	character-constant
 *	string-literal
 *	punctuator
 *	whitespace
 *	newline
 */

enum LEXEME_TYPES {
	LT_EMPTY,
	LT_HEADERNAME,
	LT_IDENTIFIER,
	LT_PPNUMBER,
	LT_CHARACTERCONSTANT,
	LT_STRINGLITERAL,
	LT_PUNCTUATOR,
	LT_NEWLINE,
};

extern char *PPTOKEN_STRINGS[PPTN_NTOKENS];
void printPPToken(struct pptoken pptoken);


struct pptoken_queue;

struct pptoken_queue *newPPTokenQueue();
void freePPTokenQueue(struct pptoken_queue *queue);
int ppTokenQueueEnqueue(struct pptoken_queue *queue, struct pptoken token);
int ppTokenQueueDequeue(struct pptoken_queue *queue, struct pptoken *tokenOut);


struct pptoken_list;

struct pptoken_list *newPPTokenList();
void freePPTokenList(struct pptoken_list *list);
int ppTokenListCount(struct pptoken_list *list);
int ppTokenListAtIndex(struct pptoken_list *list, int index, struct pptoken *tokenOut);
int ppTokenListAppend(struct pptoken_list *list, struct pptoken token);


#endif
