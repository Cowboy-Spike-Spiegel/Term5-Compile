#include "LL1_Parser.h"
#include <iomanip>
#define END "$"

extern vector<pair<string, string>> lex_outcome;

Parser::Parser(string grammar_file) {
	// 初始化语法分析表
	G = Grammar(grammar_file);

	// 栈放入$符号
	Symbol.push_back(END);
	// 栈放入起始符号
	Symbol.push_back(G.start);

	// 输入暂存符号为$
	input.push_back(make_pair("PUNCTUATOR", END));
	// 将词法分析的文件结果转化到输入中
	Lex_Input();

	// 参照Grammar对栈和输入进行分析
	Analysis();
}


void Parser::Lex_Input() {
	// 把东西放到栈中
	for (int i=lex_outcome.size()-1; i >= 0 ; i--)
		input.push_back(lex_outcome[i]);
}


void Parser::Analysis() {
	cout << "\n";
	cout << setiosflags(ios::left) << setw(20) << "STACK";
	cout << setiosflags(ios::left) << setw(20) << "INPUT";
	cout << setiosflags(ios::left) << setw(20) << "GENERATIVE" << endl;
	print("");

	string top1;
	pair<string, string> top2;
	while (1) {
		
		// 获得当前两个栈栈顶
		top1 = Symbol.back();
		top2 = input.back();

		string index;	// 当前保存的符号
		// 符号
		if (top2.first == "PUNCTUATOR")
			index = top2.second;
		// 数据
		else if (top2.first == "NUMBER")
			index = "NUMBER";

		
		// 统计出错数量
		int fail = 0;
		// 结果
		string final_str = "";
		// 保存生成信息
		string generative = "";
		vector<string> search_index;

		// 两栈栈顶一样
		if (top1 == index) {
			Symbol.pop_back();
			input.pop_back();
			final_str += top1;
		}
		// 栈顶不一样
		else {
			// 清除当前循环的index
			search_index.clear();
			search_index.push_back(top1);
			search_index.push_back(index);
			
			auto target = G.Table.find(search_index);
			// 没找到式子对应
			if (target == G.Table.end()) {
				// 出错
				if (index != "$") {
					input.pop_back();
					generative += "Error, and pop " + index;
					fail++;
				}
				else {
					cout << "Illegal input!" << endl;
					break;
				}
			}
			// 找到是Synch
			else if (G.Table[search_index] == "Synch") {
				// 出错
				if (top1 != "$") {
					Symbol.pop_back();
					generative += "Error, and pop " + top1;
					fail++;
				}
				else {
					cout << "Illegal input!" << endl;
					break;
				}
			}
			// 找到了，用相应式子转换
			else {
				string exp = target->second;
				auto out_symbol = G.split(exp, " ");
				generative += Symbol.back() + "->" + exp;
				Symbol.pop_back(); //弹出栈顶元素

				//逆序插到栈中
				for (int i = out_symbol.size() - 1; i >= 0; i--) {
					if (out_symbol[i] == "" || out_symbol[i] == "Epsilon")
						continue;
					Symbol.push_back(out_symbol[i]);
				}
			}

		}

		// 输出行信息
		print(generative);

		// 输出结束信息
		if (Symbol.size() == 1) {
			if (input.size() == 1) {
				if (fail == 0)
					cout << "Analysis success!" << endl;
				else
					cout << "Error, and finally got <" + final_str + ">, with [" << fail << "] fails.\n";
				break;
			}
			else {
				cout << "Illegal input!" << endl;
				break;
			}
		}
	}
}

void Parser::print(string generative) {
	//输出左侧的栈信息
	string left_out = "";
	for (auto left : Symbol)
		left_out += left;
	cout << setiosflags(ios::left) << setw(20) << left_out;

	//输出右侧的栈信息
	string right_out = "";
	for (auto right = input.rbegin(); right != input.rend(); right++)
		right_out += (*right).second;
	cout << setiosflags(ios::left) << setw(20) << right_out;
	cout << setiosflags(ios::left) << setw(20) << generative;

	cout << endl;
}