%{
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex();

typedef struct SymbolTableEntry {
    char *name;
    int temp_reg;
} SymbolTableEntry;

SymbolTableEntry *symbolTable[1001];
int symbolTableSize = 1;

typedef struct Quad {
    int op;
    char *result;
    char *arg1;
    char *arg2;
} Quad;

Quad *quads[1001];
int quadCount = 1;

typedef struct RegisterDesc{
     int free;
     char *var;
     int dirty;  // Does register value need to be stored?
}RegisterDesc;

RegisterDesc registers[5];  // Default 5 registers R1-R5

typedef struct Instruction {
    char *op;          // Operation (LD, ST, ADD, etc.)
    char *reg;         // Target register
    char *arg1;        // First argument 
    char *arg2;        // Second argument
} Instruction;

Instruction *targetCode[1001];
int targetCount = 1;

SymbolTableEntry *lookupSymbol(char *name);
void addSymbol(char *name, int temp_reg);
void emitQuad(int op, char *result, char *arg1, char *arg2);
void backpatch(int cnt,int size);
int temp = 0;

%}

%start list
%union {   
        int value; 
        char *id;
        int opcode;
        int punc;
        int keyword;
        int relop;
        int ret;
        struct argsa {
          int is_type;
          int value;
          char *ide;
        }inti_num_expr;
}
%token<punc> LP RP 
%token<id> IDEN 
%token<value> NUM 
%token<keyword> SET LOOP WHEN WHILE
%token<opcode> ADD SUB MUL DIV MOD
%token<relop> EQUAL NOTEQUAL LT GT LE GE  
%type<relop> reln 
%type<opcode> oper
%type<inti_num_expr> atom  
%type<id> expr
%type stmt asgn cond loop boolean
%type<ret> M

%%

list : stmt | stmt list ;

stmt : asgn | cond | loop ;

asgn : LP SET IDEN atom RP {
     if($4.is_type!=2){
          addSymbol($3,0);
          emitQuad(SET,$3,$4.ide,NULL);
     }
     else{
          char *int_to_str = (char*)malloc(sizeof(char)*12);
          sprintf(int_to_str,"%d",temp);
          int len = strlen(int_to_str);
          int t = 2+len;
          char *result = (char*)malloc(sizeof(char)*(t));
          result[0] = '$';
          for(int i=0;i<len;i++){
               result[i+1] = int_to_str[i];
          }
          emitQuad(SET,$3,result,NULL);
     }  
}
     ;

cond : LP WHEN boolean M list RP{
     backpatch($4,quadCount);
}
     ;

M    :  {   $$ = quadCount - 1;
}
     ;

loop : LP M LOOP WHILE  boolean M list RP{
     char *temp_1 = (char *)malloc(sizeof(char)*12);
     sprintf(temp_1,"goto %d",($2)+1);
     emitQuad(LOOP,NULL,NULL,temp_1);
     backpatch($6,quadCount);
}
     ;

