/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "escstr.h"

enum {
	ESC_NEEDED	= 1<<0,
	ESC_SIMPLE	= 1<<1,
	ESC_HEX		= 1<<2,
};

static const int charescbits[256] = {
/* 0x00: ^@   */	ESC_NEEDED | ESC_HEX,
/* 0x01: ^A   */	ESC_NEEDED | ESC_HEX,
/* 0x02: ^B   */	ESC_NEEDED | ESC_HEX,
/* 0x03: ^C   */	ESC_NEEDED | ESC_HEX,
/* 0x04: ^D   */	ESC_NEEDED | ESC_HEX,
/* 0x05: ^E   */	ESC_NEEDED | ESC_HEX,
/* 0x06: ^F   */	ESC_NEEDED | ESC_HEX,
/* 0x07: '\a' */	ESC_NEEDED | ESC_SIMPLE,
/* 0x08: '\b' */	ESC_NEEDED | ESC_SIMPLE,
/* 0x09: '\t' */	ESC_NEEDED | ESC_SIMPLE,
/* 0x0a: '\n' */	ESC_NEEDED | ESC_SIMPLE,
/* 0x0b: '\v' */	ESC_NEEDED | ESC_SIMPLE,
/* 0x0c: '\f' */	ESC_NEEDED | ESC_SIMPLE,
/* 0x0d: '\r' */	ESC_NEEDED | ESC_SIMPLE,
/* 0x0e: ^N   */	ESC_NEEDED | ESC_HEX,
/* 0x0f: ^O   */	ESC_NEEDED | ESC_HEX,
/* 0x10: ^P   */	ESC_NEEDED | ESC_HEX,
/* 0x11: ^Q   */	ESC_NEEDED | ESC_HEX,
/* 0x12: ^R   */	ESC_NEEDED | ESC_HEX,
/* 0x13: ^S   */	ESC_NEEDED | ESC_HEX,
/* 0x14: ^T   */	ESC_NEEDED | ESC_HEX,
/* 0x15: ^U   */	ESC_NEEDED | ESC_HEX,
/* 0x16: ^V   */	ESC_NEEDED | ESC_HEX,
/* 0x17: ^W   */	ESC_NEEDED | ESC_HEX,
/* 0x18: ^X   */	ESC_NEEDED | ESC_HEX,
/* 0x19: ^Y   */	ESC_NEEDED | ESC_HEX,
/* 0x1a: ^Z   */	ESC_NEEDED | ESC_HEX,
/* 0x1b: ^[   */	ESC_NEEDED | ESC_HEX,
/* 0x1c: ^\   */	ESC_NEEDED | ESC_HEX,
/* 0x1d: ^]   */	ESC_NEEDED | ESC_HEX,
/* 0x1e: ^^   */	ESC_NEEDED | ESC_HEX,
/* 0x1f: ^_   */	ESC_NEEDED | ESC_HEX,
/* 0x20: ' '  */	0,
/* 0x21: '!'  */	0,
/* 0x22: '\"' */	ESC_NEEDED | ESC_SIMPLE,
/* 0x23: '#'  */	0,
/* 0x24: '$'  */	0,
/* 0x25: '%'  */	0,
/* 0x26: '&'  */	0,
/* 0x27: '\'' */	ESC_NEEDED | ESC_SIMPLE,
/* 0x28: '('  */	0,
/* 0x29: ')'  */	0,
/* 0x2a: '*'  */	0,
/* 0x2b: '+'  */	0,
/* 0x2c: ','  */	0,
/* 0x2d: '-'  */	0,
/* 0x2e: '.'  */	0,
/* 0x2f: '/'  */	0,
/* 0x30: '0'  */	0,
/* 0x31: '1'  */	0,
/* 0x32: '2'  */	0,
/* 0x33: '3'  */	0,
/* 0x34: '4'  */	0,
/* 0x35: '5'  */	0,
/* 0x36: '6'  */	0,
/* 0x37: '7'  */	0,
/* 0x38: '8'  */	0,
/* 0x39: '9'  */	0,
/* 0x3a: ':'  */	0,
/* 0x3b: ';'  */	0,
/* 0x3c: '<'  */	0,
/* 0x3d: '='  */	0,
/* 0x3e: '>'  */	0,
/* 0x3f: '\?' */	ESC_NEEDED | ESC_SIMPLE,
/* 0x40: '@'  */	0,
/* 0x41: 'A'  */	0,
/* 0x42: 'B'  */	0,
/* 0x43: 'C'  */	0,
/* 0x44: 'D'  */	0,
/* 0x45: 'E'  */	0,
/* 0x46: 'F'  */	0,
/* 0x47: 'G'  */	0,
/* 0x48: 'H'  */	0,
/* 0x49: 'I'  */	0,
/* 0x4a: 'J'  */	0,
/* 0x4b: 'K'  */	0,
/* 0x4c: 'L'  */	0,
/* 0x4d: 'M'  */	0,
/* 0x4e: 'N'  */	0,
/* 0x4f: 'O'  */	0,
/* 0x50: 'P'  */	0,
/* 0x51: 'Q'  */	0,
/* 0x52: 'R'  */	0,
/* 0x53: 'S'  */	0,
/* 0x54: 'T'  */	0,
/* 0x55: 'U'  */	0,
/* 0x56: 'V'  */	0,
/* 0x57: 'W'  */	0,
/* 0x58: 'X'  */	0,
/* 0x59: 'Y'  */	0,
/* 0x5a: 'Z'  */	0,
/* 0x5b: '['  */	0,
/* 0x5c: '\\' */	ESC_NEEDED | ESC_SIMPLE,
/* 0x5d: ']'  */	0,
/* 0x5e: '^'  */	0,
/* 0x5f: '_'  */	0,
/* 0x60: '`'  */	0,
/* 0x61: 'a'  */	0,
/* 0x62: 'b'  */	0,
/* 0x63: 'c'  */	0,
/* 0x64: 'd'  */	0,
/* 0x65: 'e'  */	0,
/* 0x66: 'f'  */	0,
/* 0x67: 'g'  */	0,
/* 0x68: 'h'  */	0,
/* 0x69: 'i'  */	0,
/* 0x6a: 'j'  */	0,
/* 0x6b: 'k'  */	0,
/* 0x6c: 'l'  */	0,
/* 0x6d: 'm'  */	0,
/* 0x6e: 'n'  */	0,
/* 0x6f: 'o'  */	0,
/* 0x70: 'p'  */	0,
/* 0x71: 'q'  */	0,
/* 0x72: 'r'  */	0,
/* 0x73: 's'  */	0,
/* 0x74: 't'  */	0,
/* 0x75: 'u'  */	0,
/* 0x76: 'v'  */	0,
/* 0x77: 'w'  */	0,
/* 0x78: 'x'  */	0,
/* 0x79: 'y'  */	0,
/* 0x7a: 'z'  */	0,
/* 0x7b: '{'  */	0,
/* 0x7c: '|'  */	0,
/* 0x7d: '}'  */	0,
/* 0x7e: '~'  */	0,
/* 0x7f: ^?   */	ESC_NEEDED | ESC_HEX,
/* 0x80:      */	ESC_NEEDED | ESC_HEX,
/* 0x81:      */	ESC_NEEDED | ESC_HEX,
/* 0x82:      */	ESC_NEEDED | ESC_HEX,
/* 0x83:      */	ESC_NEEDED | ESC_HEX,
/* 0x84:      */	ESC_NEEDED | ESC_HEX,
/* 0x85:      */	ESC_NEEDED | ESC_HEX,
/* 0x86:      */	ESC_NEEDED | ESC_HEX,
/* 0x87:      */	ESC_NEEDED | ESC_HEX,
/* 0x88:      */	ESC_NEEDED | ESC_HEX,
/* 0x89:      */	ESC_NEEDED | ESC_HEX,
/* 0x8a:      */	ESC_NEEDED | ESC_HEX,
/* 0x8b:      */	ESC_NEEDED | ESC_HEX,
/* 0x8c:      */	ESC_NEEDED | ESC_HEX,
/* 0x8d:      */	ESC_NEEDED | ESC_HEX,
/* 0x8e:      */	ESC_NEEDED | ESC_HEX,
/* 0x8f:      */	ESC_NEEDED | ESC_HEX,
/* 0x90:      */	ESC_NEEDED | ESC_HEX,
/* 0x91:      */	ESC_NEEDED | ESC_HEX,
/* 0x92:      */	ESC_NEEDED | ESC_HEX,
/* 0x93:      */	ESC_NEEDED | ESC_HEX,
/* 0x94:      */	ESC_NEEDED | ESC_HEX,
/* 0x95:      */	ESC_NEEDED | ESC_HEX,
/* 0x96:      */	ESC_NEEDED | ESC_HEX,
/* 0x97:      */	ESC_NEEDED | ESC_HEX,
/* 0x98:      */	ESC_NEEDED | ESC_HEX,
/* 0x99:      */	ESC_NEEDED | ESC_HEX,
/* 0x9a:      */	ESC_NEEDED | ESC_HEX,
/* 0x9b:      */	ESC_NEEDED | ESC_HEX,
/* 0x9c:      */	ESC_NEEDED | ESC_HEX,
/* 0x9d:      */	ESC_NEEDED | ESC_HEX,
/* 0x9e:      */	ESC_NEEDED | ESC_HEX,
/* 0x9f:      */	ESC_NEEDED | ESC_HEX,
/* 0xa0:      */	ESC_NEEDED | ESC_HEX,
/* 0xa1:      */	ESC_NEEDED | ESC_HEX,
/* 0xa2:      */	ESC_NEEDED | ESC_HEX,
/* 0xa3:      */	ESC_NEEDED | ESC_HEX,
/* 0xa4:      */	ESC_NEEDED | ESC_HEX,
/* 0xa5:      */	ESC_NEEDED | ESC_HEX,
/* 0xa6:      */	ESC_NEEDED | ESC_HEX,
/* 0xa7:      */	ESC_NEEDED | ESC_HEX,
/* 0xa8:      */	ESC_NEEDED | ESC_HEX,
/* 0xa9:      */	ESC_NEEDED | ESC_HEX,
/* 0xaa:      */	ESC_NEEDED | ESC_HEX,
/* 0xab:      */	ESC_NEEDED | ESC_HEX,
/* 0xac:      */	ESC_NEEDED | ESC_HEX,
/* 0xad:      */	ESC_NEEDED | ESC_HEX,
/* 0xae:      */	ESC_NEEDED | ESC_HEX,
/* 0xaf:      */	ESC_NEEDED | ESC_HEX,
/* 0xb0:      */	ESC_NEEDED | ESC_HEX,
/* 0xb1:      */	ESC_NEEDED | ESC_HEX,
/* 0xb2:      */	ESC_NEEDED | ESC_HEX,
/* 0xb3:      */	ESC_NEEDED | ESC_HEX,
/* 0xb4:      */	ESC_NEEDED | ESC_HEX,
/* 0xb5:      */	ESC_NEEDED | ESC_HEX,
/* 0xb6:      */	ESC_NEEDED | ESC_HEX,
/* 0xb7:      */	ESC_NEEDED | ESC_HEX,
/* 0xb8:      */	ESC_NEEDED | ESC_HEX,
/* 0xb9:      */	ESC_NEEDED | ESC_HEX,
/* 0xba:      */	ESC_NEEDED | ESC_HEX,
/* 0xbb:      */	ESC_NEEDED | ESC_HEX,
/* 0xbc:      */	ESC_NEEDED | ESC_HEX,
/* 0xbd:      */	ESC_NEEDED | ESC_HEX,
/* 0xbe:      */	ESC_NEEDED | ESC_HEX,
/* 0xbf:      */	ESC_NEEDED | ESC_HEX,
/* 0xc0:      */	ESC_NEEDED | ESC_HEX,
/* 0xc1:      */	ESC_NEEDED | ESC_HEX,
/* 0xc2:      */	ESC_NEEDED | ESC_HEX,
/* 0xc3:      */	ESC_NEEDED | ESC_HEX,
/* 0xc4:      */	ESC_NEEDED | ESC_HEX,
/* 0xc5:      */	ESC_NEEDED | ESC_HEX,
/* 0xc6:      */	ESC_NEEDED | ESC_HEX,
/* 0xc7:      */	ESC_NEEDED | ESC_HEX,
/* 0xc8:      */	ESC_NEEDED | ESC_HEX,
/* 0xc9:      */	ESC_NEEDED | ESC_HEX,
/* 0xca:      */	ESC_NEEDED | ESC_HEX,
/* 0xcb:      */	ESC_NEEDED | ESC_HEX,
/* 0xcc:      */	ESC_NEEDED | ESC_HEX,
/* 0xcd:      */	ESC_NEEDED | ESC_HEX,
/* 0xce:      */	ESC_NEEDED | ESC_HEX,
/* 0xcf:      */	ESC_NEEDED | ESC_HEX,
/* 0xd0:      */	ESC_NEEDED | ESC_HEX,
/* 0xd1:      */	ESC_NEEDED | ESC_HEX,
/* 0xd2:      */	ESC_NEEDED | ESC_HEX,
/* 0xd3:      */	ESC_NEEDED | ESC_HEX,
/* 0xd4:      */	ESC_NEEDED | ESC_HEX,
/* 0xd5:      */	ESC_NEEDED | ESC_HEX,
/* 0xd6:      */	ESC_NEEDED | ESC_HEX,
/* 0xd7:      */	ESC_NEEDED | ESC_HEX,
/* 0xd8:      */	ESC_NEEDED | ESC_HEX,
/* 0xd9:      */	ESC_NEEDED | ESC_HEX,
/* 0xda:      */	ESC_NEEDED | ESC_HEX,
/* 0xdb:      */	ESC_NEEDED | ESC_HEX,
/* 0xdc:      */	ESC_NEEDED | ESC_HEX,
/* 0xdd:      */	ESC_NEEDED | ESC_HEX,
/* 0xde:      */	ESC_NEEDED | ESC_HEX,
/* 0xdf:      */	ESC_NEEDED | ESC_HEX,
/* 0xe0:      */	ESC_NEEDED | ESC_HEX,
/* 0xe1:      */	ESC_NEEDED | ESC_HEX,
/* 0xe2:      */	ESC_NEEDED | ESC_HEX,
/* 0xe3:      */	ESC_NEEDED | ESC_HEX,
/* 0xe4:      */	ESC_NEEDED | ESC_HEX,
/* 0xe5:      */	ESC_NEEDED | ESC_HEX,
/* 0xe6:      */	ESC_NEEDED | ESC_HEX,
/* 0xe7:      */	ESC_NEEDED | ESC_HEX,
/* 0xe8:      */	ESC_NEEDED | ESC_HEX,
/* 0xe9:      */	ESC_NEEDED | ESC_HEX,
/* 0xea:      */	ESC_NEEDED | ESC_HEX,
/* 0xeb:      */	ESC_NEEDED | ESC_HEX,
/* 0xec:      */	ESC_NEEDED | ESC_HEX,
/* 0xed:      */	ESC_NEEDED | ESC_HEX,
/* 0xee:      */	ESC_NEEDED | ESC_HEX,
/* 0xef:      */	ESC_NEEDED | ESC_HEX,
/* 0xf0:      */	ESC_NEEDED | ESC_HEX,
/* 0xf1:      */	ESC_NEEDED | ESC_HEX,
/* 0xf2:      */	ESC_NEEDED | ESC_HEX,
/* 0xf3:      */	ESC_NEEDED | ESC_HEX,
/* 0xf4:      */	ESC_NEEDED | ESC_HEX,
/* 0xf5:      */	ESC_NEEDED | ESC_HEX,
/* 0xf6:      */	ESC_NEEDED | ESC_HEX,
/* 0xf7:      */	ESC_NEEDED | ESC_HEX,
/* 0xf8:      */	ESC_NEEDED | ESC_HEX,
/* 0xf9:      */	ESC_NEEDED | ESC_HEX,
/* 0xfa:      */	ESC_NEEDED | ESC_HEX,
/* 0xfb:      */	ESC_NEEDED | ESC_HEX,
/* 0xfc:      */	ESC_NEEDED | ESC_HEX,
/* 0xfd:      */	ESC_NEEDED | ESC_HEX,
/* 0xfe:      */	ESC_NEEDED | ESC_HEX,
/* 0xff:      */	ESC_NEEDED | ESC_HEX,
};

