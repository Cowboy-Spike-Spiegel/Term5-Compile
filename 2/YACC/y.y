%{
	#include<stdio.h>
	extern int yylex();
	extern int yyparse();
%}

%token number

%%
line : E'\n'	{printf("Success\n");return;}
;
E : E'+'T	{printf("Use : E->E+T\n");}
    | E'-'T	{printf("Use : E->E-T\n");}
    | T		{printf("Use : E->T\n");}
    ;
T : T'*'F	{printf("Use : T->T*F\n");}
    | T'/'F	{printf("Use : T->T/F\n");}
    | F		{printf("Use : T->F\n");}
    ;
F : '('E')'     {printf("Use : F->(E)\n");}
    | number    {printf("Use : F->number\n");}
    ;
    
%%     

int main(){
    yyparse();
    return 0;
}
void yyerror(char *s)
{
    printf("%s\n",s);
    system("pause");
}
