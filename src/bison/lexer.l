%{

#include "y.tab.h"
#include <stdio.h>


#undef YY_INPUT
#define YY_INPUT(b,r,s) readInputForLexer(b,&r,s)

%}

DIGIT   [0-9]
%%

\+      { printf("got plus\n"); return FUNCTION_PLUS; }
\-      { printf("got minus\n"); return FUNCTION_MINUS; }
{DIGIT}* { printf("got number\n"); return NUMBER; }
%%


void yyerror(char* s) {
    printf("error %s \n",s);
}

int yywrap() {
    return -1;
}