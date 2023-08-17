#include <iostream>
#include "lex.h"

using namespace std;

int main(int argc, char const* argv[])
{
    //生成可执行文件的时候，往里面添加
    if (argc != 4) {
        cout << "Usage: Lex_Analysis [infile_name] [outfile_name] [errorfile_name]" << endl;
        exit(1);
    }

    Lex myLex(argv[1], argv[2], argv[3]);
    myLex.parser();     // 执行语法分析
    myLex.print_info(); // 输出结果再terminal和outFile

    return 0;
}