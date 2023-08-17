#include <sstream>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include "LL1_Grammar.h"


#define EPSILON "Epsilon"
#define END "$"

Grammar::Grammar(string file_name) {
	// �ļ��������ս�����ս������ʼ���ţ����ʽ
	init(file_name);

	//�������ʽ�е���ݹ飬���û����ݹ���������
	Eliminate_left_common_factor();
	Eliminate_left_recur();

	//����������ս����FIRST/FOLLOW����
	create_first();
	create_follow();

	// �����
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
	// ����ÿ�еı��ʽ
	int flag = 0;
	for (auto item : expressions) {
		flag = 0;
		
		// �����ұߵ�����ʽ
		for (string g : item.second) {
			// ��ߵķ��ŵ����ұߵ��׷���������ݹ�
			if (g.substr(0, item.first.length()) == item.first) {
				flag = 1;
				break;
			}
		}

		// ��ǰ���ʽ��Ҫ����������� #
		vector<string> new_production;   // ԭ�����±��ʽ
		vector<string> new_symbol_production; // �·����±��ʽ
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
			// ����մ�
			new_symbol_production.push_back(EPSILON);
			expressions[item.first] = new_production;
			expressions.insert(make_pair(item.first + "#", new_symbol_production));
		}
	}
}

void Grammar::create_first() {
	// �������еķ��ս������ʽ
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

	// $��
	if (symbol == EPSILON)
		ans.insert(EPSILON);
	// �ս��
	else if (is_terminal(symbol))
		ans.insert(symbol);
	// ���ս��
	else if (is_not_terminal(symbol)) {
		// û���������
		if (First.find(symbol) != First.end())
			ans = First[symbol];
		else {
			// δ���м��㣬����÷��ս��������ʽ
			for (auto S : expressions[symbol]) {
				//S�ǵ�������ʽ
				int flag_e = 1;
				// ��symbolΪ��ߵ�ʽ�ӽ��зֽ⣬�ֽ�ɸ������ţ����д���
				auto outcome = split(S, " ");

				//��������һ������ʽ�ڲ��ķ���
				for (auto tmp_symbol : outcome) {
					if (tmp_symbol == "")
						continue;
					set<string> temp = find_first(tmp_symbol);
					set<string> swap;

					// ��$������Ҫ��
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

	// ��ʼ����
	if (symbol == start)
		ans.insert(END);
	// �������в���ʽ��symbol��follow����������
	if (Follow.find(symbol) != Follow.end()) {
		ans = Follow[symbol];
	}
	// ��Ҫ����follow
	else {
		for (auto item : expressions) {
			for (auto exp : item.second) {
				auto out = split(exp, " ");
				//�����ò���ʽfollow
				for (int i = 0; i < out.size(); i++) {
					// ������ķ���һ��
					if (out[i] == symbol) {
						//Ҫȷ�������first���������пջ�û�з��ţ��򷵻�item.first��follow
						//Ҫȷ����ν�ȡ����ǰ���ŵĺ���һ��
						string first_list = "";
						for (int j = i + 1; j < out.size(); j++)
						{
							first_list += out[j] + " ";
						}

						set<string> first_of_str = find_first_list(first_list);

						set<string> swap;
						if (first_of_str.size() != 0 && first_of_str.find(EPSILON) == first_of_str.end())  //û�п�
						{
							set_union(ans.begin(), ans.end(), first_of_str.begin(), first_of_str.end(), insert_iterator<set<string> >(swap, swap.begin()));
							ans = swap;
						}
						else if (first_of_str.size() == 0)  //֮�����ȥ�������������Ϊ�մ�Ҳ�᷵�ؿմ�
						{
							set<string> left_follow;
							if (item.first != symbol)  //��ֹѭ������
							{
								left_follow = find_follow(item.first);
							}
							set_union(ans.begin(), ans.end(), left_follow.begin(), left_follow.end(), insert_iterator<set<string> >(swap, swap.begin()));
							ans = swap;
						}
						else if (first_of_str.find(EPSILON) != first_of_str.end()) //�ҵ�����epsilon�����
						{
							set_union(ans.begin(), ans.end(), first_of_str.begin(), first_of_str.end(), insert_iterator<set<string> >(swap, swap.begin()));
							ans = swap;
							ans.erase(EPSILON);
							set<string> left_follow;
							if (item.first != symbol)  //��ֹѭ������
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

// ���ַ�������first
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
					// ���������ŵ�follow��
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