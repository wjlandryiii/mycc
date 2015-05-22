/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum token_types {
	TOK_INVALID,
	TOK_IDENTIFIER,
	TOK_KEYWORD,
	TOK_CONSTANT,
	TOK_STRING,
	TOK_OPERATOR,
	TOK_SEPARATOR,
	TOK_EOF
};

char *token_class_str[] = {
	"INVALID",
	"IDENTIFIER",
	"KEYWORD",
	"CONSTANT",
	"STRING",
	"OPERATOR",
	"SEPARATOR",
	"EOF",
};

char *keywords[] = {
	"auto",
	"break",
	"case",
	"char",
	"const",
	"continue",
	"default",
	"do",
	"double",
	"else",
	"enum",
	"extern",
	"float",
	"for",
	"goto",
	"if",
	"int",
	"long",
	"register",
	"return",
	"short",
	"signed",
	"sizeof",
	"static",
	"struct",
	"switch",
	"typedef",
	"union",
	"unsigned",
	"void",
	"volatile",
	"while",
	NULL
};

enum CONST_TYPE {
	CT_INVALID,
	CT_INTEGER,
	CT_CHARACTER,
	CT_FLOAT,
	CT_ENUM,
};

char *token_const_types[] = {
	"INVALID",
	"INTEGER",
	"CHARACTER",
	"FLOAT",
	"ENUM",
};

struct token {
	int class;
	int const_type;
	char *start;
	char *end;
};


int iskeyword(char *s, int len){
	char **words = keywords;

	while(*words != NULL){
		if(strncmp(s, *words, len) == 0){
			return 1;
		} else {
			words++;
		}
	}
	return 0;
}

int issimpleesc(int c){
	switch(c){
		case '\'':
		case '\"':
		case '\?':
		case '\\':
		case 'a':
		case 'b':
		case 'f':
		case 'n':
		case 'r':
		case 't':
		case 'v':
			return 1;
		default:
			return 0;
	}
}

int isodigit(int c){
	switch(c){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			return 1;
		default:
			return 0;
	}
}



// "++", "+=", "+",
// "--", "-=", "->", "-",
// "*=", "*",
// "/=", "/",
// "%=", "%",
// "!=", "!",
// "&&", "&=", "&",
// "||", "|=", "|",
// "<<=", "<<", "<=", "<",
// ">>=", ">>", ">=", ">",
// "^=", "^",
// "==", "=",
// "[",
// "]",
// "(",
// ")",
// "...", ".",
// "~",
// "?",
// ":",
// ",",





