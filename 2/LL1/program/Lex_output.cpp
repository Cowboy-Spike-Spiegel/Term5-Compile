#include "Lex_Output.h"
#include <vector>
#include <string>

extern vector<pair<string, string>> lex_outcome;

Output::Output(string outFile_name) {
	fout.open(outFile_name);
	if (fout.is_open() == false) {
		cout << "wrong open outFile" << endl;
		exit(1);
	}
}


Output::~Output() {
	fout.close();
}


// ���һ�������Ϣ
void Output::add(dataType type, string token, int row, int col)
{
	string typeStr;
	switch(type)
	{
	case dataType::NUMBER: {
		typeStr = "NUMBER";
		break;
	}
	case dataType::CHAR: {
		typeStr = "CHAR";
		break;
	}
	case dataType::STRING: {
		typeStr = "STRING";
		break;
	}
	case dataType::IDENTIFIER: {
		typeStr = "IDENTIFIER";
		break;
	}
	case dataType::KEYWORD: {
		typeStr = "KEYWORD";
		break;
	}
	case dataType::PUNCTUATOR: {
		typeStr = "PUNCTUATOR";
		break;
	}
	case dataType::PRECOMPLIED: {
		typeStr = "PRECOMPLIED";
		break;
	}
	case dataType::NOTE: {
		typeStr = "NOTE";
		break;
	}
	case dataType::ERROR: {
		typeStr = "ERROR";
		cout << "wrong type for outFile";
		break;
	}
	}
	// �������������﷨������Ҫ�޸����
	fout << token + "\n" + typeStr + "\n";
	lex_outcome.push_back(make_pair(typeStr, token));
}

// ���ͳ����Ϣ
void Output::sumary(string str) {
	// fout << str;
	// ��������������
}