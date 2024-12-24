all:
	lex poly.l
	yacc -d poly.y
	gcc -o poly polyutils.c lex.yy.c y.tab.c -lm

run:
	./poly < sample.txt

clean:
	rm poly y.tab.c y.tab.h lex.yy.c
