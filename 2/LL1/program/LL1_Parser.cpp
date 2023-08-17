#include "LL1_Parser.h"
#include <iomanip>
#define END "$"

extern vector<pair<string, string>> lex_outcome;

Parser::Parser(string grammar_file) {
	// ��ʼ���﷨������
	G = Grammar(grammar_file);

	// ջ����$����
	Symbol.push_back(END);
	// ջ������ʼ����
	Symbol.push_back(G.start);

	// �����ݴ����Ϊ$
	input.push_back(make_pair("PUNCTUATOR", END));
	// ���ʷ��������ļ����ת����������
	Lex_Input();

	// ����Grammar��ջ��������з���
	Analysis();
}


void Parser::Lex_Input() {
	// �Ѷ����ŵ�ջ��
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
		
		// ��õ�ǰ����ջջ��
		top1 = Symbol.back();
		top2 = input.back();

		string index;	// ��ǰ����ķ���
		// ����
		if (top2.first == "PUNCTUATOR")
			index = top2.second;
		// ����
		else if (top2.first == "NUMBER")
			index = "NUMBER";

		
		// ͳ�Ƴ�������
		int fail = 0;
		// ���
		string final_str = "";
		// ����������Ϣ
		string generative = "";
		vector<string> search_index;

		// ��ջջ��һ��
		if (top1 == index) {
			Symbol.pop_back();
			input.pop_back();
			final_str += top1;
		}
		// ջ����һ��
		else {
			// �����ǰѭ����index
			search_index.clear();
			search_index.push_back(top1);
			search_index.push_back(index);
			
			auto target = G.Table.find(search_index);
			// û�ҵ�ʽ�Ӷ�Ӧ
			if (target == G.Table.end()) {
				// ����
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
			// �ҵ���Synch
			else if (G.Table[search_index] == "Synch") {
				// ����
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
			// �ҵ��ˣ�����Ӧʽ��ת��
			else {
				string exp = target->second;
				auto out_symbol = G.split(exp, " ");
				generative += Symbol.back() + "->" + exp;
				Symbol.pop_back(); //����ջ��Ԫ��

				//����嵽ջ��
				for (int i = out_symbol.size() - 1; i >= 0; i--) {
					if (out_symbol[i] == "" || out_symbol[i] == "Epsilon")
						continue;
					Symbol.push_back(out_symbol[i]);
				}
			}

		}

		// �������Ϣ
		print(generative);

		// ���������Ϣ
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
	//�������ջ��Ϣ
	string left_out = "";
	for (auto left : Symbol)
		left_out += left;
	cout << setiosflags(ios::left) << setw(20) << left_out;

	//����Ҳ��ջ��Ϣ
	string right_out = "";
	for (auto right = input.rbegin(); right != input.rend(); right++)
		right_out += (*right).second;
	cout << setiosflags(ios::left) << setw(20) << right_out;
	cout << setiosflags(ios::left) << setw(20) << generative;

	cout << endl;
}