#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexer.h"
#include "utils.h"

Token tokens[MAX_TOKENS];
int nTokens;

int line = 1;		// the current line in the input file

// adds a token to the end of the tokens list and returns it
// sets its code and line
Token* addTk(int code) {
	if (nTokens == MAX_TOKENS)err("too many tokens");
	Token* tk = &tokens[nTokens];
	tk->code = code;
	tk->line = line;
	nTokens++;
	return tk;
}

// copy in the dst buffer the string between [begin,end)
char* copyn(char* dst, const char* begin, const char* end) {
	char* p = dst;
	if (end - begin > MAX_STR)err("string too long");
	while (begin != end)*p++ = *begin++;
	*p = '\0';
	return dst;
}

void tokenize(const char* pch) {
	const char* start;
	Token* tk;
	char buf[MAX_STR + 1];
	for (;;) {
		switch (*pch) {
		case ' ':
			pch++;
			break;
		case '\t':
			pch++;
			break;
		case '\r':
			if (pch[1] == '\n')
				pch++;
			break;
		case '\n':
			line++;
			pch++;
			break;
		case '\0':
			addTk(FINISH);
			return;
		case ',':
			addTk(COMMA);
			pch++;
			break;
		case '=':
			if (pch[1] == '=') {
				addTk(EQUAL);
				pch += 2;
			}
			else {
				addTk(ASSIGN);
				pch++;
			}
			break;
		case '(':
			addTk(LPAR);
			pch++;
			break;
		case ':':
			addTk(COLON);
			pch++;
			break;
		case ')':
			addTk(RPAR);
			pch++;
			break;
		case '<':
			addTk(LESS);
			pch++;
			break;
		case '>':
			if (pch[1] == '=') {
				addTk(GREATEREQ);
				pch++;
			}
			else
			addTk(GREATER);
			pch++;
			break;
		case ';':
			addTk(SEMICOLON);
			pch++;
			break;
		case '+':
			addTk(ADD);
			pch++;
			break;
		case '.':
			pch++;
			break;
		case '"':
			pch++;
			start = pch;
			while (*pch && *pch != '"') {
				pch++;
			}
			if (*pch == '"') {
				tk = addTk(STRING_VALUE);
				copyn(tk->text, start, pch);
				pch++;
			}
			else {
				err("Unterminated string");
			}
			break;
		case '#':
			while (*pch != '\0' && *pch != '\n' && *pch != '\r') {
				pch++;
			}
			break;
		default:
			if (isalpha(*pch) || *pch == '_') {
				for (start = pch++; isalnum(*pch) || *pch == '_'; pch++) {}
				char* text = copyn(buf, start, pch);
				if (strcmp(text, "int") == 0)addTk(TYPE_INT);
				else if (strcmp(text, "real") == 0)addTk(TYPE_REAL);
				else if (strcmp(text, "string") == 0)addTk(TYPE_STR);
				else if (strcmp(text, "end") == 0)addTk(END);
				else if (strcmp(text, "if") == 0)addTk(IF);
				else if (strcmp(text, "else") == 0)addTk(ELSE);
				else if (strcmp(text, "function") == 0)addTk(FUNCTION);
				else if (strcmp(text, "while") == 0)addTk(WHILE);
				else if (strcmp(text, "var") == 0)addTk(VAR);
				else if (strcmp(text, "return") == 0)addTk(RETURN);
				else {
					tk = addTk(ID);
					strcpy(tk->text, text);
				}
			}
			else if (isdigit(*pch)) {
				char* end;
				int intVal = strtol(pch, &end, 10);
				if (end > pch) {
					if (*(pch + 1) == '.')
					{	
						end++;
						char str[20];
						char* endptr;
						double realVal = 0;
						
						int PastDot = strtol(pch + 2, &end, 10);
						sprintf(str, "%d.%d", intVal, PastDot);
						realVal = strtod(str, &endptr);
						tk = addTk(REAL_VALUE);

						tk->r = realVal;
						pch = end;

					}
					else
					{
						tk = addTk(INT_VALUE);
						tk->i = intVal;
						pch = end;
					}
				}
				else {
					err("invalid number: %s", pch);
				}
			}
			else err("invalid char: %c (%d)", *pch, *pch);
			break;
		}
	}
}

void showTokens() {
	for (int i = 0; i < nTokens; i++) {
		Token* tk = &tokens[i];
		printf("%d ", tk->line);
		switch (tk->code) {
		case ID: printf("ID: %s\n", tk->text); break;
		case TYPE_INT: printf("TYPE_INT\n"); break;
		case TYPE_REAL: printf("TYPE_REAL\n"); break;
		case TYPE_STR: printf("TYPE_STR\n"); break;
		case COMMA: printf("COMMA\n"); break;
		case COLON: printf("COLON\n"); break;
		case FINISH: printf("FINISH\n"); break;
		case ASSIGN: printf("ASSIGN\n"); break;
		case EQUAL: printf("EQUAL\n"); break;
		case LPAR: printf("(\n"); break;
		case RPAR: printf(")\n"); break;
		case LESS: printf("LESS\n"); break;
		case GREATER: printf("GREATER\n"); break;
		case GREATEREQ: printf("GREATEREQ\n"); break;
		case SEMICOLON: printf("SEMICOLON\n"); break;
		case INT_VALUE: printf("INT:%d\n", tk->i); break;
		case REAL_VALUE: printf("REAL:%g\n", tk->r); break;
		case STRING_VALUE: printf("STR:%s\n", tk->text); break;
		case ADD: printf("ADD\n"); break;
		case IF: printf("IF\n"); break;
		case ELSE: printf("ELSE\n"); break;
		case FUNCTION: printf("FUNCTION\n"); break;
		case END: printf("END\n"); break;
		case WHILE: printf("WHILE\n"); break;
		case VAR: printf("VAR\n"); break;
		case RETURN: printf("RETURN\n"); break;
		}
	}
}

void showToken(int i) {
	Token* tk = &tokens[i];
	printf("%d ", tk->line);
	switch (tk->code) {
	case ID: printf("ID: %s\n", tk->text); break;
	case TYPE_INT: printf("TYPE_INT\n"); break;
	case TYPE_REAL: printf("TYPE_REAL\n"); break;
	case TYPE_STR: printf("TYPE_STR\n"); break;
	case COMMA: printf("COMMA\n"); break;
	case COLON: printf("COLON\n"); break;
	case FINISH: printf("FINISH\n"); break;
	case ASSIGN: printf("ASSIGN\n"); break;
	case EQUAL: printf("EQUAL\n"); break;
	case LPAR: printf("(\n"); break;
	case RPAR: printf(")\n"); break;
	case LESS: printf("LESS\n"); break;
	case GREATER: printf("GREATER\n"); break;
	case GREATEREQ: printf("GREATEREQ\n"); break;
	case SEMICOLON: printf("SEMICOLON\n"); break;
	case INT_VALUE: printf("INT:%d\n", tk->i); break;
	case REAL_VALUE: printf("REAL:%g\n", tk->r); break;
	case STRING_VALUE: printf("STR:%s\n", tk->text); break;
	case ADD: printf("ADD\n"); break;
	case IF: printf("IF\n"); break;
	case ELSE: printf("ELSE\n"); break;
	case FUNCTION: printf("FUNCTION\n"); break;
	case END: printf("END\n"); break;
	case WHILE: printf("WHILE\n"); break;
	case VAR: printf("VAR\n"); break;
	case RETURN: printf("RETURN\n"); break;
	}
}
