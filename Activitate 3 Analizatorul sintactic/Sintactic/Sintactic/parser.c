#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utils.h"
#include "lexer.h"
#include "parser.h"

int iTk;
Token* consumed;

// Function to report errors
_Noreturn void tkerr(const char* fmt, ...) {
    fprintf(stderr, "error in line %d: ", tokens[iTk].line);
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

// Function to consume tokens
bool consume(int code) {
    if (tokens[iTk].code == code) {
        consumed = &tokens[iTk++];
        return true;
    }
    return false;
}

// Function to parse base types
bool baseType() {
    if (consume(TYPE_INT)) {
        return true;
    }
    if (consume(TYPE_REAL)) {
        return true;
    }
    if (consume(TYPE_STR)) {
        return true;
    }
}

// Function to parse variable declarations
bool defVar() {
    if (consume(VAR)) {
        if (consume(ID)) {
            if (consume(COLON)) {
                if (baseType()) {
                    if (consume(SEMICOLON)) {
                        return true;
                    }
                    else {
                        tkerr("Expected SEMICOLON !");
                    }
                }
                else {
                    tkerr("Expected baseType !");
                }
            }
            else {
                tkerr("Expected COLON !");
            }
        }
        else {
            tkerr("Expected ID !");
        }
    }
    return false;
}

// Function to parse function parameters
bool funcParam() {
    if (consume(ID)) {
        if (consume(COLON)) {
            if (baseType()) {
                return true;
            }
            else {
                tkerr("Expected baseType after COLON in function parameter");
            }
        }
        else {
            tkerr("Expected COLON after ID in function parameter");
        }
    }
    return false;
}

// Function to parse function parameter list
bool funcParams() {
    if (funcParam()) {
        while (consume(COMMA)) {
            if (!funcParam()) {
                tkerr("Invalid function parameter");
                return false;
            }
        }
        return true;
    }
    return true; // funcParams is optional
}


// Function to parse function definitions
bool defFunc() {
    if (consume(FUNCTION)) {
        if (consume(ID)) {
            if (consume(LPAR)) {
                if (funcParams() && consume(RPAR)) {
                    if (consume(COLON)) {
                        if (baseType()) {
                            while (defVar()) {} // Handle optional defVar*
                            if (block()) {
                                if (consume(END)) {
                                    return true;
                                }
                                else {
                                    tkerr("Expected END after function block");
                                }
                            }
                            else {
                                tkerr("Invalid function block");
                            }
                        }
                        else {
                            tkerr("Expected baseType after COLON in function definition");
                        }
                    }
                    else {
                        tkerr("Expected COLON after function parameters");
                    }
                }
                else {
                    tkerr("Invalid function parameters");
                }
            }
            else {
                tkerr("Expected '(' after function ID");
            }
        }
        else {
            tkerr("Expected ID after FUNCTION");
        }
    }
    return false;
}

bool block() {
    if (instr()) {
        return true;
    }
    return false;
}


bool instr() {
    if (consume(IF)) {
        if (consume(LPAR)) {
            if (expr()) {
                if (consume(RPAR)) {
                    if (block()) {
                        if (consume(ELSE)) {
                            block();
                        }
                        if (consume(END)) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    else if (expr()) {
        if (consume(SEMICOLON)) {
            if (block()) {

            }
            return true;
        }
    }

    else if (consume(RETURN)) {
        if (expr()) {
            if (consume(SEMICOLON)) {
                return true;
            }
        }
    }
    else if (consume(WHILE)) {
        if (consume(LPAR)) {
            if (expr()) {
                if (consume(RPAR)) {
                    if (block()) {
                        if (consume(END)) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    // Other instruction types handling
    return false;
}

bool expr() {
    if (exprLogic()) {
        return true;
    }
    return false;
}


bool exprLogic() {
    if (exprAssign()) {
        while ((consume(AND) || consume(OR)) && exprAssign()) {

        }

        return true;
    }
    return false;
}
bool exprAssign() {
    if (consume(ID)) {
        if (consume(ASSIGN)) {
            if (consume(INT_VALUE)) {
                return true;
            }
            if (!exprComp()) {
                tkerr("\nExpersion Compare Error");
                return false;
            }

        }
        else if (consume(SEMICOLON)) {
            iTk--;
            return true;
        }
    }
    if (exprComp()) {
        return true;
    }
    return false;
}

bool exprComp() {
    if (exprAdd()) {
        return true;
    }
    if (consume(LESS) || consume(EQUAL)) {
        if (exprAdd()) {
            return true;
        }
    }
    return false;
}

bool exprAdd() {
    if (exprMul()) {
        while (consume(ADD) || consume(SUB) && exprMul()) {}
        return true;
    }
    return false;
}

bool exprMul() {
    if (exprPrefix()) {
        while (consume(MUL) || consume(DIV) && exprPrefix()) {}
        return true;
    }
    return false;
}

bool exprPrefix() {
    if (consume(SUB) || consume(NOT)) {
        if (factor()) {
            return true;
        }
    }
    if (factor()) {
        return true;
    }
    return false;
}

bool factor() {
    if (tokens[iTk].code == COMMA) {
        iTk--;
        return factor();
    }
    else
        if ((consume(INT_VALUE) || consume(REAL_VALUE) || consume(STRING_VALUE))) {
            return true;
        }
        else if ((consume(LPAR) && expr() && consume(RPAR))) {
            return true;
        }
        else if (consume(ID)) {
            if (consume(LPAR)) {
                if (expr()) {

                    while (consume(COMMA) && expr()) {}
                }
                if (consume(RPAR)) {

                    return true;
                }
                else {
                    tkerr("\nMissing RPAR");
                    return false;
                }
            }
            else if ((tokens[iTk - 2].code == LPAR)) {
                if ((tokens[iTk].code == COMMA)) {
                    while (consume(COMMA) && expr()) {}
                    return true;
                }
            }
            return true;
        }
    return false;
}







// Main parsing routine
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
    else {
        //help();
        tkerr("syntax error \nlast  iTK : %d", iTk);
    }
    return false;
}
void help() {
    printf("\n");
    showToken(iTk);
    printf(" %d\n", iTk);
    showToken(iTk + 1);
    printf(" %d\n", iTk + 1);
    showToken(iTk + 2);
    printf(" %d\n", iTk + 2);
}
// Function to start parsing
void parse() {
    iTk = 0;
    program();
}