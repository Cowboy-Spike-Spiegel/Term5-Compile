#include <sstream>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include "LL1_Grammar.h"


#define EPSILON "Epsilon"
#define END "$"

Grammar::Grammar(string file_name) {
	// 文件读出非终结符，终结符，开始符号，表达式
	init(file_name);

	//消除表达式中的左递归，如果没有左递归则不做处理
	Eliminate_left_common_factor();
	Eliminate_left_recur();

	//计算各个非终结符的FIRST/FOLLOW集合
	create_first();
	create_follow();

	// 输出表
	predict_table();
	//print();
	print_predict_table();
}

void Grammar::init(string file_name) {
	ifstream file;
	file.open(file_name);
	if (!file.is_open()) {
		cout << "Fail to open grammar file!" << endl;
		return;
	}

	string line;

	// get not terminals
	getline(file, line);
	vector<string> non_t = split(line, " ");
	for (string item : non_t)
		not_term.insert(item);

	// get not terminals
	getline(file, line);
	vector<string> t = split(line, " ");
	for (string item : t)
		term.insert(item);
	term.insert(END);

	// get start symbol
	getline(file, line);
	start = line;

	// get grammar
	while (getline(file, line) && line != "") {
		vector<string> infos = split(line, "->");
		vector<string> info = split(infos[1], "|");
		expressions.insert(make_pair(infos[0], info));
	}
}

bool Grammar::is_terminal(string str) {
	return (term.find(str) != term.end());
}

bool Grammar::is_not_terminal(string str) {
	return (not_term.find(str) != not_term.end());
}

void Grammar::Eliminate_left_common_factor() {
}

void Grammar::Eliminate_left_recur() {
	// 遍历每行的表达式
	int flag = 0;
	for (auto item : expressions) {
		flag = 0;
		
		// 遍历右边的生成式
		for (string g : item.second) {
			// 左边的符号等于右边的首符，存在左递归
			if (g.substr(0, item.first.length()) == item.first) {
				flag = 1;
				break;
			}
		}

		// 当前表达式需要处理，后面添加 #
		vector<string> new_production;   // 原符号新表达式
		vector<string> new_symbol_production; // 新符号新表达式
		if (flag) {
			not_term.insert(item.first + "#");
			for (auto g : item.second) {
				string new_generative = "";
				if (g.substr(0, item.first.length()) == item.first) {
					new_generative = g.substr(item.first.length()) + " " + item.first + "#";
					new_symbol_production.push_back(new_generative);
				}
				else{
					new_generative = g + " " + item.first + "#";
					new_production.push_back(new_generative);
				}
			}
			// 加入空串
			new_symbol_production.push_back(EPSILON);
			expressions[item.first] = new_production;
			expressions.insert(make_pair(item.first + "#", new_symbol_production));
		}
	}
}

void Grammar::create_first() {
	// 对于所有的非终结符产生式
	for (auto item : expressions)
		find_first(item.first);

	// print First
	cout << "\nFirst:\n";
	for (auto item : First) {
		cout << item.first << "\n";
		for (auto f : item.second)
			cout << "\t" << f;
		cout << "\n";
	}
}

set<string> Grammar::find_first(string symbol) {
	set<string> ans;

	// $符
	if (symbol == EPSILON)
		ans.insert(EPSILON);
	// 终结符
	else if (is_terminal(symbol))
		ans.insert(symbol);
	// 非终结符
	else if (is_not_terminal(symbol)) {
		// 没加入则加入
		if (First.find(symbol) != First.end())
			ans = First[symbol];
		else {
			// 未进行计算，处理该非终结符的生成式
			for (auto S : expressions[symbol]) {
				//S是单个生成式
				int flag_e = 1;
				// 对symbol为左边的式子进行分解，分解成各个符号，进行处理
				auto outcome = split(S, " ");

				//遍历其中一个生成式内部的符号
				for (auto tmp_symbol : outcome) {
					if (tmp_symbol == "")
						continue;
					set<string> temp = find_first(tmp_symbol);
					set<string> swap;

					// 无$，不需要再
					if (temp.find(EPSILON) == temp.end()) {
						set_union(ans.begin(), ans.end(), temp.begin(), temp.end(), insert_iterator<set<string> >(swap, swap.begin()));
						ans = swap;
						flag_e = 0;
						break;
					}
					else {
						set_union(ans.begin(), ans.end(), temp.begin(), temp.end(), insert_iterator<set<string> >(swap, swap.begin()));
						ans = swap;
						ans.erase(EPSILON);
					}
				}
				if (flag_e == 1)
					ans.insert(EPSILON);
			}
			First.insert(make_pair(symbol, ans));
		}
	}

	return ans;
}

void Grammar::create_follow() {
	for (auto item : expressions)
		set<string> temp = find_follow(item.first);

	// print Follow
	cout << "\nFollow:\n";
	for (auto item : Follow) {
		cout << item.first << "\n";
		for (auto f : item.second)
			cout << "\t" << f;
		cout << "\n";
	}
}

