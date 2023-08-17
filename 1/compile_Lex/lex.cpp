#include "Lex.h"


// 初始化函数
Lex::Lex(string inFile_name, string outFile_name, string errorFile_name)
{
	// 处理三个文件名
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

	// 初始化行列
	ch = ' ';	// 初始化
	row_count = 1;
	col_count = 1;

	// 初始化关键字，将关键字数组元素逐个放到set中
	vector<string> keywordtable = { "int","long","short","float","double", "char" ,"unsigned", "signed" ,
		"const" ,"void" ,"volatile", "enum","struct" ,"union" ,
		"if", "else", "goto", "switch" ,"case" ,"do", "while", "for", "continue","break",
		"return", "default","typedef", "auto" ,"register", "extern", "static", "sizeof" };
	for (auto s : keywordtable)
		keyword.insert(s);
}

// 删除new的输出类
Lex::~Lex() {
	delete outStream;
	delete errorStream;
}



// 读取单个字符
char Lex::getChar()
{
	ch = inFile.get();
	
	//将内容存入缓存区
	buffer.push_back(ch);
	character_count++;
	col_count++;

	// 换行初始化数值
	if (ch == '\n') {
		row_count++;
		col_count = 1;	// 理解：上一行换行符占据了此行的0下标空间
	}

	return ch;
}



