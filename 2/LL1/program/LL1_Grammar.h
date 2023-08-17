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
    // ������������ʽ����
    map<string, vector<string>> expressions;
    // �ս������
    set<string> term;
    // ���ս������
    set<string> not_term;
    // ��ʼ����
    string start;

    // ���ս����First��
    map<string, set<string>> First;
    // ���ս����Follow��
    map<string, set<string>> Follow;

    // Ԥ�������
    map<vector<string>, string> Table;

    Grammar(string file_name);
    Grammar() {};

    // �ָ��ַ���
    vector<string> split(const string& str, const string& splits);

private:
    // �ļ��������ս�����ս������ʼ���ţ����ʽ
    void init(string file_name);
    
    // ����������
    void Eliminate_left_common_factor();
    // ������ݹ�
    void Eliminate_left_recur();
    // ����fisrt��
    void create_first();
    // ��first
    set<string> find_first(string symbol);
    // ����follow��
    void create_follow();
    // ��follow
    set<string> find_follow(string symbol);
    set<string> find_first_list(string symbol_list);

    // first��follow ����Ԥ�������
    void predict_table();
    void print_predict_table();
    void print();

    // ���ս��
    bool is_terminal(string str);
    bool is_not_terminal(string str);
};

#endif