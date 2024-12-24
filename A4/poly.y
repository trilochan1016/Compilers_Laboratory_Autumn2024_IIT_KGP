%{
#include "poly.h"  // Include the necessary header (assuming you have it)
extern Node* root; // Root node of the parse tree
%}

%union {
    char str;
    Node* node;    // Tree node
}

%token <node> POW PLUS MINUS ONE ZERO VAR
%token <str> D
%type <node> S P T X N M


%start S

%%

// Grammar rules based on your given grammar

S : P          { $$ = createNode('S',0); addChild($$,$1); root = $$; } // Root set to P
  | PLUS P     { $$ = createNode('S',0); $1 = createNode('+',1); addChild($$,$1); addChild($$,$2); root = $$; } // +P
  | MINUS P    { $$ = createNode('S',0); $1 = createNode('-',1); addChild($$,$1); addChild($$,$2); root = $$; } // -P
  ;

P : T         { $$ = createNode('P',0); addChild($$, $1); }  // Polynomial is a term
  | T PLUS P  { $$ = createNode('P',0); $2 = createNode('+',1); addChild($$,$1); addChild($$,$2); addChild($$,$3); }  // T + P
  | T MINUS P { $$ = createNode('P',0); $2 = createNode('-',1); addChild($$,$1); addChild($$,$2); addChild($$,$3);  } // T - P
  ;

T : ONE      { $$ = createNode('T',0); $1 = createNode('1',1); addChild($$,$1);}        // Term is a constant 1
  | N        { $$ = createNode('T',0); addChild($$,$1); }  // Term is a number
  | X        { $$ = createNode('T',0); addChild($$,$1); }        // Term is x or x^N
  | N X      { $$ = createNode('T',0); addChild($$,$1); addChild($$,$2); }  // Term is N x^N
  ;

X : VAR       { $$ = createNode('X',0); $1 = createNode('x',1); addChild($$,$1);}  // Term is x
  | VAR POW N { $$ = createNode('X',0); $1 = createNode('x',1); $2 = createNode('^',1); addChild($$,$1); addChild($$,$2); addChild($$,$3); }  // Term is x^N
  ;

N : D       { $$ = createNode('N',0); Node* temp = createNode($1,1); addChild($$,temp); }  // Single digit number 
  | ONE M   { $$ = createNode('N',0); $1 = createNode('1',1); addChild($$,$1); addChild($$, $2); }  // Number starts with 1 and has more digits (1M)
  | D M     { $$ = createNode('N',0); Node *temp = createNode($1,1); addChild($$,temp); addChild($$, $2); }  // Number starts with D and has more digits (DM)
  ;

M : ZERO    { $$ = createNode('M',0); $1 = createNode('0',1); addChild($$,$1);}  // The digit 0
  | ONE     { $$ = createNode('M',0); $1 = createNode('1',1); addChild($$,$1);}  // The digit 1
  | D       { $$ = createNode('M',0); Node *temp = createNode($1,1); addChild($$,temp);}  // The digits 2-9
  | ZERO M  { $$ = createNode('M',0); $1 = createNode('0',1); addChild($$,$1); addChild($$, $2); }  // 0 followed by more digits (0M)
  | ONE M   { $$ = createNode('M',0); $1 = createNode('1',1); addChild($$,$1); addChild($$, $2); }  // 1 followed by more digits (1M)
  | D M     { $$ = createNode('M',0); Node *temp = createNode($1,1); addChild($$,temp); addChild($$, $2); }  // D followed by more digits (DM)
  ;

%%



