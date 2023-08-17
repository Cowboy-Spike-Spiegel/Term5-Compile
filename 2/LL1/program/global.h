#ifndef _GLOBAL_H
#define _GLOBAL_H

// 规定数据类型
enum dataType {
	NUMBER,		// 数字常量
	CHAR,		// 字符常量
	STRING,		// 字符串常量
	IDENTIFIER,	// 标识符
	KEYWORD,	// 关键字
	PUNCTUATOR,	// 运算符
	PRECOMPLIED,// 预编译
	NOTE,		// 注释
	ERROR
};

#endif