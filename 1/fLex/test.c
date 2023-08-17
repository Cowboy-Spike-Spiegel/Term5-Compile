12345.E12 1234 413 4134 1 434 4 0.345E 0.345E+ 0.345E+123

/* 正宗大飞朱 */

#include "Lex.h" \
123
#include "Lex.h

'a'
'
'a
'\\'
Lex::Lex(string name)
{
	this->file_name = name;
	this->file_input.open(name);
	if (file_input.is_open() == false)
	{
		cout << "wrong 
		open" << endl << "test";
		cout << "wrong\
		open" << endl << "test";
		return;
	}
	buffer.clear();   //清空缓存区

	this->char_count = 0;
	this->row_count = 0;
	this->col_count = 0;
	this->word_count=0;

	this->char_const_count = 0;
	this->error_count = 0;
	this->identifier_count = 0;
	this->punctuator_count = 0;
	this->noting_count = 0;
	this->string_count = 0;
	this->keyword_count = 0;
	this->num_count = 0;
	this->pre_count = 0;

	this->is_hex = false;

	InitKeyWord();
}
