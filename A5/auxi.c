#include<stdio.h>

int pwr(int arg1,int arg2){
    if(arg2 == 0){
        return 1;
    }
    else return arg1 * pwr(arg1,arg2-1);
}

void mprn(int MEM[],int idx){
    printf("+++ MEM[%d] set to %d\n",idx,MEM[idx]);
}

int eprn(int R[],int idx){
    printf("+++ Standalone expression evaluates to %d\n",R[idx]);
}