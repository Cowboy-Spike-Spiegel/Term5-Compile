#include "Lex.h"


// ��ʼ������
Lex::Lex(string inFile_name, string outFile_name, string errorFile_name)
{
	// ���������ļ���
	this->inFile_name = inFile_name;
	this->outFile_name = outFile_name;
	this->errorFile_name = errorFile_name;
	inFile.open(inFile_name);
	if (inFile.is_open() == false) {
		cout << "wrong open" << endl;
		exit(1);
	}
	outStream = new Output(outFile_name);
	errorStream = new Error(errorFile_name);

	// ��ʼ������
	ch = ' ';	// ��ʼ��
	row_count = 1;
	col_count = 1;

	// ��ʼ���ؼ��֣����ؼ�������Ԫ������ŵ�set��
	vector<string> keywordtable = { "int","long","short","float","double", "char" ,"unsigned", "signed" ,
		"const" ,"void" ,"volatile", "enum","struct" ,"union" ,
		"if", "else", "goto", "switch" ,"case" ,"do", "while", "for", "continue","break",
		"return", "default","typedef", "auto" ,"register", "extern", "static", "sizeof" };
	for (auto s : keywordtable)
		keyword.insert(s);
}

// ɾ��new�������
Lex::~Lex() {
	delete outStream;
	delete errorStream;
}



// ��ȡ�����ַ�
char Lex::getChar()
{
	ch = inFile.get();
	
	//�����ݴ��뻺����
	buffer.push_back(ch);
	character_count++;
	col_count++;

	// ���г�ʼ����ֵ
	if (ch == '\n') {
		row_count++;
		col_count = 1;	// ��⣺��һ�л��з�ռ���˴��е�0�±�ռ�
	}

	return ch;
}



// ��ȡ���ĺ���----------------------------------------------------------------------
void Lex::parser()
{
	// ѭ����ȡ����-----------------------------------------
	int start_row, start_col;
	while (!inFile.eof())
	{
		// �ʷ������ⲿ��������������ųԵ�------------------
		while (ch == ' ' || ch == '\t' || ch == '\f' || ch == '\n') {
			buffer.clear();
			getChar();
		}
		if (ch == EOF)
			return;
		// cout << "char " << ch << endl;

		// ��ȡ��ǰ����ʼ����, �����������ջ���������get�ַ��������ch��
		// �ļ���һ���ַ�������ǿ�в���еĻ���ÿ�н�β�ض��ǻ��з�
		// ��ôת��һ��˼·�����ǰ�ÿһ�У����˵�һ�У���0�±�ռ����Ϊ��һ�еĻ��з����Ϳ��Խ����
		start_row = this->row_count;
		start_col = this->col_count-1;

		// ------------------------------------------------------
		// ���������Զ�����ȡ
		if (isdigit(ch)) {
			this->type = dataType::NUMBER;
			parser_number(start_row, start_col);
		}

		// �����ַ������Զ�����ȡ
		else if (ch == '\'') {
			this->type = dataType::CHAR;
			parser_char(start_row, start_col);
		}

		// �����ַ��������Զ�����ȡ
		else if (ch == '\"') {
			this->type = dataType::STRING;
			parser_string(start_row, start_col);
		}

		// �����ʶ���Զ�����ȡ
		else if (isalpha(ch) || ch == '_') {
			this->type = dataType::IDENTIFIER;
			parser_identifier(start_row, start_col);
		}

		// �����������Զ�����ȡ
		else if (ch == '>' || ch == '<'
			|| ch == '=' || ch == '*' || ch == '%' || ch == ':' || ch == '^' || ch == '!'
			|| ch == '+' || ch == '-' || ch == '&' || ch == '|'
			|| ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '.' || ch == '?' || ch == ',' || ch == ';' || ch == '~' || ch == '\\'
			) {
			this->type = dataType::PUNCTUATOR;
			parser_punctuator(start_row, start_col);
		}

		// ����Ԥ�����Զ�����ȡ
		else if (ch == '#') {
			this->type = dataType::PRECOMPLIED;
			parser_precompiled(start_row, start_col);
		}

		// '/'�й�ע�ͣ������
		else if (ch == '/') {
			char next = inFile.peek();

			// ����ע���Զ�����ȡ
			if (next == '/' || next == '*') {
				this->type = dataType::NOTE;
				parser_note(start_row, start_col);
			}

			// �����������Զ�����ȡ (/=����/)
			else {
				this->type = dataType::PUNCTUATOR;
				parser_punctuator(start_row, start_col);
			}
		}

		// ��ͷ���Ƿ��ַ���ֱ��������������Ͷ���
		else {
			cout << ch;
			this->type = dataType::ERROR;
			print_errorFile("start with invalid character", start_row, start_col);
			
			// ����buffer���������һ��
			buffer.clear();
			getChar();
		}

		// ��֤�������ȡ�˵�ǰ�ʻ㲢��buffer����һ���߽�������ַ�����Ӧ�ļ�ָ��
		// ͳ�Ƽ���
		word_count++;
		switch (this->type)
		{
		case dataType::NUMBER: {
			number_count++;
			break;
		}
		case dataType::CHAR: {
			char_count++;
			break;
		}
		case dataType::STRING: {
			string_count++;
			break;
		}
		case dataType::IDENTIFIER: {
			identifier_count++;
			break;
		}
		case dataType::KEYWORD: {
			keyword_count++;
			break;
		}
		case dataType::PUNCTUATOR: {
			punctuator_count++;
			break;
		}
		case dataType::PRECOMPLIED: {
			precompile_count++;
			break;
		}
		case dataType::NOTE: {
			note_count++;
			break;
		}
		case dataType::ERROR: {
			error_count++;
			break;
		}
		}
	}
}



