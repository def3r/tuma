#ifndef TOML_H_
#define TOML_H_

#include "types.h"
#include <stdio.h>

char *parseMacInfo(FILE *f, turingMachine **tm);
char *parseTransition(FILE *f, turingMachine **tm);
void parseLine(FILE *f, char *line, size_t len, turingMachine **tm);
void parseTOML(const char *fname, turingMachine **tm);

#endif // !TOML_H_
