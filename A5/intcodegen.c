#include "lex.yy.c"
#include "y.tab.c"

void add_to_symboltable(char *id) {
    if (SymbolTable == NULL) {
        SymbolTable = malloc(sizeof(symboltable));
        SymbolTable->id = strdup(id);
        SymbolTable->offset = mem_offset++;
        SymbolTable->next = NULL;
    } else {
        symboltable *temp = SymbolTable;
        while (temp->next != NULL) {
            if (strcmp(temp->id, id) == 0) return;
            temp = temp->next;
        }
        if (strcmp(temp->id, id) == 0) return;
        temp->next = malloc(sizeof(symboltable));
        temp->next->id = strdup(id);
        temp->next->offset = mem_offset++;
        temp->next->next = NULL;
    }
}

int find_offset(char *id) {
    symboltable *temp = SymbolTable;
    while (temp != NULL) {
        if (strcmp(temp->id, id) == 0) return temp->offset;
        temp = temp->next;
    }
    return -1;  // Not found
}

int get_temp_reg() {
    if (temp_reg < 11) return temp_reg++;
    temp_reg = 2;  // Reset to 2 if we've used all temporary registers
    return temp_reg;
}

int main() {
    printf("#include <stdio.h>\n");
    printf("#include <stdlib.h>\n");
    printf("#include <math.h>\n");
    printf("#include \"auxi.c\"\n\n");
    printf("int main()\n{\n");
    printf("    int R[12];\n");
    printf("    int MEM[65536];\n\n");

    yyparse();

    printf("    return 0;\n");
    printf("}\n");


}