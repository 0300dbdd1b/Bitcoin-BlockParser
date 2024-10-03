#ifndef PARSER_H
 #define PARSER_H
 #include "structs.h"


typedef struct ParserOptions
{
	int flag;
} ParserOptions;

void Parser(const char *datadir, ParserOptions *opts);

#endif // !PARSER_H
