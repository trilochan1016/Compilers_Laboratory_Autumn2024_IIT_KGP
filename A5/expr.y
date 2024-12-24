%{
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex();

typedef struct _symboltable {
    char *id;
    int offset;
    struct _symboltable *next;
} symboltable;

symboltable *SymbolTable = NULL;
int mem_offset = 0;
int temp_reg = 1;  
int reg = 0;   // for r[0] and r[1]

void add_to_symboltable(char *id);
int find_offset(char *id);
int get_temp_reg();

%}



%start program
%union { int opcode; 
         int keyword; 
         int punc; 
         int value; 
         char *id;
         struct argsa inti_num_expr;}
%token<opcode> ADD SUB MUL DIV MOD EXP
%token<punc> LP RP
%token<keyword> SET
%token<value> NUM
%token<id> ID
%type<opcode> op
%type expr exprstmt stmt setstmt program
%type<inti_num_expr> arg

%%

program : stmt program   { temp_reg = 1; reg = 0;}
        | stmt            { temp_reg = 1; reg = 0;}
        ;

stmt : setstmt      { temp_reg = 1; reg = 0;}
     | exprstmt      { temp_reg = 1; reg = 0;}
     ;

setstmt : LP SET ID NUM RP  {
    add_to_symboltable($3);
    int offset = find_offset($3);
    printf("    MEM[%d] = %d;\n", offset, $4);
    printf("    mprn(MEM, %d);\n", offset);
}
        | LP SET ID ID RP   {
    add_to_symboltable($3);
    add_to_symboltable($4);
    int offset1 = find_offset($3);
    int offset2 = find_offset($4);
    printf("    R[0] = MEM[%d];\n", offset2);
    printf("    MEM[%d] = R[0];\n", offset1);
    printf("    mprn(MEM, %d);\n", offset1);
}
        | LP SET ID expr RP {
    add_to_symboltable($3);
    int offset = find_offset($3);
    printf("    MEM[%d] = R[2];\n", offset);
    printf("    mprn(MEM, %d);\n", offset);
    
}
        ;

exprstmt : expr {
    printf("    eprn(R, 2);\n");
}
        ;

