#ifndef POLY_H
#define POLY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_CHILDREN 3

typedef struct Node {
    char type;
    int val;
    char *inh;
    int is_terminal;
    struct Node* parent;
    struct Node* children[MAX_CHILDREN];
    int num_children;
} Node;

Node* createNode(char type, int terminal);
void addChild(Node* parent, Node* child);
// void setatt(Node* root);
// void printTree(Node* root, int depth);
// int evalpoly(Node* root, int x);
// void printderivative(Node* root);
// void freeTree(Node* root);

extern int yylex();
extern int yyparse();
extern void yyerror(const char* s);

#endif // POLY_H