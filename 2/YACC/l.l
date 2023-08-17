%{
    #include<stdio.h>
    #include "y.tab.h"
%}

digit   ([0-9])
number  ({digit}+)

%%

{number}   {
    return number;
}

\+   {
    return '+';
}
\-   {
    return '-';
}
\*   {
    return '*';
}
\/   {
    return '/';
}
\(   {
 
    return '(';
}
\)   {

    return ')';
}
\n  {
    return '\n';
}
.   {
    printf("Don't exist:%s\n",yytext);
    return;
}

%%

