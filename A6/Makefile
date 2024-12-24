all:
	flex prog.l       
	yacc -d prog.y                     
	gcc -o codegen codegen.c -lm

run: 
	./codegen < sample.txt > output.txt
	
clean:
	rm -f lex.yy.c y.tab.c y.tab.h codegen output.txt
