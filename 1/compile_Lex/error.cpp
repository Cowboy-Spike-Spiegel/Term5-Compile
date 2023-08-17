#include "Error.h"

Error::Error(string errorFile_name) {
	fout.open(errorFile_name);
	if (fout.is_open() == false) {
		cout << "wrong open errorFile" << endl;
		exit(1);
	}
}

Error::~Error() {
	fout.close();
}

void Error::add(dataType type, string token, string description, int row, int col)
{
	string typeStr;
	switch (type)
	{
	case dataType::NUMBER: {
		typeStr = "NUMBER";
		cout << "wrong type for outFile";
		break;
	}
	case dataType::CHAR: {
		typeStr = "CHAR";
		cout << "wrong type for outFile";
		break;
	}
	case dataType::STRING: {
		typeStr = "STRING";
		cout << "wrong type for outFile";
		break;
	}
	case dataType::IDENTIFIER: {
		typeStr = "IDENTIFIER";
		cout << "wrong type for outFile";
		break;
	}
	case dataType::KEYWORD: {
		typeStr = "KEYWORD";
		cout << "wrong type for outFile";
		break;
	}
	case dataType::PUNCTUATOR: {
		typeStr = "PUNCTUATOR";
		cout << "wrong type for outFile";
		break;
	}
	case dataType::PRECOMPLIED: {
		typeStr = "PRECOMPLIED";
		cout << "wrong type for outFile";
		break;
	}
	case dataType::NOTE: {
		typeStr = "NOTE";
		cout << "wrong type for outFile";
		break;
	}
	case dataType::ERROR: {
		typeStr = "ERROR";
		break;
	}
	}
	fout << "{\n"
		<< "\tcontent  :\t" + token + "\n"
		<< "\tdataType :\t" + typeStr + "\n"
		<< "\tposition :\t(" + to_string(row) + ", " + to_string(col) + ")\n"
		<< "\tdescription :\t" + description + "\n"
		<< "}\n";
}