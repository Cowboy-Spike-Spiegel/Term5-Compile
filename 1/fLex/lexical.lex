%{
	int words = 0;
	int character_cnt = 0;
	int lines = 1;
	void charCnt_add(int n);
	void line_add(void);
%}




 # 正则表达式
digit			[0-9]
space			[ \t]
letter			[_A-Za-z]
lineComment		(\/\/([^\n]*(\\+\n)*)*\n)
comment			(\/\*(.*\n*)*\*\/)
string			(\"([^"\n]*(\\\n)*(\\\")*)*\")
id				{letter}({letter}|{digit})*
number			{digit}+(\.{digit}+)?([eE][+-]?{digit}+)?
keyword			"int"|"long"|"short"|"float"|"double"|"char"|"unsigned"|"signed"|"const"|"void"|"volatile"|"enum"|"struct"|"union"|"if"|"else"|"goto"|"switch"|"case"|"do"|"while"|"for"|"continue"|"break"|"return"|"default"|"typedef"|"auto"|"register"|"extern"|"static"|"sizeof"
operator		">"|">>"|">="|">>="|"<"|"<<"|"<="|"<<="|"!"|"!="|"="|"=="|"/"|"/="|"*"|"*="|"%"|"%="|"^"|"^="|"|"|"||"|"|="|"?"|"&"|"&&"|"&="|"+"|"+="|"++"|"-"|"-="|"--"	
delimiter		"("|")"|"{"|"}"|";"
char			(\'[^']*\')
illString		(\"([^"\n]*(\\\")*)*\n)
illNum			({digit}+(\.{digit}+)?([eE][+-]?[^0-9]))|({digit}+\.[^0-9])
macro			#.*\n



%%



{keyword} {
	printf("< %s , - >\n",yytext);
	charCnt_add(yyleng);
	words++ ;
}

{id} {
	printf("< id , %s >\n",yytext);
	charCnt_add(yyleng);
	words++;
}

{number} {
	printf("< num , %s >\n",yytext);
	charCnt_add(yyleng);
}

{operator} {
	printf("< operator , %s >\n",yytext);
	charCnt_add(yyleng);
}

{char} {
	printf("< char , %s >\n",yytext);
	charCnt_add(yyleng);
}

{string} {
	printf("< string , %s >\n",yytext);
	line_add();
        charCnt_add(yyleng);
}
	
{lineComment} {
	printf("< comment , %s >\n",yytext);
	line_add();
	charCnt_add(yyleng);
}

{comment} {
	line_add();
	printf("< comment , %s >\n",yytext);
	charCnt_add(yyleng);
}

{delimiter} {
	printf("< %s , - >\n",yytext);
        charCnt_add(yyleng);
}

{illNum} {
	printf("wrong numbers");
        printf("    wrong location: %d line\n",lines);
	line_add();
}
	
{illString} {
	printf("double quotes not match");
	printf("    wrong location: %d line\n",lines);
	charCnt_add(yyleng);
	line_add();
}

{space} {
	 charCnt_add(1);

}

{macro} {
	printf("< macro , %s >\n",yytext);
	lines++;
	charCnt_add(yyleng);

}

. {
	charCnt_add(1);
	printf("unknown character: %s",yytext);
	printf("    wrong location: %d line\n",lines);	
}

\n {
	charCnt_add(1);
	lines++;
}



%%



int main (void) {
	yylex();
	printf("characters count: %d\n",character_cnt );
	printf("total lines: %d\n",lines);
	printf("word count: %d\n",words);
	return 0;
}
int yywrap() {
	return 1;
}
void charCnt_add(int n) {
	character_cnt+=n;
}
void line_add(void) {
	for(int i=0;i<yyleng;i++)
		if(yytext[i]=='\n')
			lines++;
}