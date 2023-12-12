#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utils.h"
#include "lexer.h"
#include "parser.h"

int iTk;
int start;
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

bool baseType() {
    if (consume(TYPE_INT)) {
        return true;
    }
    else if (consume(TYPE_REAL)) {
        return true;
    }
    else if (consume(TYPE_STR)) {
        return true;
    }
    tkerr("No base type found !");
    return false;
}

bool defVar() {
    start = iTk;
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
    iTk = start;
    return false;
}

bool funcParam() {
    start = iTk;
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
    iTk = start;
    return false;
}

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
    return true;
}


bool defFunc() {
    start = iTk;
    if (consume(FUNCTION)) {
        if (consume(ID)) {
            if (consume(LPAR)) {
                if (funcParams() && consume(RPAR)) {
                    if (consume(COLON)) {
                        if (baseType()) {
                            while (defVar()) {}
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
    iTk = start;
    return false;
}

bool block() {
    start = iTk;
    if (instr()) {
        return true;
    }
    iTk = start;
    return false;
}


bool instr() {
    start = iTk;
    if (consume(IF)) {
        if (consume(LPAR)) {
            if (expr()) {
                if (consume(RPAR)) {
                    if (block()) {
                        if (consume(ELSE)) {
                            if (!block()) {
                                tkerr("Invalid ELSE block");
                            }
                        }
                        if (consume(END)) {
                            return true;
                        }
                        else {
                            tkerr("Expected END of IF");
                        }
                    }
                    else
                    {   
                        tkerr("Invlaid IF block");
                    }
                }
                else
                {
                    tkerr("Expected RPAR at end of IF");
                }
            }
            else {
                tkerr("Expected Expression inside IF");
            }
        }
        else
        {
            tkerr("Expected LPAR after IF !");
        }
    }
    else if (consume(RETURN)) {
        if (expr()) {
            if (consume(SEMICOLON)) {
                return true;
            }
            tkerr("Expected SEMICOLON");
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
                    tkerr("Invalid WHILE block");
                }
                else
                {
                    tkerr("Expected RPAR");
                }
            }
            else {
                tkerr("Invalid While Expression");
            }
        }
        else {
            tkerr("Expected LPAR after WHILE");
        }
    }
    else if (consume(SEMICOLON)) {
        return true;
    }
    else if (expr()) {
        if (consume(SEMICOLON)) {
            if (block()) {

            }
            return true;
        }
        else {
            tkerr("Expected SEMICOLON");
        }
    }
    iTk = start;
    return false;
}

bool expr() {
    start = iTk;
    if (exprLogic()) {
        return true;
    }
    iTk = start;
    return false;
}

bool exprLogic() {
    start = iTk;
    if (exprAssign()) {
        while ((consume(AND) || consume(OR)) ) {
            exprAssign();
        }

        return true;
    }
    iTk = start;
    return false;
}

bool exprAssign() {
    start = iTk;
    if (consume(ID)) {
        if (consume(ASSIGN)) {
            if (consume(INT_VALUE)) {
                return true;
            }
            else if (consume(REAL_VALUE)) {
                return true;
            }
            else if (consume(STRING_VALUE)) {
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
    iTk = start;
    return false;
}

bool exprComp() {
    start = iTk;
    if (exprAdd()) {
        return true;
    }
    if (consume(LESS) || consume(EQUAL)) {
        if (exprAdd()) {
            return true;
        }
    }
    iTk = start;
    return false;
}

bool exprAdd() {
    start = iTk;
    if (exprMul()) {
        while (consume(ADD) || consume(SUB) && exprMul()) {}
        return true;
    }
    iTk = start;
    return false;
}

bool exprMul() {
    start = iTk;
    if (exprPrefix()) {
        while (consume(MUL) || consume(DIV) && exprPrefix()) {}
        return true;
    }
    iTk = start;
    return false;
}

bool exprPrefix() {
    start = iTk;
    if (consume(SUB) || consume(NOT)) {
        if (factor()) {
            return true;
        }
    }
    if (factor()) {
        return true;
    }
    iTk = start;
    return false;
}

bool factor() {
    start = iTk;
    if (tokens[iTk].code == COMMA) {
        iTk--;
        return factor();
    }
    else if ((consume(INT_VALUE) || consume(REAL_VALUE) || consume(STRING_VALUE))) {
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
    iTk = start;
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

void parse() {
    iTk = 0;
    program();
}