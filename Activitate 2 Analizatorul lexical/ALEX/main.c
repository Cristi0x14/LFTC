#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "utils.h"

int main() {
    char* input = loadFile("1.q");
    tokenize(input);
    showTokens();
    free(input);
    return 0;
}