#ifndef LL1_PARSER_H
#define LL1_PARSER_H

#include <iostream>
#include <stack>
#include <string>
#include "LL1_Grammar.h"

class Parser {
private:
	Grammar G;

	// ����ջ
	vector<string> Symbol;
	// ����
	vector<pair<string, string>> input;

	void Lex_Input();
	void Analysis();
	void print(string generative);

public:
	Parser(string grammar_file);
	Parser() {};
};

#endif