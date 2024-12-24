%{
#include "argsa.h"
#include "y.tab.h"
int yylex();
void yyerror(char *s);
%}

%%
"set"                    { yylval.keyword = SET; return SET; }
[-]?[0-9]+              { yylval.value = atoi(yytext); return NUM; }
[a-zA-Z_][a-zA-Z0-9_]*   { yylval.id = strdup(yytext); return ID; }
"+"                      { yylval.opcode = ADD; return ADD; }
"-"                      { yylval.opcode = SUB; return SUB; }
"*"                      { yylval.opcode = MUL; return MUL; }
"/"                      { yylval.opcode = DIV; return DIV; }
"%"                      { yylval.opcode = MOD; return MOD; }
"**"                     { yylval.opcode = EXP; return EXP; }
"("                      { yylval.punc = LP; return LP; }
")"                      { yylval.punc = RP; return RP; }
[ \t\n]+                 { }
.                        { printf("Unexpected character: %s\n", yytext); exit(1); }
%%

int yywrap(){
    return 1;
}
