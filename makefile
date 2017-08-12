lex : lex.yy.c
	g++ lex.yy.c parser.tab.c -lfl -o lex 

lex.yy.c : lexer.l parser.tab.h
	flex lexer.l

parser.tab.c parser.tab.h : parser.y
	bison -d parser.y

rm :
	rm lex lex.yy.c 
