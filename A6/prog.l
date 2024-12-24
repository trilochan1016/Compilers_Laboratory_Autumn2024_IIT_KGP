%{
#include"y.tab.h"
#define JMP 100
int yylex();
void yyerror(char *msg);
%}

%%

"set"                       { yylval.keyword = SET; return SET;}
"when"                      { yylval.keyword = WHEN; return WHEN;}
"loop"                      { yylval.keyword = LOOP; return LOOP;}
"while"                     { yylval.keyword = WHILE; return WHILE;}
[-]?[0-9]+                  { yylval.value = atoi(yytext); return NUM; }
[a-zA-Z_][a-zA-Z0-9_]*      { yylval.id = strdup(yytext); return IDEN; }
"+"                         { yylval.opcode = ADD; return ADD; }
"-"                         { yylval.opcode = SUB; return SUB; }
"*"                         { yylval.opcode = MUL; return MUL; }
"/"                         { yylval.opcode = DIV; return DIV; }
"%"                         { yylval.opcode = MOD; return MOD; }
"("                         { yylval.punc = LP; return LP; }
")"                         { yylval.punc = RP; return RP; }
"="                         { yylval.relop = EQUAL; return EQUAL; }
"/="                        { yylval.relop = NOTEQUAL; return NOTEQUAL;}
">"                         { yylval.relop = GT; return GT;}
"<"                         { yylval.relop = LT; return LT;}
">="                        { yylval.relop = GE; return GE;}
"<="                        { yylval.relop = LE; return LE;}
[ \t\n]+                    {}


%%

int yywrap(){
    return 1;
}