// 读取核心函数----------------------------------------------------------------------
void Lex::parser()
{
	// 循环读取词语-----------------------------------------
	int start_row, start_col;
	while (!inFile.eof())
	{
		// 词法分析外部，进行无意义符号吃掉------------------
		while (ch == ' ' || ch == '\t' || ch == '\f' || ch == '\n') {
			buffer.clear();
			getChar();
		}
		if (ch == EOF)
			return;
		// cout << "char " << ch << endl;

		// 获取当前的起始坐标, 进入分析，清空缓存区，并get字符到类变量ch中
		// 文件是一个字符串，但强行拆成行的话，每行结尾必定是换行符
		// 那么转换一下思路，我们把每一行（除了第一行）的0下标空间放置为上一行的换行符，就可以解决了
		start_row = this->row_count;
		start_col = this->col_count-1;

		// ------------------------------------------------------
		// 进入数字自动机读取
		if (isdigit(ch)) {
			this->type = dataType::NUMBER;
			parser_number(start_row, start_col);
		}

		// 进入字符常量自动机读取
		else if (ch == '\'') {
			this->type = dataType::CHAR;
			parser_char(start_row, start_col);
		}

		// 进入字符串常量自动机读取
		else if (ch == '\"') {
			this->type = dataType::STRING;
			parser_string(start_row, start_col);
		}

		// 进入标识符自动机读取
		else if (isalpha(ch) || ch == '_') {
			this->type = dataType::IDENTIFIER;
			parser_identifier(start_row, start_col);
		}

		// 进入运算子自动机读取
		else if (ch == '>' || ch == '<'
			|| ch == '=' || ch == '*' || ch == '%' || ch == ':' || ch == '^' || ch == '!'
			|| ch == '+' || ch == '-' || ch == '&' || ch == '|'
			|| ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '.' || ch == '?' || ch == ',' || ch == ';' || ch == '~' || ch == '\\'
			) {
			this->type = dataType::PUNCTUATOR;
			parser_punctuator(start_row, start_col);
		}

		// 进入预编译自动机读取
		else if (ch == '#') {
			this->type = dataType::PRECOMPLIED;
			parser_precompiled(start_row, start_col);
		}

		// '/'有关注释，特殊的
		else if (ch == '/') {
			char next = inFile.peek();

			// 进入注释自动机读取
			if (next == '/' || next == '*') {
				this->type = dataType::NOTE;
				parser_note(start_row, start_col);
			}

			// 进入运算子自动机读取 (/=或者/)
			else {
				this->type = dataType::PUNCTUATOR;
				parser_punctuator(start_row, start_col);
			}
		}

		// 开头即非法字符，直接输出错误，无类型定义
		else {
			cout << ch;
			this->type = dataType::ERROR;
			print_errorFile("start with invalid character", start_row, start_col);
			
			// 清理buffer并且往后读一个
			buffer.clear();
			getChar();
		}

		// 保证到这里，读取了当前词汇并且buffer里有一个边界往后的字符，对应文件指针
		// 统计计数
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



// 数字读取，分为十进制&八进制整数小数----------------------------------------------------------
void Lex::parser_number(int start_row, int start_col) {
	/* 状态
		1：正常识别整数
		2：前面是小数点
		3：读取小数部分
		4：前面是E
		5：读取E后+/-
		6：读取E数值部分
		7：读取十六进制数值
	*/ 
	int state = 1;

	// step1：先读出十六进制或者八进制前缀-----------------------------------
	// 带0的，十六进制或者八进制或者单0
	if (ch == '0') {
		getChar();

		// 读进去十六进制
		if (ch == 'x')
			state = 7;
		// 00开头，报错
		else if (ch == '0') {
			// buffer不用pop，需要输出错误格式
			this->type = dataType::ERROR;
			print_errorFile("number start by 00", start_row, start_col);

			// 清理buffer并且往后读一个
			buffer.clear();
			getChar();
			return;
		}
		// 读进去八进制（已必定不是0了）
		else if (isdigit(ch));
		// 识别出小数点，形式为0.**
		else if (ch == '.')
			state = 2;
		// 截止读取（EOF或者其他）
		else {
			buffer.pop_back();
			print_outFile(start_row, start_col);

			// 清理buffer并且把当前字符加入buffer
			buffer.clear();
			buffer.push_back(ch);
			return;
		}
	}

	// step2：正常读取（起始为十进制state=1或者小数state=2或者十六进制state=7）------
	while (1) {
		getChar();

		switch (state) {
		// 正常读取整数
		case 1: {
			// 读取数字，继续
			if (isdigit(ch));
			// 读到小数点，进入小数点状态
			else if (ch == '.')
				state = 2;
			// 读到E，进入E状态
			else if (ch == 'E')
				state = 4;
			// 截止读取
			else {
				buffer.pop_back();
				print_outFile(start_row, start_col);

				// 清理buffer并且把当前字符加入buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// 前面是小数点
		case 2: {
			// 都是数字，往后读
			if (isdigit(ch))
				state = 3;
			// 小数点后不是数字，显然格式不对
			else {
				buffer.pop_back();
				this->type = dataType::ERROR;
				print_errorFile("number **. ended invalid", start_row, start_col);

				// 清理buffer并且把当前字符加入buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// 读取小数部分
		case 3: {
			// 都是数字，往后读
			if (isdigit(ch));
			// 出现E，转移状态
			else if (ch == 'E')
				state = 4;
			// 截止读取
			else {
				buffer.pop_back();
				print_outFile(start_row, start_col);

				// 清理buffer并且把当前字符加入buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// 前面是E
		case 4: {
			// **E0
			if (ch == '0') {
				// buffer不用pop，需要输出错误格式
				this->type = dataType::ERROR;
				print_errorFile("number **E0 is invalid", start_row, start_col);

				// 清理buffer并且把当前字符加入buffer
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
			// E后不是数字不是+-，显然不对
			else {
				buffer.pop_back();
				this->type = dataType::ERROR;
				print_errorFile("number **E ended invalid", start_row, start_col);

				// 清理buffer并且把当前字符加入buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// 读E+/-后第一个
		case 5: {
			// 后面跟0，错
			if (ch == '0') {
				// buffer不用pop，需要输出错误格式
				this->type = dataType::ERROR;
				print_errorFile("number **E+/-0 is invalid", start_row, start_col);

				// 清理buffer并且把当前字符加入buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			// 数字，转到正常读取指数部分
			else if (isdigit(ch))
				state = 6;
			// 不为数字，错误字符
			else {
				buffer.pop_back();
				this->type = dataType::ERROR;
				print_errorFile("number **E+/- ended invalid", start_row, start_col);

				// 清理buffer并且把当前字符加入buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// 读E+/-小数部分
		case 6: {
			// 数字，往后读
			if (isdigit(ch));
			// 截止读取
			else {
				buffer.pop_back();
				print_outFile(start_row, start_col);

				// 清理buffer并且把当前字符加入buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		// 读取十六进制数值部分（暂只允许读取整数，不支持小数指数类型的十六进制数）
		case 7: {
			// 十六进制数字，往后读
			if (isdigit(ch) || ch>='a'&&ch<='f' || ch>='A'&&ch<='F');
			// 截止读取
			else {
				buffer.pop_back();
				print_outFile(start_row, start_col);

				// 清理buffer并且把当前字符加入buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			break;
		}
		}
	}
}

// 字符常量读取----------------------------------------------------------------------
void Lex::parser_char(int start_row, int start_col) {
	// 读取字母
	if (getChar() == EOF || ch == '\n') {
		buffer.pop_back();
		this->type = dataType::ERROR;
		print_errorFile("' ended with no character", start_row, start_col);

		// 清理buffer并且把当前字符加入buffer
		buffer.clear();
		buffer.push_back(ch);
		return;
	}
	// 转义符处理
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

			// 清理buffer并且把当前字符加入buffer
			buffer.clear();
			buffer.push_back(ch);
			return;
		}
	}
	// 读取右边 '
	if (getChar() == '\'') {
		print_outFile(start_row, start_col);

		// 清理buffer并且往后读一个
		buffer.clear();
		getChar();
	}
	else {
		buffer.pop_back();
		this->type = dataType::ERROR;
		print_errorFile("char with no right '", start_row, start_col);

		// 清理buffer并且把当前字符加入buffer
		buffer.clear();
		buffer.push_back(ch);
	}
}

// 字符串读取-----------------------------------------------------------------------
void Lex::parser_string(int start_row, int start_col) {
	// 循环读取到右边的"
	while (1) {
		// 读取字符，若文件尾则返回错误信息
		if (getChar() == EOF || ch == '\n') {
			buffer.pop_back();
			this->type = dataType::ERROR;
			print_errorFile("string with no right \"", start_row, start_col);
			return;
		}

		// 转义字符，需要看后面一个是谁
		if (ch == '\\') {
			getChar();
			// 输出不闭合错误
			if (ch == EOF) {
				buffer.pop_back();
				this->type = dataType::ERROR;
				print_errorFile("string with no right \"", start_row, start_col);

				// 清理buffer并且把当前字符加入buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
			// 换行，需要pop转义符和换行符
			else if (ch == '\n') {
				buffer.pop_back();
				buffer.pop_back();
				// 吃掉 \t \n
				while (inFile.peek() == '\t' || inFile.peek() == '\n') {
					getChar();
					buffer.pop_back();
				} 
			}
		}
		// 双引号，闭合
		else if (ch == '"') {
			print_outFile(start_row, start_col);

			// 清理buffer并且往后读一个
			buffer.clear();
			getChar();
			return;
		}
	}
}

// 标识符（包括关键字读取）----------------------------------------------------------------
void Lex::parser_identifier(int start_row, int start_col) {
	while (1) {
		getChar();

		// 合法字符，读入
		if (isdigit(ch) || isalpha(ch) || ch == '_');

		// 结束读入
		else {
			buffer.pop_back();

			// 是关键字
			if (keyword.find(buffer) != keyword.end()) {
				this->type = dataType::KEYWORD;
				print_outFile(start_row, start_col);
			}
			// 普通标识符
			else
				print_outFile(start_row, start_col);

			// 清理buffer并且把当前字符加入buffer
			buffer.clear();
			buffer.push_back(ch);
			return;
		}
	}
}

// 运算子读取-----------------------------------------------------------------------
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
	// 后面跟=	--------------------------------------------------
	else if (ch == '=' || ch == '*' || ch == '/' || ch == '%' || ch == ':' || ch == '^' || ch == '!') {
		next = inFile.peek();
		if (next == '=')
			getChar();
	}
	// +，+=，-，-=，++，--,->, &,&&,&=,|,||,|=	-------------------
	else if (ch == '+' || ch == '-' || ch == '&' || ch == '|') {
		next = inFile.peek();
		// 相同或者后面跟等于
		if (next == ch || next == '=')
			getChar();
		// ->
		else if (ch == '-' && next == '>')
			getChar();
	}

	// 输出
	print_outFile(start_row, start_col);

	// 清理buffer并且往后读一个
	buffer.clear();
	getChar();
}

// 预编译信息读取--------------------------------------------------------------------
void Lex::parser_precompiled(int start_row, int start_col) {
	while (1) {
		// 读取字符，若文件尾或换行则结束读取
		if (getChar() == EOF || ch == '\n') {
			buffer.pop_back();
			print_outFile(start_row, start_col);

			// 清理buffer并且把当前字符加入buffer
			buffer.clear();
			buffer.push_back(ch);
			return;
		}

		// 转义符编译连接
		else if (ch == '\\') {
			// 结束终止
			if (getChar() == EOF) {
				buffer.pop_back();
				print_outFile(start_row, start_col);
				return;
			}
			// 能够转义换行
			else if (ch == '\n') {
				buffer.pop_back();
				buffer.pop_back();
			}
		}
	}
}

// 注释读取------------------------------------------------------------------------
void Lex::parser_note(int start_row, int start_col) {
	getChar();
	// 单行注释---------------------------------------------------
	if (buffer == "//") {
		while (1) {
			// 读取字符，若文件尾或换行则结束读取
			if (getChar() == EOF || ch == '\n') {
				buffer.pop_back();
				print_outFile(start_row, start_col);

				// 清理buffer并且把当前字符加入buffer
				buffer.clear();
				buffer.push_back(ch);
				return;
			}
		}
	}
	// 多行注释---------------------------------------------------
	else {
		int state = 1;
		/* state
			1: /*
			2：/**
		*/
		while (1) {
			getChar();
			switch (state) {
			case 1: {
				// 读取字符，若文件尾则报错不闭合
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

					// 清理buffer并且往后读一个
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



// 输出到outFile
void Lex::print_outFile(int start_row, int start_col) {
	outStream->add(this->type, this->buffer, start_row, start_col);
}

// 输出到errorFile
void Lex::print_errorFile(string description, int start_row, int start_col) {
	errorStream->add(this->type, this->buffer, description, start_row, start_col);
}

// 生成结果字符串添加到terminal和outFile
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
