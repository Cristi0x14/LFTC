#pragma once
#include <stdlib.h>
#include <stdbool.h>
// parse the extracted tokens
void parse();
bool defVar();
bool defFunc();
bool block();
bool params();
bool paramList();
bool param();
bool stmt();
bool expr();