// ���ֶ�ȡ����Ϊʮ����&�˽�������С��----------------------------------------------------------
void Lex::parser_number(int start_row, int start_col) {
	/* ״̬
		1������ʶ������
		2��ǰ����С����
		3����ȡС������
		4��ǰ����E
		5����ȡE��+/-
		6����ȡE��ֵ����
		7����ȡʮ��������ֵ
	*/ 
	int state = 1;

	// step1���ȶ���ʮ�����ƻ��߰˽���ǰ׺-----------------------------------
	// ��0�ģ�ʮ�����ƻ��߰˽��ƻ��ߵ�0
	if (ch == '0') {
		getChar();

		// ����ȥʮ������
		if (ch == 'x')
			state = 7;
		// 00��ͷ������
		else if (ch == '0') {
			// buffer����pop����Ҫ��������ʽ
			this->type = dataType::ERROR;
			print_errorFile("number start by 00", start_row, start_col);

			// ����buffer���������һ��
			buffer.clear();
			getChar();
			return;
		}
		// ����ȥ�˽��ƣ��ѱض�����0�ˣ�
		else if (isdigit(ch));
		// ʶ���С���㣬��ʽΪ0.**
		else if (ch == '.')
			state = 2;
		// ��ֹ��ȡ��EOF����������
		else {
			buffer.pop_back();
			print_outFile(start_row, start_col);

			// ����buffer���Ұѵ�ǰ�ַ�����buffer
			buffer.clear();
			buffer.push_back(ch);
			return;
		}
	}

	// step2��������ȡ����ʼΪʮ����state=1����С��state=2����ʮ������state=7��------
	while (1) {
		getChar();

		switch (state) {
		// ������ȡ����
		case 1: {
			// ��ȡ���֣�����
			if (isdigit(ch));
			// ����С���㣬����С����״̬
			else if (ch == '.')
				state = 2;
			// ����E������E״̬
			else if (ch == 'E')
				state = 4;
			// ��ֹ��ȡ
			else {
				buffer.pop_back();
				print_outFile(start_row, start_col);

				// ����buffer���Ұѵ�ǰ�ַ�����buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// ǰ����С����
		case 2: {
			// �������֣������
			if (isdigit(ch))
				state = 3;
			// С����������֣���Ȼ��ʽ����
			else {
				buffer.pop_back();
				this->type = dataType::ERROR;
				print_errorFile("number **. ended invalid", start_row, start_col);

				// ����buffer���Ұѵ�ǰ�ַ�����buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// ��ȡС������
		case 3: {
			// �������֣������
			if (isdigit(ch));
			// ����E��ת��״̬
			else if (ch == 'E')
				state = 4;
			// ��ֹ��ȡ
			else {
				buffer.pop_back();
				print_outFile(start_row, start_col);

				// ����buffer���Ұѵ�ǰ�ַ�����buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// ǰ����E
		case 4: {
			// **E0
			if (ch == '0') {
				// buffer����pop����Ҫ��������ʽ
				this->type = dataType::ERROR;
				print_errorFile("number **E0 is invalid", start_row, start_col);

				// ����buffer���Ұѵ�ǰ�ַ�����buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			// **E+/-
			else if (ch == '+' || ch == '-')
				state = 5;
			// **E(0/1/--9)
			else if (isdigit(ch))
				state = 6;
			// E�������ֲ���+-����Ȼ����
			else {
				buffer.pop_back();
				this->type = dataType::ERROR;
				print_errorFile("number **E ended invalid", start_row, start_col);

				// ����buffer���Ұѵ�ǰ�ַ�����buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// ��E+/-���һ��
		case 5: {
			// �����0����
			if (ch == '0') {
				// buffer����pop����Ҫ��������ʽ
				this->type = dataType::ERROR;
				print_errorFile("number **E+/-0 is invalid", start_row, start_col);

				// ����buffer���Ұѵ�ǰ�ַ�����buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			// ���֣�ת��������ȡָ������
			else if (isdigit(ch))
				state = 6;
			// ��Ϊ���֣������ַ�
			else {
				buffer.pop_back();
				this->type = dataType::ERROR;
				print_errorFile("number **E+/- ended invalid", start_row, start_col);

				// ����buffer���Ұѵ�ǰ�ַ�����buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// ��E+/-С������
		case 6: {
			// ���֣������
			if (isdigit(ch));
			// ��ֹ��ȡ
			else {
				buffer.pop_back();
				print_outFile(start_row, start_col);

				// ����buffer���Ұѵ�ǰ�ַ�����buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// ��ȡʮ��������ֵ���֣���ֻ�����ȡ��������֧��С��ָ�����͵�ʮ����������
		case 7: {
			// ʮ���������֣������
			if (isdigit(ch) || ch>='a'&&ch<='f' || ch>='A'&&ch<='F');
			// ��ֹ��ȡ
			else {
				buffer.pop_back();
				print_outFile(start_row, start_col);

				// ����buffer���Ұѵ�ǰ�ַ�����buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		}
	}
}

// �ַ�������ȡ----------------------------------------------------------------------
void Lex::parser_char(int start_row, int start_col) {
	// ��ȡ��ĸ
	if (getChar() == EOF || ch == '\n') {
		buffer.pop_back();
		this->type = dataType::ERROR;
		print_errorFile("' ended with no character", start_row, start_col);

		// ����buffer���Ұѵ�ǰ�ַ�����buffer
		buffer.clear();
		buffer.push_back(ch);
		return;
	}
	// ת�������
	else if (ch == '\\') {
		char next = inFile.peek();
		if (next == '\n' || next == '\'' || next == '\"' || next == '\\') {
			buffer.pop_back();
			getChar();
		}
		else {
			buffer.pop_back();
			this->type = dataType::ERROR;
			print_errorFile("' ended with no character", start_row, start_col);

			// ����buffer���Ұѵ�ǰ�ַ�����buffer
			buffer.clear();
			buffer.push_back(ch);
			return;
		}
	}
	// ��ȡ�ұ� '
	if (getChar() == '\'') {
		print_outFile(start_row, start_col);

		// ����buffer���������һ��
		buffer.clear();
		getChar();
	}
	else {
		buffer.pop_back();
		this->type = dataType::ERROR;
		print_errorFile("char with no right '", start_row, start_col);

		// ����buffer���Ұѵ�ǰ�ַ�����buffer
		buffer.clear();
		buffer.push_back(ch);
	}
}

// �ַ�����ȡ-----------------------------------------------------------------------
void Lex::parser_string(int start_row, int start_col) {
	// ѭ����ȡ���ұߵ�"
	while (1) {
		// ��ȡ�ַ������ļ�β�򷵻ش�����Ϣ
		if (getChar() == EOF || ch == '\n') {
			buffer.pop_back();
			this->type = dataType::ERROR;
			print_errorFile("string with no right \"", start_row, start_col);
			return;
		}

		// ת���ַ�����Ҫ������һ����˭
		if (ch == '\\') {
			getChar();
			// ������պϴ���
			if (ch == EOF) {
				buffer.pop_back();
				this->type = dataType::ERROR;
				print_errorFile("string with no right \"", start_row, start_col);

				// ����buffer���Ұѵ�ǰ�ַ�����buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			// ���У���Ҫpopת����ͻ��з�
			else if (ch == '\n') {
				buffer.pop_back();
				buffer.pop_back();
				// �Ե� \t \n
				while (inFile.peek() == '\t' || inFile.peek() == '\n') {
					getChar();
					buffer.pop_back();
				} 
			}
		}
		// ˫���ţ��պ�
		else if (ch == '"') {
			print_outFile(start_row, start_col);

			// ����buffer���������һ��
			buffer.clear();
			getChar();
			return;
		}
	}
}

// ��ʶ���������ؼ��ֶ�ȡ��----------------------------------------------------------------
void Lex::parser_identifier(int start_row, int start_col) {
	while (1) {
		getChar();

		// �Ϸ��ַ�������
		if (isdigit(ch) || isalpha(ch) || ch == '_');

		// ��������
		else {
			buffer.pop_back();

			// �ǹؼ���
			if (keyword.find(buffer) != keyword.end()) {
				this->type = dataType::KEYWORD;
				print_outFile(start_row, start_col);
			}
			// ��ͨ��ʶ��
			else
				print_outFile(start_row, start_col);

			// ����buffer���Ұѵ�ǰ�ַ�����buffer
			buffer.clear();
			buffer.push_back(ch);
			return;
		}
	}
}

// �����Ӷ�ȡ-----------------------------------------------------------------------
void Lex::parser_punctuator(int start_row, int start_col) {
	char next;
	// >,>>,>=,>>=	-------------------------------------------
	if (ch == '>') {
		next = inFile.peek();
		// >> >=
		if (next == '>' || next == '=') {
			getChar();
			// >>=
			if (ch == '>') {
				next = inFile.peek();
				if (next == '=')
					getChar();
			}
		}
	}
	// < , << , <= , <<=, <>	---------------------------------
	else if (ch == '<') {
		next = inFile.peek();
		// <> <= <<
		if (next == '>' || next == '=' || next == '<') {
			getChar();
			// <<=
			if (ch == '<')
			{
				next = inFile.peek();
				if (next == '=')
					getChar();
			}
		}
	}
	// �����=	--------------------------------------------------
	else if (ch == '=' || ch == '*' || ch == '/' || ch == '%' || ch == ':' || ch == '^' || ch == '!') {
		next = inFile.peek();
		if (next == '=')
			getChar();
	}
	// +��+=��-��-=��++��--,->, &,&&,&=,|,||,|=	-------------------
	else if (ch == '+' || ch == '-' || ch == '&' || ch == '|') {
		next = inFile.peek();
		// ��ͬ���ߺ��������
		if (next == ch || next == '=')
			getChar();
		// ->
		else if (ch == '-' && next == '>')
			getChar();
	}

	// ���
	print_outFile(start_row, start_col);

	// ����buffer���������һ��
	buffer.clear();
	getChar();
}

// Ԥ������Ϣ��ȡ--------------------------------------------------------------------
void Lex::parser_precompiled(int start_row, int start_col) {
	while (1) {
		// ��ȡ�ַ������ļ�β�����������ȡ
		if (getChar() == EOF || ch == '\n') {
			buffer.pop_back();
			print_outFile(start_row, start_col);

			// ����buffer���Ұѵ�ǰ�ַ�����buffer
			buffer.clear();
			buffer.push_back(ch);
			return;
		}

		// ת�����������
		else if (ch == '\\') {
			// ������ֹ
			if (getChar() == EOF) {
				buffer.pop_back();
				print_outFile(start_row, start_col);
				return;
			}
			// �ܹ�ת�廻��
			else if (ch == '\n') {
				buffer.pop_back();
				buffer.pop_back();
			}
		}
	}
}

// ע�Ͷ�ȡ------------------------------------------------------------------------
void Lex::parser_note(int start_row, int start_col) {
	getChar();
	// ����ע��---------------------------------------------------
	if (buffer == "//") {
		while (1) {
			// ��ȡ�ַ������ļ�β�����������ȡ
			if (getChar() == EOF || ch == '\n') {
				buffer.pop_back();
				print_outFile(start_row, start_col);

				// ����buffer���Ұѵ�ǰ�ַ�����buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
		}
	}
	// ����ע��---------------------------------------------------
	else {
		int state = 1;
		/* state
			1: /*
			2��/**
		*/
		while (1) {
			getChar();
			switch (state) {
			case 1: {
				// ��ȡ�ַ������ļ�β�򱨴��պ�
				if (ch == EOF) {
					buffer.pop_back();
					this->type = dataType::ERROR;
					print_errorFile("note just has /*", start_row, start_col);
					return;
				}
				else if (ch == '*')
					state = 2;
				break;
			}
			case 2: {
				if (ch == EOF) {
					buffer.pop_back();
					this->type = dataType::ERROR;
					print_errorFile("note just has /*---*", start_row, start_col);
					return;
				}
				else if (ch == '/') {
					print_outFile(start_row, start_col);

					// ����buffer���������һ��
					buffer.clear();
					getChar();
					return;
				}
				else
					state = 1;
				break;
			}
			}
		}
	}
}



// �����outFile
void Lex::print_outFile(int start_row, int start_col) {
	outStream->add(this->type, this->buffer, start_row, start_col);
}

// �����errorFile
void Lex::print_errorFile(string description, int start_row, int start_col) {
	errorStream->add(this->type, this->buffer, description, start_row, start_col);
}

// ���ɽ���ַ�����ӵ�terminal��outFile
void Lex::print_info()
{
	// sumary
	string ans = "\nProgram Analysis Result:\n";
	ans += "\nTotal lines :\t" + to_string(this->col_count);
	ans += "\nTotal chars : \t" + to_string(character_count);
	ans += "\nTotal words : \t" + to_string(word_count);
	ans += "\nnumbers :\t" + to_string(number_count);
	ans += "\nchar_consts :\t" + to_string(char_count);
	ans += "\nstrings :\t" + to_string(string_count);
	ans += "\nidentifiers :\t" + to_string(identifier_count);
	ans += "\nkeywords :\t" + to_string(keyword_count);
	ans += "\npunctuators :\t" + to_string(punctuator_count);
	ans += "\nprecompiles :\t" + to_string(precompile_count);
	ans += "\nnotes :\t" + to_string(note_count);
	ans += "\nerrors :\t" + to_string(error_count);

	// output
	outStream->sumary(ans);
	cout << ans;
	cout << "\ninFile :\t" << inFile_name;
	cout << "\noutFile :\t" << outFile_name;
	cout << "\nerrorFile :\t" << errorFile_name;
}
