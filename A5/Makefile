all:
	flex expr.l       
	yacc -d expr.y                     
	gcc -o icgen intcodegen.c -lm

run: 
	./icgen < sample.txt > intcode.c
	gcc -o ic intcode.c -lm
	./ic
	
clean:
	rm -f lex.yy.c y.tab.c y.tab.h icgen ic intcode.c