static int charescbitsTest(int c, int flags){
	int i;
	i = (unsigned int)(char)c;
	return (charescbits[i] & flags) == flags;
}

static int isEscNeeded(int c)		{ return charescbitsTest(c, ESC_NEEDED); }
static int isEscSimple(int c)		{ return charescbitsTest(c, ESC_SIMPLE); }
static int isEscHex(int c)		{ return charescbitsTest(c, ESC_HEX); }

static int simpleEscChar(int c){
	if(c == '\n'){
		return 'n';
	} else if(c == '\t'){
		return 't';
	} else if(c == '\v'){
		return 'v';
	} else if(c == '\b'){
		return 'b';
	} else if(c == '\r'){
		return 'r';
	} else if(c == '\f'){
		return 'f';
	} else if(c == '\a'){
		return 'a';
	} else if(c == '\\'){
		return '\\';
	} else if(c == '\?'){
		return '\?';
	} else if(c == '\''){
		return '\'';
	} else if(c == '\"'){
		return '\"';
	} else {
		fprintf(stderr, "programmer error simpleEscChar(0x%02x)\n", c);
		exit(1);
	}
}

int puts_escaped(char *s, char *e){
	while(*s && s < e){
		if(!isEscNeeded(*s)){
			putchar(*s);
		} else {
			if(isEscHex(*s)){
				printf("\\x%2x", *s);
			} else if(isEscSimple(*s)){
				printf("\\%c", simpleEscChar(*s));
			} else {
				fprintf(stderr, "programmer error puts_escapted()\n");
				exit(1);
			}
		}
		s++;
	}
	return 0;
}
