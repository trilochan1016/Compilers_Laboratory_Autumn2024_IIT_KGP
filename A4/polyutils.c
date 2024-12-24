#include "poly.h"
#include <ctype.h>

Node* root = NULL;

// Error handling
void yyerror(const char* s) {
    fprintf(stderr, "Error: %s\n", s);
}

Node* createNode(char type,int is_terminal){
    Node *newnode  = (Node *)malloc(sizeof(Node));
    newnode->type = type;
    newnode->is_terminal = is_terminal;
    newnode->parent = NULL;
    newnode->num_children = 0;
    newnode->children[0] = NULL;
    newnode->children[1] = NULL;
    newnode->children[2] = NULL;
    newnode->val = 0;
    newnode->inh = strdup("");
    return newnode;
}
    
void addChild(Node* parent, Node* child) {
    if (parent->num_children < MAX_CHILDREN) {
        parent->children[parent->num_children++] = child;
        child->parent = parent;
    }
}

void printParseTree(Node* node, int depth) {
    if (node == NULL) return;

    // Print indentation based on the depth of the node
    for (int i = 0; i < depth; i++) {
        printf("  ");  // Indentation for each level
    }

    // Print the current node's details
    printf("==> %c [ val = %d , inh =  %s]\n", node->type, node->val, node->inh);

    // Recursively print the children
    for (int i = 0; i < node->num_children; i++) {
        printParseTree(node->children[i], depth + 1);  // Increase depth for child nodes
    }
}

Node* setatt(Node* root) {
    if (root == NULL) return NULL;

    if(root->type == 'S' && root->num_children == 1){
        root->children[0]->inh = strdup("+");
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }
    
    else if(root->type == 'S' && root->num_children == 2 && root->children[0]->type == '-'){
        root->children[1]->inh = strdup("-");
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }

    else if(root->type == 'S' && root->num_children == 2 && root->children[0]->type == '+'){
        root->children[1]->inh = strdup("+");
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }

    else if(root->type == 'P' && root->num_children == 1){
        root->children[0]->inh = strdup(root->inh);
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }

    else if(root->type == 'P' && root->num_children == 3 && root->children[1]->type == '+'){
        root->children[0]->inh = strdup(root->inh);
        root->children[2]->inh = strdup("+");
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }

    else if(root->type == 'P' && root->num_children == 3 && root->children[1]->type == '-'){
        root->children[0]->inh = strdup(root->inh);
        root->children[2]->inh = strdup("-");
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }

    else if(root->type == 'T' && root->num_children == 1 && root->children[0]->type == '1'){
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }

    else if(root->type == 'T' && root->num_children == 1 && root->children[0]->type == 'N'){
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }

    else if(root->type == 'T' && root->num_children == 1 && root->children[0]->type == 'X'){
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }

    else if(root->type == 'T' && root->num_children == 2){
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }

    else if(root->type == 'X' && root->num_children == 1){
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }

    else if(root->type == 'X' && root->num_children == 3){
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
    }

    else if(root->type == 'N' && root->num_children == 1){
        for (int i = 0; i < root->num_children; i++) {
            root->children[i]=setatt(root->children[i]);
        }
        root->val = root->children[0]->val;
    }

    else if(root->type == 'N' && root->num_children == 2 && root->children[0]->type == '1'){
        root->children[0]=setatt(root->children[0]);
        char temp = (char)(48+root->children[0]->val);
        char* v=(char*)malloc(sizeof(char)*2);
        v[0]=temp;
        root->children[1]->inh = v;
        root->children[1]=setatt(root->children[1]);
        root->val = root->children[1]->val;
    }

    else if(root->type == 'N' && root->num_children == 2 && (int)(root->children[0]->type) >= 50 && (int)(root->children[0]->type) <= 57){
        root->children[0]=setatt(root->children[0]);
        char temp = (char)(48+root->children[0]->val);
        char* v=(char*)malloc(sizeof(char)*2);
        v[0]=temp;
        root->children[1]->inh = v;
        root->children[1]=setatt(root->children[1]);
        root->val = root->children[1]->val;
    }

    else if(root->type == 'M' && root->num_children == 1){
        root->children[0]=setatt(root->children[0]);
        root->val = atoi(root->inh)*10 + root->children[0]->val;
    }

    else if(root->type == 'M' && root->num_children == 2){
        root->children[0]=setatt(root->children[0]);
        root->children[1]->inh = (char *)malloc((2+strlen(root->inh))*sizeof(char));
        strcpy(root->children[1]->inh,root->inh);
        char temp = (char)(48+root->children[0]->val);
        char* v=(char*)malloc(sizeof(char)*2);
        v[0]=temp;
        strcat(root->children[1]->inh,v);
        root->children[1]=setatt(root->children[1]);
        root->val = root->children[1]->val;
    }

    if((int)(root->type) >= 48 && (int)(root->type) <= 57){
        root->val = (int)(root->type) - 48;
    }
    else{
        return root;
    }    
}

