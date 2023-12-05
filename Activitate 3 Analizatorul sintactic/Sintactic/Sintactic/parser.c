#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include "lexer.h"
#include "parser.h"
#include "utils.h"

int iTk;
Token* consumed;	

_Noreturn void tkerr(const char* fmt, ...) {
	fprintf(stderr, "error in line %d: ", tokens[iTk].line);
	va_list va;
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

bool consume(int code) {
	if (tokens[iTk].code == code) {
		consumed = &tokens[iTk++];
		return true;
	}
	return false;
}

bool program() {
	for (;;) {
		if (defVar()) {}
		else if (defFunc()) {}
		else if (block()) {}
		else break;
	}
	if (consume(FINISH)) {
		return true;
	}
	else tkerr("syntax error");
	return false;
}

void parse() {
	iTk = 0;
	program();
}

bool defVar() {

    if (consume(TYPE_INT)) {
        if (consume(ID)) {

            return true; 
        }
    }
    return false; 
}

bool defFunc() {

    if (consume(FUNCTION)) {
        if (consume(ID)) {
            if (params()) {
                if (block()) {
                    return true;
                }
            }
        }
    }
    return false; 
}

bool params() {
    if (consume(LPAR)) {
        if (paramList()) {
            if (consume(RPAR)) {
                if (consume(COLON)) {
                    if (consume(TYPE_INT) || consume(TYPE_REAL) || consume(TYPE_STR)) {
                        return true;
                    }
                    else {
                        tkerr("\nExpected a TYPE_ !");
                    }
                }
                else {
                    tkerr("\nExpected ':' !");
                }
            }
            else {
                tkerr("\nExpected ')' !");
            }
        }
    }
    else {
        tkerr("\nExpected '(' !");
    }
    return false;
}

bool paramList() {
    if (param()) {  
        while (consume(COMMA)) {  
            if (!param()) {
                return false;  
            }
        }
        return true;  
    }
    return true;  
}

bool param() {
    if (consume(ID)) {
        if (consume(COLON)) {
            if (consume(TYPE_INT) || consume(TYPE_REAL) || consume(TYPE_STR)) {
                return true;  
            }
        }
    }
    return false;  
}


bool block() {

    if (consume(LCPAR)) {
        while (stmt()) {
            if (!consume(SEMICOLON)) {
                break;
            }
        }
        if (consume(RCPAR)) {
            return true; 
        }
    }
    return false; 
}

bool stmt() {
    if (consume(ID)) {
        if (consume(ASSIGN)) {
            if (expr()) {
                return true; 
            }
        }
    }
    return false; 
}

bool expr() {
    
    return true; 
}


