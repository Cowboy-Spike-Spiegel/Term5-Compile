#ifndef LEX_H
#define LEX_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <set>

// 引入全局定义模块，输出词法分析模块，输出错误模块
#include "global.h"
#include "Lex_Output.h"
#include "Lex_Error.h"


using namespace std;

class Lex
{
private:
	// 输入输出文件||类
	string inFile_name;
	string outFile_name;
	string errorFile_name;
	ifstream inFile;
	Output* outStream;
	Error* errorStream;

	// 缓冲区
	string buffer;

	// 字符属性
	char ch;				// 存储当前读取的字符
	int character_count = 0;// 字符总数

	// 当前词法属性
	int row_count;			// 行数
	int col_count;			// 列数
	dataType type;			// 当前属性

	// 各类词语计数
	int word_count = 0;
	int number_count = 0;
	int char_count = 0;
	int string_count = 0;
	int identifier_count = 0;
	int keyword_count = 0;
	int punctuator_count = 0;
	int precompile_count = 0;
	int note_count = 0;
	int error_count = 0;

	set<string> keyword; //关键词表

public:
	Lex(string inFile_name, string outFile_name, string errorFile_name);
	~Lex();
	
	char getChar();	// 读取单个字符
	
	//进行词法分析操作
	void parser();

	void parser_number(int start_row, int start_col);
	void parser_char(int start_row, int start_col);
	void parser_string(int start_row, int start_col);
	void parser_identifier(int start_row, int start_col);	// 包含keyword
	void parser_punctuator(int start_row, int start_col);
	void parser_precompiled(int start_row, int start_col);
	void parser_note(int start_row, int start_col);

	// 输出信息到outFile输出文件
	void print_outFile(int start_row, int start_col);
	// 输出信息到errorFile输出文件
	void print_errorFile(string description, int start_row, int start_col);

	
	// 输出统计信息到outFile输出文件
	void print_info();
	
};

#endif

