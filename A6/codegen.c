#include"lex.yy.c"
#include"y.tab.c"

int blk1[1001];
int blk2[1001];

SymbolTableEntry *lookupSymbol(char *name) {
    for (int i = 1; i < symbolTableSize; i++) {
        if (strcmp(symbolTable[i]->name, name) == 0) {
            return symbolTable[i];
        }
    }
    return NULL;
}

void addSymbol(char *name, int temp_reg) {
    for (int i = 1; i < symbolTableSize; i++) {
        if (strcmp(symbolTable[i]->name, name) == 0){
            symbolTable[i]->temp_reg = temp_reg;
            return;
        }
    }
    SymbolTableEntry *entry = (SymbolTableEntry *)malloc(sizeof(SymbolTableEntry));
    entry->name = strdup(name);
    entry->temp_reg = temp_reg;
    symbolTable[symbolTableSize++] = entry;
    return;
}

void emitQuad(int op, char *res, char *a1, char *a2) {
    Quad *q = (Quad *)malloc(sizeof(Quad));
    q->op = op;
    q->result = res;
    q->arg1 = a1;
    q->arg2 = a2;
    quads[quadCount++] = q;
}

void backpatch(int cnt,int size){
    char *tmp = (char*)malloc(sizeof(char)*12);
    sprintf(tmp,"goto %d",size);
    quads[cnt]->result = strdup(tmp);
}

int isLeader(int index){
    // First instruction is always a leader
    if (index == 1) {
        return 1;
    }

    Quad *curr = quads[index];
    Quad *prev = quads[index-1];
    
    // Target of a jump instruction is a leader
    if (prev->op == JMP || prev->op == LOOP || prev->op == EQUAL || prev->op == NOTEQUAL || prev->op == LT || prev->op == GT || prev->op == LE || prev->op == GE){
        return 1;
    }
    
    // Check if current instruction is a target of any jump
    for (int i = 1; i < quadCount; i++) {
        if (quads[i]->op == JMP || quads[i]->op == LOOP) {
            char expectedGoto[12];
            sprintf(expectedGoto, "goto %d", index);
            if (quads[i]->arg2 && strcmp(quads[i]->arg2, expectedGoto) == 0) {
                return 1;
            }
        }
        else if (quads[i]->op == EQUAL || quads[i]->op == NOTEQUAL ||
                 quads[i]->op == LT || quads[i]->op == GT ||
                 quads[i]->op == LE || quads[i]->op == GE) {
            char expectedGoto[12];
            sprintf(expectedGoto, "goto %d", index);
            if (quads[i]->result && strcmp(quads[i]->result, expectedGoto) == 0) {
                return 1;
            }
        }
    }
    
    return 0;
}