int next_token(char *s, struct token *token){

	while(isspace(*s)){
		s++;
	}

	token->start = s;

	if(isdigit(*s)){
		if(*s == '0'){
			s++;
			if(*s == 'x'){
				s++;
				while(isxdigit(*s)){
					s++;
				}
				if(*s == 'u' || *s == 'U'){
					s++;
				}
				if(*s == 'l' || *s == 'L'){
					s++;
				}
				token->end = s;
				token->class = TOK_CONSTANT;
				token->const_type = CT_INTEGER;
				return 0;
			} else {
				while('0' <= *s && *s <= '7'){
					s++;
				}
				if(*s == 'u' || *s == 'U'){
					s++;
				}
				if(*s == 'l' || *s == 'L'){
					s++;
				}
				token->end = s;
				token->class = TOK_CONSTANT;
				token->const_type = CT_INTEGER;
				return 0;
			}
		} else {
			while(isdigit(*s)){
				s++;
			}
			if(*s == '.'){
				s++;
				while(isdigit(*s)){
					s++;
				}
				if(*s == 'e' || *s == 'E'){
					s++;
					if(*s == '-' || *s == '+'){
						s++;
					}
					while(isdigit(*s)){
						s++;
					}
				}
				if(*s == 'f' || *s == 'F' || *s == 'l' || *s == 'L'){
					s++;
				}
				token->end = s;
				token->class = TOK_CONSTANT;
				token->const_type = CT_FLOAT;
				return 0;
			} else {
				if(*s == 'u' || *s == 'U'){
					s++;
				}
				if(*s == 'l' || *s == 'L'){
					s++;
				}
				token->end = s;
				token->class = TOK_CONSTANT;
				token->const_type = CT_INTEGER;
				return 0;
			}
		}
	} else if(*s == '\''){
		s++;
		if(*s == '\\'){
			s++;

			if(*s == 'n' || *s == 't' || *s == 'b' || *s == 'r' || *s == 'f' || *s == 'a' || *s == '\\' || *s == '?' || *s == '\'' || *s == '\"'){
				s++;
				if(*s == '\''){
					s++;
					token->end = s;
					token->class = TOK_CONSTANT;
					token->const_type = CT_CHARACTER;
					return 0;
				} else {
					return -1;
				}
			} else if(*s == 'x') {
				s++;
				if(isxdigit(*s)){
					s++;
					if(isxdigit(*s)){
						s++;
						if(*s == '\''){
							s++;
							token->end = s;
							token->class = TOK_CONSTANT;
							token->const_type = CT_CHARACTER;
							return 0;
						} else {
							return -1;
						}
					} else {
						return -1;
					}
				} else {
					return -1;
				}
			} else if('0' <= *s && *s <= '7'){
				s++;
				if('0' < *s && *s <= '7'){
					s++;
				}
				if('0' < *s && *s <= '7'){
					s++;
				}
				if(*s == '\''){
					s++;
					token->end = s;
					token->class = TOK_CONSTANT;
					token->const_type = CT_CHARACTER;
					return 0;
				} else if(issimpleesc(*s)) {
					s++;
				} else {
					return -1;
				}
			}
		} else if(isprint(*s)){
			// TODO: not all cases excluded
			s++;
			if(*s == '\''){
				s++;
				token->end = s;
				token->class = TOK_CONSTANT;
				token->const_type = CT_CHARACTER;
				return 0;
			}
		}
	} else if(isalpha(*s) || *s == '_'){
		s++;
		while(isalnum(*s) || *s == '_'){
			s++;
		}
		token->end = s;
		if(iskeyword(token->start, token->end-token->start)){
			token->class = TOK_KEYWORD;
			return 0;
		} else {
			token->class = TOK_IDENTIFIER;
			return 0;
		}
	} else if(*s == '\"'){
		s++;

		while(*s != '\"' && (isalnum(*s) || isprint(*s) || isspace(*s))){
			if(*s != '\\'){
				s++;
			} else {
				s++;
				if(*s == 'x'){
					s++;
					if(isxdigit(*s)){
						s++;
						if(isxdigit(*s)){
							s++;
						} else {
							return -1;
						}
					} else {
						return -1;
					}
				} else if(isodigit(*s)){
					s++;
					if(isodigit(*s)){
						s++;
					}
					if(isodigit(*s)){
						s++;
					}
				} else  {
					s++;
				}
			}
		}
		if(*s == '\"'){
			s++;
			token->class = TOK_STRING;
			token->end = s;
			return 0;
		} else {
			return -1;
		}
	} else if(*s == '+'){
		s++;
		if(*s == '+' || *s == '='){
			s++;
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '-'){
		s++;
		if(*s == '-' || *s == '=' || *s == '>'){
			s++;
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '*'){
		s++;
		if(*s == '='){
			s++;
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '/'){
		s++;
		if(*s == '='){
			s++;
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '%'){
		s++;
		if(*s == '='){
			s++;
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '!'){
		s++;
		if(*s == '='){
			s++;
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '&'){
		s++;
		if(*s == '&' || *s == '='){
			s++;
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '|'){
		s++;
		if(*s == '|' || *s == '='){
			s++;
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '<'){
		s++;
		if(*s == '='){
			s++;
		} else if(*s == '<'){
			s++;
			if(*s == '='){
				s++;
			}
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '>'){
		s++;
		if(*s == '='){
			s++;
		} else if(*s == '>'){
			s++;
			if(*s == '='){
				s++;
			}
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '^'){
		s++;
		if(*s == '='){
			s++;
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '='){
		s++;
		if(*s == '='){
			s++;
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(*s == '.'){
		s++;
		if(*s == '.' && *(s+1) == '.'){
			s += 2;
		}
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(strchr("[]()~?:,", *s) != NULL){
		s++;
		token->class = TOK_OPERATOR;
		token->end = s;
		return 0;
	} else if(strchr("{};", *s) != NULL){
		s++;
		token->class = TOK_SEPARATOR;
		token->end = s;
		return 0;
	}

	return -1;
}

void print_token(struct token *token){
	char *p;

	printf("Token: _");
	for(p = token->start; p < token->end; p++){
		putchar(*p);
	}
	printf("_\n");
	printf("\tCLASS: %s\n", token_class_str[token->class]);
	if(token->class == TOK_CONSTANT){
		printf("\tconst_type: %s\n", token_const_types[token->const_type]);
	}
	printf("\n");
}

int test_token(char *s, char *t){
	int len;
	struct token token;

	if(next_token(s, &token)){
		printf("Failed to tokenize: _%s_\n", s);
		return -1;
	}
	len = token.end - token.start;
	if(len != strlen(t) || strncmp(token.start, t, len)){
		printf("Token is unexpected: ");
		putchar('_');
		fwrite(token.start, 1, len, stdout);
		putchar('_');
		printf(" != _%s_\n", t);
	}
	return 0;
}

int main(int argc, char *argv[]){
	// Identifier tests
	test_token(" _int ", "_int");
	test_token(" token ", "token");
	test_token(" main ", "main");
	test_token(" argc ", "argc");
	test_token(" argv ", "argv");
	test_token(" x0 ", "x0");
	test_token(" long_function_name ", "long_function_name");
	test_token(" int_ ", "int_");
	test_token(" I32 ", "I32");

	// Keyword tests
	test_token(" int ", "int");
	test_token(" void ", "void");
	test_token(" void foo ", "void");
	test_token(" return ", "return");
	test_token(" return(", "return");
	test_token(" case ", "case");

	// Integer Constant tests
	test_token(" 0 ", "0");
	test_token(" 1 ", "1");
	test_token(" 15 ", "15");
	test_token(" 1234567890 ", "1234567890");
	test_token(" 04 ", "04");
	test_token(" 037 ", "037");
	test_token(" 0755 ", "0755");
	test_token(" 0x0 ", "0x0");
	test_token(" 0x1 ", "0x1");
	test_token(" 0xA ", "0xA");
	test_token(" 0xAABBCCDD ", "0xAABBCCDD");
	test_token(" 15L ", "15L");
	test_token(" 15l ", "15l");
	test_token(" 15U ", "15U");
	test_token(" 15u ", "15u");
	test_token(" 15UL ", "15UL");
	test_token(" 15uL ", "15uL");
	test_token(" 15Ul ", "15Ul");
	test_token(" 0x5544AAFFUL ", "0x5544AAFFUL");
	test_token(" 0755U ", "0755U");

	// Character Constant tests
	test_token(" 'a' ", "'a'");
	test_token(" 'A' ", "'A'");
	test_token(" '$' ", "'$'");
	test_token(" '\\n' ", "'\\n'");
	test_token(" '\\t' ", "'\\t'");
	test_token(" '\\x55' ", "'\\x55'");
	test_token(" '\\xAB' ", "'\\xAB'");
	test_token(" '\\0' ", "'\\0'");
	test_token(" '\\04' ", "'\\04'");
	test_token(" '\\755' ", "'\\755'");

	// Float Constant tests
	test_token(" 1.0 ", "1.0");
	test_token(" 14.0 ", "14.0");
	test_token(" 3.14159 ", "3.14159");
	test_token(" 1.0f ", "1.0f");
	test_token(" 1.0e4 ", "1.0e4");
	test_token(" 1.0e-4 ", "1.0e-4");
	test_token(" 1.0e-14 ", "1.0e-14");
	test_token(" 1.0e+4 ", "1.0e+4");
	test_token(" 1.0e+14 ", "1.0e+14");
	test_token(" 1.0e-4f ", "1.0e-4f");
	test_token(" 1.0e-4F ", "1.0e-4F");
	test_token(" 1.0e-4l ", "1.0e-4l");
	test_token(" 1.0e-4L ", "1.0e-4L");

	// String tests
	test_token(" \"abcd\" ", "\"abcd\"");
	test_token(" \"asdf	asdf\" ", "\"asdf	asdf\"");
	test_token(" \"asdf\\nasdf\" ", "\"asdf\\nasdf\"");
	test_token(" \"asdf\\x11asdf\" ", "\"asdf\\x11asdf\"");
	test_token(" \"asdf\\0asdf\" ", "\"asdf\\0asdf\"");

	// Operator tests
	test_token(" ++ ", "++");
	test_token(" += ", "+=");
	test_token(" + ", "+");
	test_token(" -- ", "--");
	test_token(" -= ", "-=");
	test_token(" -> ", "->");
	test_token(" - ", "-");
	test_token(" *= ", "*=");
	test_token(" * ", "*");
	test_token(" /= ", "/=");
	test_token(" / ", "/");
	test_token(" %= ", "%=");
	test_token(" % ", "%");
	test_token(" != ", "!=");
	test_token(" ! ", "!");
	test_token(" && ", "&&");
	test_token(" &= ", "&=");
	test_token(" & ", "&");
	test_token(" || ", "||");
	test_token(" |= ", "|=");
	test_token(" | ", "|");
	test_token(" <<= ", "<<=");
	test_token(" << ", "<<");
	test_token(" <= ", "<=");
	test_token(" < ", "<");
	test_token(" >>= ", ">>=");
	test_token(" >> ", ">>");
	test_token(" >= ", ">=");
	test_token(" > ", ">");
	test_token(" ^= ", "^=");
	test_token(" ^ ", "^");
	test_token(" == ", "==");
	test_token(" = ", "=");
	test_token(" [ ", "[");
	test_token(" ] ", "]");
	test_token(" ( ", "(");
	test_token(" ) ", ")");
	test_token(" ... ", "...");
	test_token(" . ", ".");
	test_token(" ~ ", "~");
	test_token(" ? ", "?");
	test_token(" : ", ":");
	test_token(" , ", ",");

	// Separator tests
	test_token(" { ", "{");
	test_token(" } ", "}");
	test_token(" ; ", ";");

	return 0;
}
