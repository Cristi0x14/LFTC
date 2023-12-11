#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "utils.h"

int main() {

	char* input = loadFile("1.q");
	tokenize(input);
	showTokens();
	//showToken(53);
	parse();
	free(input);
	
	return 0;
	
}