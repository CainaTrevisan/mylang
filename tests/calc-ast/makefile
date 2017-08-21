calc : calc.yy.c 
	gcc calc.yy.c calc.tab.c ast.c ast.h -lfl -lm -o calc

calc.yy.c : calc.l calc.tab.h
	flex -o calc.yy.c calc.l

calc.tab.c calc.tab.h : calc.y
	bison -d calc.y

rm :
	rm calc calc.yy.c calc.tab.c calc.tab.h lex.yy.c
