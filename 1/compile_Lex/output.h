#ifndef OUTPUT_H
#define OUTPUT_H

#include <iostream>
#include <fstream>
#include <string>

#include "global.h"


using namespace std;

class Output
{
private:
	ofstream fout;

public:
	Output(string outFile_name);
	~Output();

	void add(dataType type, string token, int row, int col);
	void sumary(string str);
};

#endif