expr : LP oper atom atom RP{
     if($3.is_type!=2 && $4.is_type!=2){
          temp++;
          char *int_to_str = (char*)malloc(sizeof(char)*12);
          sprintf(int_to_str,"%d",temp);
          int len = strlen(int_to_str);
          int t = 2+len;
          char *result = (char*)malloc(sizeof(char)*(t));
          result[0] = '$';
          for(int i=0;i<len;i++){
               result[i+1] = int_to_str[i];
          }
          addSymbol(result,0);
          emitQuad($2,result,$3.ide,$4.ide);
          $$ = strdup(result);

     }
     else if($3.is_type==2 && $4.is_type==2){
          temp++;
          char *int_to_str = (char*)malloc(sizeof(char)*12);
          sprintf(int_to_str,"%d",temp);
          int len = strlen(int_to_str);
          int t = 2+len;
          char *result = (char*)malloc(sizeof(char)*(t));
          result[0] = '$';
          for(int i=0;i<len;i++){
               result[i+1] = int_to_str[i];
          }
          addSymbol(result,0);
          char *int_to_str1 = (char*)malloc(sizeof(char)*12);
          sprintf(int_to_str1,"%d",temp-1);
          int len1 = strlen(int_to_str1);
          int t1 = 2+len1;
          char *result1 = (char*)malloc(sizeof(char)*(t1));
          result1[0] = '$';
          for(int i=0;i<len1;i++){
               result1[i+1] = int_to_str1[i];
          }

          char *int_to_str2 = (char*)malloc(sizeof(char)*12);
          sprintf(int_to_str2,"%d",temp-2);
          int len2 = strlen(int_to_str2);
          int t2 = 2+len2;
          char *result2 = (char*)malloc(sizeof(char)*(t2));
          result2[0] = '$';
          for(int i=0;i<len2;i++){
               result2[i+1] = int_to_str2[i];
          }

          emitQuad($2,result,result1,result2);
          $$ = strdup(result);
     }
     else if($3.is_type==2 && $4.is_type!=2){
          char *int_to_str = (char*)malloc(sizeof(char)*12);
          sprintf(int_to_str,"%d",temp);
          int len = strlen(int_to_str);
          int t = 2+len;
          char *result = (char*)malloc(sizeof(char)*(t));
          result[0] = '$';
          for(int i=0;i<len;i++){
               result[i+1] = int_to_str[i];
          }

          temp++;
          char *int_to_str1 = (char*)malloc(sizeof(char)*12);
          sprintf(int_to_str1,"%d",temp);
          int len1 = strlen(int_to_str);
          int t1 = 2+len1;
          char *result1 = (char*)malloc(sizeof(char)*(t1));
          result1[0] = '$';
          for(int i=0;i<len1;i++){
               result1[i+1] = int_to_str1[i];
          }
          addSymbol(result1,0);
          emitQuad($2,result1,result,$4.ide);
          $$ = strdup(result1);
     }
     else{
          char *int_to_str = (char*)malloc(sizeof(char)*12);
          sprintf(int_to_str,"%d",temp);
          int len = strlen(int_to_str);
          int t = 2+len;
          char *result = (char*)malloc(sizeof(char)*(t));
          result[0] = '$';
          for(int i=0;i<len;i++){
               result[i+1] = int_to_str[i];
          }

          temp++;
          char *int_to_str1 = (char*)malloc(sizeof(char)*12);
          sprintf(int_to_str1,"%d",temp);
          int len1 = strlen(int_to_str);
          int t1 = 2+len1;
          char *result1 = (char*)malloc(sizeof(char)*(t1));
          result1[0] = '$';
          for(int i=0;i<len1;i++){
               result1[i+1] = int_to_str1[i];
          }
          addSymbol(result1,0);
          emitQuad($2,result1,$4.ide,result);
          $$ = strdup(result1);
     }
}
     ;

boolean : LP reln atom atom RP{
        switch($2){
          case EQUAL: 
               emitQuad(EQUAL,NULL,$3.ide,$4.ide);
               break;
          case NOTEQUAL: 
               emitQuad(NOTEQUAL,NULL,$3.ide,$4.ide);
               break;
          case LT: 
               emitQuad(LT,NULL,$3.ide,$4.ide); 
               break;
          case GT: 
               emitQuad(GT,NULL,$3.ide,$4.ide);
               break;
          case LE: 
               emitQuad(LE,NULL,$3.ide,$4.ide);
               break;
          case GE: 
               emitQuad(GE,NULL,$3.ide,$4.ide);
               break;
     }
        }

        ;

atom : IDEN{
     $$.is_type = 0;
     $$.value = 0;
     $$.ide = (char*)malloc(sizeof(char)*12);
     $$.ide = $1;
     addSymbol($1,0);
}
          
     | NUM{
     $$.is_type = 1;
     $$.value = $1;
     char *int_to_str = (char*)malloc(sizeof(char)*12);
     sprintf(int_to_str,"%d",$1);
     int len = strlen(int_to_str);
     int t = 1+len;
     $$.ide = (char*)malloc(sizeof(char)*(t));
     for(int i=0;i<len;i++){
          $$.ide[i] = int_to_str[i];
     }
}
     | expr{
     $$.is_type = 2;
     $$.value = 0;
     $$.ide = strdup($1);
}
     ;

oper : ADD | SUB | MUL | DIV | MOD ;

reln : EQUAL | NOTEQUAL | LT | GT | LE | GE ;

%%

void yyerror(char *msg) { 
    fprintf(stderr, "Error: %s\n", msg); 
}