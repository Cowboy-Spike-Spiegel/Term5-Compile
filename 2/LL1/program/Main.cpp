#include <iostream>
#include "lex.h"
#include "LL1_Parser.h"
#include <string>

using namespace std;

vector<pair<string, string>> lex_outcome;

int main(int argc, char const* argv[])
{
    // 词法分析部分
    if (argc != 5) {
        cout << "Usage: LL1 [infile_name] [outfile_name] [errorfile_name] [gramfile_name]" << endl;
        exit(1);
    }

    Lex myLex(argv[1], argv[2], argv[3]);
    myLex.parser();     // 执行语法分析
    myLex.print_info(); // 输出结果再terminal和outFile

    // 语义分析部分
    Parser myParser(argv[4]);

    return 0;
}