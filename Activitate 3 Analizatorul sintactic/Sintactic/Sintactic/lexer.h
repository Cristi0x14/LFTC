#pragma once

enum {
	ID, // Identificatori
	VAR, FUNCTION, IF, ELSE, WHILE, END, RETURN,
	TYPE_INT, TYPE_REAL, TYPE_STR, // Cuvinte cheie
	INT_CONST, REAL_CONST, STR_CONST, // Constante
	COMMA, COLON, SEMICOLON, LPAR, RPAR, FINISH, // Delimitatori
	ADD, SUB, MUL, DIV, AND, OR, NOT, // Operatori
	ASSIGN, EQUAL, NOTEQ, LESS, GREATER, GREATEREQ, 
	SPACE, COMMENT ,
	INT_VALUE,REAL_VALUE,STRING_VALUE
};

#define MAX_STR		127

typedef struct {
	int code;		// ID, TYPE_INT, ...
	int line;		// the line from the input file
	union {
		char text[MAX_STR + 1];		// the chars for ID, STR
		int i;		// the value for INT
		double r;		// the value for REAL
	};
}Token;

#define MAX_TOKENS		4096
extern Token tokens[];
extern int nTokens;

void tokenize(const char* pch);
void showTokens();
void showToken(int i);