int evalpoly(Node* root, int x) {
    if (root == NULL) return 0;
    if(root->type == 'S'){
        if(root->num_children==1){
            return evalpoly(root->children[0],x);
        }
        else{
            return evalpoly(root->children[1],x);
        }
    }
    else if(root->type == 'P'){
        if(root->num_children == 1){
            return evalpoly(root->children[0],x);
        }
        else{
            return (evalpoly(root->children[0],x)+evalpoly(root->children[2],x));
        }
    }
    else if(root->type == 'T'){
        int temp;
        if(root->num_children == 1){
            if(root->children[0]->type == '1'){
                temp=1;
            }
            else{
                temp=evalpoly(root->children[0],x);
            }
        }
        else{
            temp=evalpoly(root->children[0],x)*evalpoly(root->children[1],x);
        }
        if(strcmp(root->inh,"-")==0){
            temp=-temp;
        }
        return temp;
    }
    else if(root->type == 'X'){
        if(root->num_children == 1){
            return x;
        }
        else{
            return pow(x,evalpoly(root->children[2],x));
        }
    }
    else if(root->type == 'N'){
        return root->val;
    }
}

void newprint(Node *root,int coefficient){
    if(root->num_children == 1){
        printf(" %d ",coefficient);
    }
    else{
        int n = root->children[2]->val;
        int ne = coefficient*n;
        printf(" %dx^%d ",ne,(n-1));
    }
}

void printderivative(Node *root){
    if(root->type == 'S'){
        if(root->num_children==1){
            printderivative(root->children[0]);
        }
        else{
            printderivative(root->children[1]);
        }
    }
    else if(root->type == 'P'){
        if(root->num_children == 1){
            printderivative(root->children[0]);
        }
        else{
            printderivative(root->children[0]);
            printderivative(root->children[2]);
        }
    }
    else if(root->type == 'T'){
        
        if(root->num_children == 1 && root->children[0]->type == 'X'){
            if(strcmp(root->inh,"+")==0){
            printf(" + ");
        }
        else{
            printf(" - ");
        }
            newprint(root->children[0],1);
        }
        if(root->num_children == 2){
            if(strcmp(root->inh,"+")==0){
            printf(" + ");
            }
            else{
            printf(" - ");
            }
            newprint(root->children[1],root->children[0]->val);
        }

    }

}

int main() {
    if (yyparse() != 0) {
        fprintf(stderr, "Error: Failed to parse input\n");
        return 1;
    }

    if (root == NULL) {
        fprintf(stderr, "Error: Failed to build parse tree\n");
        return 1;
    }
    root=setatt(root);
    printParseTree(root,0);
    printf("\n\n");
    for(int i=-5;i<=5;i++){
        printf("+++ f(%d)  =  %d\n",i,evalpoly(root,i));
    }
    printf("\n\n");
    printf("+++ f'(x) =  ");
    printderivative(root);
    printf("\n");
    return 0;
}