expr : LP op arg arg RP {
    char *op_str = $2 == ADD ? "+" : $2 == SUB ? "-" : $2 == MUL ? "*" : $2 == DIV ? "/" : $2 == MOD ? "%" : "pwr";
    if ($2 == EXP ) {
        if($3.is_type==0 && $4.is_type==0){
            temp_reg++;
            int offset1 = find_offset($3.ide);
            int offset2 = find_offset($4.ide);
            printf("    R[0] = MEM[%d];\n", offset1);
            printf("    R[1] = MEM[%d];\n", offset2);
            printf("    R[%d] = %s(R[0],R[1]);\n", temp_reg, op_str);
        }
        else if($3.is_type==1 && $4.is_type==1){
            temp_reg++;
            printf("    R[%d] = %s(%d,%d);\n", temp_reg, op_str, $3.value,  $4.value);
        }
        else if($3.is_type==0 && $4.is_type==1){
            temp_reg++;
            int offset = find_offset($3.ide);
            printf("    R[%d] = MEM[%d];\n", reg, offset);
            if(reg == 0){
                reg = 1;
            }
            else{
                reg = 0;
            }
            int reg_tmp;
            if(reg==0){
                reg_tmp = 1;
            }
            else{
                reg_tmp = 0;
            }
            printf("    R[%d] = %s(R[%d],%d);\n", temp_reg, op_str, reg_tmp, $4.value);
        }
        else if($3.is_type==1 && $4.is_type==0){
            temp_reg++;
            int offset = find_offset($4.ide);
            printf("    R[%d] = MEM[%d];\n", reg, offset);
            if(reg == 0){
                reg = 1;
            }
            else{
                reg = 0;
            }
            int reg_tmp;
            if(reg==0){
                reg_tmp = 1;
            }
            else{
                reg_tmp = 0;
            }
            printf("    R[%d] = %s(%d,R[%d]);\n", temp_reg, op_str, $3.value, reg_tmp);
        }
        else if($3.is_type==2 && $4.is_type==2){
            printf("    R[%d] = %s(R[%d],R[%d]);\n", temp_reg-1, op_str, temp_reg-1,  temp_reg);
            temp_reg--;
        }
        else if($3.is_type==2 && $4.is_type==0){
            int offset = find_offset($4.ide);
            printf("    R[%d] = MEM[%d];\n", reg, offset);
            if(reg == 0){
                reg = 1;
            }
            else{
                reg = 0;
            }
            int reg_tmp;
            if(reg==0){
                reg_tmp = 1;
            }
            else{
                reg_tmp = 0;
            }
            printf("    R[%d] = %s(R[%d],R[%d]);\n", temp_reg, op_str, temp_reg, reg_tmp);
        }
        else if($3.is_type==0 && $4.is_type==2){
            int offset = find_offset($3.ide);
            printf("    R[%d] = MEM[%d];\n", reg, offset);
            if(reg == 0){
                reg = 1;
            }
            else{
                reg = 0;
            }
            int reg_tmp;
            if(reg==0){
                reg_tmp = 1;
            }
            else{
                reg_tmp = 0;
            }
            printf("    R[%d] = %s(R[%d],R[%d]);\n", temp_reg, op_str, reg_tmp, temp_reg);
            
        }
        else if($3.is_type==2 && $4.is_type==1){
            printf("    R[%d] = %s(R[%d],%d);\n", temp_reg, op_str, temp_reg, $4.value);
            
        }
        else if($3.is_type==1 && $4.is_type==2){
            printf("    R[%d] = %s(%d,R[%d]);\n", temp_reg, op_str, $3.value, temp_reg);
        }    
    } else {
        if($3.is_type==0 && $4.is_type==0){
            temp_reg++;
            int offset1 = find_offset($3.ide);
            int offset2 = find_offset($4.ide);
            printf("    R[0] = MEM[%d];\n", offset1);
            printf("    R[1] = MEM[%d];\n", offset2);
            printf("    R[%d] = R[0] %s R[1];\n", temp_reg, op_str);
        }
        else if($3.is_type==1 && $4.is_type==1){
            temp_reg++;
            printf("    R[%d] = %d %s %d;\n", temp_reg, $3.value, op_str, $4.value);
        }
        else if($3.is_type==0 && $4.is_type==1){
            temp_reg++;
            int offset = find_offset($3.ide);
            printf("    R[%d] = MEM[%d];\n", reg, offset);
            if(reg == 0){
                reg = 1;
            }
            else{
                reg = 0;
            }
            int reg_tmp;
            if(reg==0){
                reg_tmp = 1;
            }
            else{
                reg_tmp = 0;
            }
            printf("    R[%d] = R[%d] %s %d;\n", temp_reg, reg_tmp, op_str, $4.value);
        }
        else if($3.is_type==1 && $4.is_type==0){
            temp_reg++;
            int offset = find_offset($4.ide);
            printf("    R[%d] = MEM[%d];\n", reg, offset);
            if(reg == 0){
                reg = 1;
            }
            else{
                reg = 0;
            }
            int reg_tmp;
            if(reg==0){
                reg_tmp = 1;
            }
            else{
                reg_tmp = 0;
            }
            printf("    R[%d] = %d %s R[%d];\n", temp_reg, $3.value, op_str, reg_tmp);
        }
        else if($3.is_type==2 && $4.is_type==2){
            printf("    R[%d] = R[%d] %s R[%d];\n", temp_reg-1, temp_reg-1, op_str, temp_reg);
            temp_reg--;
        }
        else if($3.is_type==2 && $4.is_type==0){
            int offset = find_offset($4.ide);
            printf("    R[%d] = MEM[%d];\n", reg, offset);
            if(reg == 0){
                reg = 1;
            }
            else{
                reg = 0;
            }
            int reg_tmp;
            if(reg==0){
                reg_tmp = 1;
            }
            else{
                reg_tmp = 0;
            }
            printf("    R[%d] = R[%d] %s R[%d];\n", temp_reg, temp_reg, op_str, reg_tmp);
        }
        else if($3.is_type==0 && $4.is_type==2){
            int offset = find_offset($3.ide);
            printf("    R[%d] = MEM[%d];\n", reg, offset);
            if(reg == 0){
                reg = 1;
            }
            else{
                reg = 0;
            }
            int reg_tmp;
            if(reg==0){
                reg_tmp = 1;
            }
            else{
                reg_tmp = 0;
            }
            printf("    R[%d] = R[%d] %s R[%d];\n", temp_reg, reg_tmp, op_str, temp_reg);
            
        }
        else if($3.is_type==2 && $4.is_type==1){
            printf("    R[%d] = R[%d] %s %d;\n", temp_reg, temp_reg, op_str, $4.value);
            
        }
        else if($3.is_type==1 && $4.is_type==2){
            printf("    R[%d] = %d %s R[%d];\n", temp_reg, $3.value, op_str, temp_reg);
        }
    }
}
     ;

op : ADD | SUB | MUL | DIV | MOD | EXP ;

arg : ID {
    // int offset = find_offset($1);
    // printf("    R[%d] = MEM[%d];\n", reg, offset);
    // if(reg == 0){
    //     reg = 1;
    // }
    // else{
    //     reg = 0;
    // }
    $$.is_type = 0;
    $$.value = 0;
    $$.ide = (char*)malloc(sizeof(char)*10);
    $$.ide = $1;
}
    | NUM {
    $$.is_type = 1;
    $$.value = $1;
}
    | expr{
        $$.is_type = 2;
        $$.value = 0;
    }
    ;

%%

void yyerror(char *msg) { fprintf(stderr, "Error: %s\n", msg); }

