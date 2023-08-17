#ifndef LEX_H
#define LEX_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <set>

// ����ȫ�ֶ���ģ�飬����ʷ�����ģ�飬�������ģ��
#include "global.h"
#include "Lex_Output.h"
#include "Lex_Error.h"


using namespace std;

class Lex
{
private:
	// ��������ļ�||��
	string inFile_name;
	string outFile_name;
	string errorFile_name;
	ifstream inFile;
	Output* outStream;
	Error* errorStream;

	// ������
	string buffer;

	// �ַ�����
	char ch;				// �洢��ǰ��ȡ���ַ�
	int character_count = 0;// �ַ�����

	// ��ǰ�ʷ�����
	int row_count;			// ����
	int col_count;			// ����
	dataType type;			// ��ǰ����

	// ����������
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

	set<string> keyword; //�ؼ��ʱ�

public:
	Lex(string inFile_name, string outFile_name, string errorFile_name);
	~Lex();
	
	char getChar();	// ��ȡ�����ַ�
	
	//���дʷ���������
	void parser();

	void parser_number(int start_row, int start_col);
	void parser_char(int start_row, int start_col);
	void parser_string(int start_row, int start_col);
	void parser_identifier(int start_row, int start_col);	// ����keyword
	void parser_punctuator(int start_row, int start_col);
	void parser_precompiled(int start_row, int start_col);
	void parser_note(int start_row, int start_col);

	// �����Ϣ��outFile����ļ�
	void print_outFile(int start_row, int start_col);
	// �����Ϣ��errorFile����ļ�
	void print_errorFile(string description, int start_row, int start_col);

	
	// ���ͳ����Ϣ��outFile����ļ�
	void print_info();
	
};

#endif

