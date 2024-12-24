%{
#include "poly.h"
#include "y.tab.h"
%}

%%
"2"         {  yylval.str = '2'; return D; }
"3"         {  yylval.str = '3'; return D; }
"4"         {  yylval.str = '4'; return D; }
"5"         {  yylval.str = '5'; return D; }
"6"         {  yylval.str = '6'; return D; }
"7"         {  yylval.str = '7'; return D; }
"8"         {  yylval.str = '8'; return D; }
"9"         {  yylval.str = '9'; return D; }
"0"           {  return ZERO;}
"1"           {  return ONE; }
"x"           {  return VAR; }
"^"           {  return POW; }
"+"           {  return PLUS; }
"-"           {  return MINUS; }
[ \t\n]+      { /* Skip whitespace */ }
.             { printf("Unknown character: %s\n", yytext); }
%%

int yywrap() {
    return 1;
}