set<string> Grammar::find_follow(string symbol) {
	set<string> ans;

	// 起始符号
	if (symbol == start)
		ans.insert(END);
	// 遍历所有产生式，symbol是follow，不用再找
	if (Follow.find(symbol) != Follow.end()) {
		ans = Follow[symbol];
	}
	// 需要计算follow
	else {
		for (auto item : expressions) {
			for (auto exp : item.second) {
				auto out = split(exp, " ");
				//遍历该产生式follow
				for (int i = 0; i < out.size(); i++) {
					// 和输入的符号一样
					if (out[i] == symbol) {
						//要确定后面的first，若后面有空或没有符号，则返回item.first的follow
						//要确定如何截取出当前符号的后面一段
						string first_list = "";
						for (int j = i + 1; j < out.size(); j++)
						{
							first_list += out[j] + " ";
						}

						set<string> first_of_str = find_first_list(first_list);

						set<string> swap;
						if (first_of_str.size() != 0 && first_of_str.find(EPSILON) == first_of_str.end())  //没有空
						{
							set_union(ans.begin(), ans.end(), first_of_str.begin(), first_of_str.end(), insert_iterator<set<string> >(swap, swap.begin()));
							ans = swap;
						}
						else if (first_of_str.size() == 0)  //之后可以去掉这种情况，因为空串也会返回空串
						{
							set<string> left_follow;
							if (item.first != symbol)  //防止循环迭代
							{
								left_follow = find_follow(item.first);
							}
							set_union(ans.begin(), ans.end(), left_follow.begin(), left_follow.end(), insert_iterator<set<string> >(swap, swap.begin()));
							ans = swap;
						}
						else if (first_of_str.find(EPSILON) != first_of_str.end()) //找到了有epsilon的情况
						{
							set_union(ans.begin(), ans.end(), first_of_str.begin(), first_of_str.end(), insert_iterator<set<string> >(swap, swap.begin()));
							ans = swap;
							ans.erase(EPSILON);
							set<string> left_follow;
							if (item.first != symbol)  //防止循环迭代
							{
								left_follow = find_follow(item.first);
							}
							swap.clear();
							set_union(ans.begin(), ans.end(), left_follow.begin(), left_follow.end(), insert_iterator<set<string> >(swap, swap.begin()));
							ans = swap;
						}

					}
				}
			}
		}
		Follow.insert(make_pair(symbol, ans));
	}
	return ans;
}

// 对字符序列求first
set<string> Grammar::find_first_list(string symbol_list) {
	set<string> ans;
	int flag_e = 1;
	auto outcome = split(symbol_list, " ");

	for (auto tmp_symbol : outcome)
	{
		if (tmp_symbol == "")
			continue;
		set<string> temp = find_first(tmp_symbol);
		set<string> swap;

		if (temp.find(EPSILON) == temp.end()) {
			set_union(ans.begin(), ans.end(), temp.begin(), temp.end(), insert_iterator<set<string> >(swap, swap.begin()));
			ans = swap;
			flag_e = 0;
			break;
		}
		else {
			/*cout << "erase" << endl;*/
			set_union(ans.begin(), ans.end(), temp.begin(), temp.end(), insert_iterator<set<string> >(swap, swap.begin()));
			ans = swap;
			ans.erase(EPSILON);
		}
	}
	if (flag_e == 1)
		ans.insert(EPSILON);

	return ans;
}

void Grammar::predict_table() {
	for (auto item : expressions) {
		for (auto exp : item.second) {
			set<string> first_symbol = find_first_list(exp);
			for (auto str : first_symbol) {
				if (str != EPSILON) {
					vector<string> temp;
					temp.push_back(item.first);
					temp.push_back(str);

					Table.insert(make_pair(temp, exp));
				}
				else {
					// 放置左侧符号的follow集
					for (auto f_str : Follow[item.first]) {
						vector<string> temp;
						temp.push_back(item.first);
						temp.push_back(f_str);

						Table.insert(make_pair(temp, exp));
					}
				}
			}
		}
	}

	for (auto item : expressions) {
		for (auto follow : Follow[item.first]) {
			vector<string> temp;
			temp.push_back(item.first);
			temp.push_back(follow);

			if (Table.find(temp) == Table.end())
				Table[temp] = "Synch";
		}
	}
}

void Grammar::print_predict_table() {
	cout << "\n";
	int gap = 12;
	
	int count = 1;
	cout << setiosflags(ios::left) << setw(gap) << "|Table";
	for (auto t : term) {
		count++;
		string str = "|" + t;
		cout << setiosflags(ios::left) << setw(gap) << str;
	}
	cout << "|\n|";
	for (int i = 0; i < count; i++)
		cout << "-----------|";
	cout << "\n";

	for (auto non_t : not_term) {
		cout << setiosflags(ios::left) << setw(gap) << "|" + non_t;
		for (auto t : term) {
			vector<string> temp;
			temp.push_back(non_t);
			temp.push_back(t);

			if (Table.find(temp) != Table.end()) {
				//cout << setw(gap) << non_t << "->" << Table[temp];
				if (Table[temp] != "Synch")
					cout << setiosflags(ios::left) << setw(gap) << "|" + non_t + "->" + Table[temp];
				else
					cout << setiosflags(ios::left) << setw(gap) << "|" + Table[temp];
			}
			else
				cout << setiosflags(ios::left) << setw(gap) << "|";
		}
		cout << "|\n|";
		for (int i = 0; i < count; i++)
			cout << "-----------|";
		cout << "\n";
	}
}

void Grammar::print()
{
	for (auto item : Table) {
		for (auto f : item.first)
			cout << f << "\n";
		cout << item.second << "---\n\n";
	}
}


vector<string> Grammar::split(const string& str, const string& splits) {
	vector<string> res;
	if (str == "")
		return res;
	string strs = str + splits;
	size_t pos = strs.find(splits);
	int step = splits.size();

	while (pos != strs.npos) {
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		strs = strs.substr(pos + step, strs.size());
		pos = strs.find(splits);
	}
	return res;
}