// Helper function to check if string is an integer
int isInteger(char* str) {
    // Handle negative numbers
    if(str[0] == '-') str++;
    // Check if all remaining characters are digits
    while(*str) {
        if(!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

// Initialize register descriptors
void initRegisters() {
    for(int i = 0; i < 5; i++) {
        registers[i].free = 1;
        registers[i].var = NULL;
        registers[i].dirty = 0;
    }
}

// Helper to get register number as string
char* getRegString(int regNum) {
    char* reg = (char*)malloc(3);
    sprintf(reg, "R%d", regNum + 1);
    return reg;
}

// Find register containing variable/temporary
int findRegister(char *var) {
    for(int i = 0; i < 5; i++) {
        if(registers[i].var && strcmp(registers[i].var, var) == 0) {
            return i;
        }
    }
    return -1;
}

// Get free register or allocate based on algorithm
int getReg(char *var, int isTarget) {
    // First check if already in a register
    int reg = findRegister(var);
    if(reg != -1) return reg;
    
    // Find a free register
    for(int i = 0; i < 5; i++) {
        if(registers[i].free) {
            registers[i].free = 0;
            registers[i].var = strdup(var);
            registers[i].dirty = 0;
            return i;
        }
    }

    // Need to spill a register
    // First try to find one that's not dirty
    for(int i = 0; i < 5; i++) {
        if(!registers[i].dirty) {
            if(!isTarget && !(isdigit(var[0]) || var[0] == '-')) {
                Instruction *loadInstr = malloc(sizeof(Instruction));
                loadInstr->op = strdup("LD");
                loadInstr->reg = getRegString(i);
                loadInstr->arg1 = strdup(var);
                loadInstr->arg2 = NULL;
                targetCode[targetCount++] = loadInstr;
            }
            registers[i].var = strdup(var);
            registers[i].dirty = 0;
            return i;
        }
    }
    
    // All registers are dirty, spill the first one
    int spillReg = 0;
    if(registers[spillReg].dirty && registers[spillReg].var[0] != '$') {
        Instruction *storeInstr = malloc(sizeof(Instruction));
        storeInstr->op = strdup("ST");
        storeInstr->reg = strdup(registers[spillReg].var);
        storeInstr->arg1 = getRegString(spillReg);
        storeInstr->arg2 = NULL;
        targetCode[targetCount++] = storeInstr;
    }
    
    registers[spillReg].var = strdup(var);
    registers[spillReg].dirty = 0;
    
    if(!isTarget && !(isdigit(var[0]) || var[0] == '-')) {
        Instruction *loadInstr = malloc(sizeof(Instruction));
        loadInstr->op = strdup("LD");
        loadInstr->reg = getRegString(spillReg);
        loadInstr->arg1 = strdup(var);
        loadInstr->arg2 = NULL;
        targetCode[targetCount++] = loadInstr;
    }
    
    return spillReg;
}

//Generate target code for an arithmetic operation
void genArithmetic(Quad *q) {
    char *op;
    switch(q->op) {
        case ADD: op = "ADD"; break;
        case SUB: op = "SUB"; break;
        case MUL: op = "MUL"; break;
        case DIV: op = "DIV"; break;
        case MOD: op = "REM"; break;
        default: return;
    }
    
    // Get registers for operands
    int reg1,reg2,targetReg;

    if(!isInteger(q->arg1) && q->arg1[0] != '$'){
        reg1 = getReg(q->arg1, 1);
        Instruction *load1 = malloc(sizeof(Instruction));
        load1->op = strdup("LD");
        load1->reg = getRegString(reg1);
        load1->arg1 = strdup(q->arg1);
        load1->arg2 = NULL;
        addSymbol(q->arg1,reg1);
        targetCode[targetCount++] = load1;
    }
    else if(q->arg1[0] == '$'){
        SymbolTableEntry *temp = lookupSymbol(q->arg1);
        reg1 = temp->temp_reg;
    }

    if(!isInteger(q->arg2) && q->arg2[0] != '$'){
        reg2 = getReg(q->arg2, 1);
        Instruction *load2 = malloc(sizeof(Instruction));
        load2->op = strdup("LD");
        load2->reg = getRegString(reg2);
        load2->arg1 = strdup(q->arg2);
        load2->arg2 = NULL;
        addSymbol(q->arg2,reg2);
        targetCode[targetCount++] = load2;
    }
    else if(q->arg2[0] == '$'){
        SymbolTableEntry *temp = lookupSymbol(q->arg2);
        reg2 = temp->temp_reg;
    }

    targetReg = getReg(q->result, 1);
    addSymbol(q->result,targetReg);

    
    // Generate arithmetic instruction
    Instruction *arithInstr = malloc(sizeof(Instruction));
    arithInstr->op = strdup(op);
    arithInstr->reg = getRegString(targetReg);
    if(isInteger(q->arg1)){
        arithInstr->arg1 = q->arg1;
    }
    else{
        arithInstr->arg1 = getRegString(reg1);
    }
    if(isInteger(q->arg2)){
        arithInstr->arg2 = q->arg2;
    }
    else{
        arithInstr->arg2 = getRegString(reg2);
    }
    targetCode[targetCount++] = arithInstr;
    
    // Store result if not temporary
    if(q->result[0] != '$') {
        Instruction *storeInstr = malloc(sizeof(Instruction));
        storeInstr->op = strdup("ST");
        storeInstr->reg = strdup(q->result);
        storeInstr->arg1 = getRegString(targetReg);
        storeInstr->arg2 = NULL;
        targetCode[targetCount++] = storeInstr;
    }
    
    registers[targetReg].free = 0;
    registers[targetReg].var = strdup(q->result);
    registers[targetReg].dirty = 1;
}

// Generate target code for assignment with proper LDI/LD handling
void genAssignment(Quad *q) {
    int targetReg;
    if(isInteger(q->arg1)){
        targetReg = getReg(q->result, 1);
        Instruction *loadInstr = malloc(sizeof(Instruction));
        loadInstr->op = strdup("LDI");
        loadInstr->reg = getRegString(targetReg);
        loadInstr->arg1 = strdup(q->arg1);
        loadInstr->arg2 = NULL;
        targetCode[targetCount++] = loadInstr;
    }    
    else{
        SymbolTableEntry *temp = lookupSymbol(q->arg1);
        targetReg = temp->temp_reg;
    }

    Instruction *storeInstr = malloc(sizeof(Instruction));
    storeInstr->op = strdup("ST");
    storeInstr->reg = strdup(q->result);
    storeInstr->arg1 = getRegString(targetReg);
    storeInstr->arg2 = NULL;
    addSymbol(q->result,targetReg);
    targetCode[targetCount++] = storeInstr;
    registers[targetReg].free = 0;
    registers[targetReg].var = NULL;
    registers[targetReg].dirty = 0;
    }
    
// Generate target code for conditional jump
void genCondJump(Quad *q) {
    char *op;
    switch(q->op) {
        case EQUAL: op = "JNE"; break;    
        case NOTEQUAL: op = "JEQ"; break; 
        case LT: op = "JGE"; break;       
        case GT: op = "JLE"; break;       
        case LE: op = "JGT"; break;       
        case GE: op = "JLT"; break;       
        default: return;
    }
    
    // Load operands if needed
    int reg1,reg2; // getReg(q->arg1, 0);
    //int reg2 = getReg(q->arg2, 0);
    
    if(!(isdigit(q->arg1[0]) || q->arg1[0] == '-' || q->arg1[0] == '$')) {
        Instruction *load1 = malloc(sizeof(Instruction));
        load1->op = strdup("LD");
        //SymbolTableEntry *temp = lookupSymbol(q->arg1);
        reg1 = getReg(q->arg1,1);//temp->temp_reg;
        load1->reg = getRegString(reg1);
        load1->arg1 = strdup(q->arg1);
        load1->arg2 = NULL;
        targetCode[targetCount++] = load1;
    }
    
    if(!(isdigit(q->arg2[0]) || q->arg2[0] == '-' || q->arg2[0] == '$')) {
        Instruction *load2 = malloc(sizeof(Instruction));
        load2->op = strdup("LD");
        //SymbolTableEntry *temp = lookupSymbol(q->arg2);
        reg2 = getReg(q->arg2,1);//temp->temp_reg;
        load2->reg = getRegString(reg2);
        load2->arg1 = strdup(q->arg2);
        load2->arg2 = NULL;
        targetCode[targetCount++] = load2;
    }
    

    // Generate conditional jump
    Instruction *jumpInstr = malloc(sizeof(Instruction));
    jumpInstr->op = strdup(op);
    if(!(isdigit(q->arg1[0]) || q->arg1[0] == '-' || q->arg1[0] == '$')){
        jumpInstr->reg = getRegString(reg1);
    }
    else if(isdigit(q->arg1[0]) || q->arg1[0] == '-'){
        jumpInstr->reg = strdup(q->arg1);
    }
    else{
        SymbolTableEntry *temp = lookupSymbol(q->arg1);
        reg1 = temp->temp_reg;
        jumpInstr->reg = getRegString(reg1);
    }
    if(isdigit(q->arg2[0]) || q->arg2[0] == '-') {
        jumpInstr->arg1 = strdup(q->arg2);
    } else {
        jumpInstr->arg1 = getRegString(reg2);
    }
    jumpInstr->arg2 = NULL;
    targetCode[targetCount++] = jumpInstr;
}

int main(){
    
    yyparse();

    // Print header without extra spaces
    printf("Blocks of intermediate code:\n\n");
    
    int blockNum = 1;
    
    // First pass: Mark all leaders
    int *leaders = (int*)calloc(quadCount + 1, sizeof(int));
    for (int i = 1; i < quadCount; i++) {
        if (isLeader(i)) {
            leaders[i] = 1;
        }
    }

    for (int i = 1; i <  quadCount; i++) {
        if (leaders[i]) {
            blk1[i] = blockNum;
            printf("Block %d\n", blockNum++);
        }
        else{
            blk1[i] = blockNum - 1;
        }
        Quad *q = quads[i];
        printf("    %d: ", i);
        switch(q->op){
            case SET:
                printf("%s = %s\n",q->result,q->arg1);
                break;
            case ADD:
                printf("%s = %s + %s\n", q->result, q->arg1, q->arg2);
                break;
            case SUB:
                printf("%s = %s - %s\n", q->result, q->arg1, q->arg2);
                break;
            case MUL:
                printf("%s = %s * %s\n", q->result, q->arg1, q->arg2);
                break;
            case DIV:
                printf("%s = %s / %s\n", q->result, q->arg1, q->arg2);
                break;
            case MOD:
                printf("%s = %s %% %s\n", q->result, q->arg1, q->arg2);
                break;
            case EQUAL:
                printf("iffalse (%s == %s) %s\n", q->arg1, q->arg2, q->result);                
                break;
            case NOTEQUAL:
                printf("iffalse (%s != %s) %s\n", q->arg1, q->arg2, q->result);
                break;
            case LT:
                printf("iffalse (%s < %s) %s\n", q->arg1, q->arg2, q->result);
                break;
            case GT:
                printf("iffalse (%s > %s) %s\n", q->arg1, q->arg2, q->result);
                break;
            case LE:
                printf("iffalse (%s <= %s) %s\n", q->arg1, q->arg2, q->result);
                break;
            case GE:
                printf("iffalse (%s >= %s) %s\n", q->arg1, q->arg2, q->result);
                break;
            case JMP:
                printf("goto %s\n", q->arg2);
                break;
            case LOOP:
                printf("%s\n", q->arg2);
                break;
        }
    }
    blk1[quadCount] = blockNum;
    printf("\n");
    printf("    %d:\n",quadCount);

    printf("\n\nTarget code:\n\n");

    initRegisters();
    int currentBlock = 1;
    // Process each quad
    for(int i = 1; i < quadCount; i++) {
        if (leaders[i]) {
            initRegisters();
            currentBlock++;
            //printf("Block %d\n", currentBlock++); 
        } 
        int start = targetCount;
        int final;
        Quad *q = quads[i];
        switch(q->op) {
        case SET:
            genAssignment(q);
            break;
            case ADD:
            case SUB:
            case MUL:
            case DIV:
            case MOD:
                genArithmetic(q);
                break;
            case EQUAL:
            case NOTEQUAL:
            case LT:
            case GT:
            case LE:
            case GE:
                genCondJump(q);
                break;
            case LOOP:
                // Unconditional jump
                Instruction *instr = malloc(sizeof(Instruction));
                instr->op = strdup("JMP");
                char *target = strstr(q->arg2, "goto ");
                char *str = strdup(target + 5);
                int t = atoi(str);
                int block_1 = blk1[t];
                for(int i=1;i<targetCount;i++){
                    if(block_1 == blk2[i]){
                        t = i;
                        break;
                    }
                }
                char *int_to_str = (char*)malloc(sizeof(char)*12);
                sprintf(int_to_str,"%d",t);
                instr->reg = int_to_str;
                instr->arg1 = NULL;
                instr->arg2 = NULL;
                targetCode[targetCount++] = instr;
                break;
        }
        final = targetCount;

        for(int i=start;i<final;i++){
            blk2[i] = currentBlock - 1;
        }

    }

    blk2[targetCount] = currentBlock;

    for(int i=1;i<targetCount;i++){
        if(targetCode[i]->op[0]=='J' && targetCode[i]->op[1]!='M'){
            int block_no = blk2[i];
            int line_no_in_1;
            for(int i=1;i<quadCount;i++){
                if(blk1[i]>block_no){
                    line_no_in_1 = i-1;
                    break;
                }
            }
            Quad *q = quads[line_no_in_1]; 
            char *target = strstr(q->result, "goto ");
            char *str = strdup(target + 5);
            int t = atoi(str);
            int tar_block = blk1[t];
            int line_no_in_2;
            for(int j=i;i<targetCount;j++){
                if(blk2[j] == tar_block){
                    line_no_in_2 = j;
                    break;
                }
            }
            char *tmp = (char *)malloc(sizeof(char));
            sprintf(tmp,"%d",line_no_in_2);
            targetCode[i]->arg2 = tmp;
        }
    }

    int block_target_code_generation = 1;
    for(int i=1;i<targetCount;i++){
        if(blk2[i]==block_target_code_generation){
            block_target_code_generation++;
            printf("Block %d\n", blk2[i]);
        }
        printf("    %d: ",i);
        Instruction *instr = targetCode[i];
        if(instr->op) {
            printf("%s ", instr->op);
            if(instr->reg) printf("%s ", instr->reg);
            if(instr->arg1) printf("%s ", instr->arg1);
            if(instr->arg2) printf("%s", instr->arg2);
            printf("\n");
        }
    }
    printf("\n    %d:",targetCount);
    return 0;
}


