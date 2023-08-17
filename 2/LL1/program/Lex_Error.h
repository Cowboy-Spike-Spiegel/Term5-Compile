#ifndef LEX_ERROR_H
#define LEX_ERROR_H

#include <iostream>
#include <fstream>
#include <string>

#include "global.h"


using namespace std;

class Error
{
private:
	ofstream fout;

public:
	Error(string errorFile_name);
	~Error();

	void add(dataType type, string token, string description, int row, int col);
};

#endif

