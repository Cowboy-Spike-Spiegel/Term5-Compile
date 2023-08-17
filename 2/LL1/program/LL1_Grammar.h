#ifndef LL1_GRAM_H
#define LL1_GRAM_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

class Grammar {
public:
    // 待分析的生成式集合
    map<string, vector<string>> expressions;
    // 终结符集合
    set<string> term;
    // 非终结符集合
    set<string> not_term;
    // 起始符号
    string start;

    // 非终结符的First集
    map<string, set<string>> First;
    // 非终结符的Follow集
    map<string, set<string>> Follow;

    // 预测分析表
    map<vector<string>, string> Table;

    Grammar(string file_name);
    Grammar() {};

    // 分割字符串
    vector<string> split(const string& str, const string& splits);

private:
    // 文件读出非终结符，终结符，开始符号，表达式
    void init(string file_name);
    
    // 消除左公因子
    void Eliminate_left_common_factor();
    // 消除左递归
    void Eliminate_left_recur();
    // 生成fisrt集
    void create_first();
    // 找first
    set<string> find_first(string symbol);
    // 生成follow集
    void create_follow();
    // 找follow
    set<string> find_follow(string symbol);
    set<string> find_first_list(string symbol_list);

    // first和follow 计算预测分析表
    void predict_table();
    void print_predict_table();
    void print();

    // 是终结符
    bool is_terminal(string str);
    bool is_not_terminal(string str);
};

#endif