%{

#define YY_DECL extern "C" int yylex()

#include "parser.tab.h"
%}

%%
[ \t\n]          			 ;
print			  			{ printf("print function\n")   ; }
"="
"=="						{ return ASSIGN; }
"<"							{ return ASSIGN; }
">"
">="
"<="
"<="
"("
")"
"{"
"}"
"."
","
"+"
"-"
"*"
"/"
"%"
"**"
[0-9]+\.[0-9]+    			 { yylval.fval = atof(yytext)   ; return FLOAT ; }
-?[0-9]+            		 { yylval.ival = atoi(yytext)   ; return INT ; }
0x[0-9|a-f|A-F]+ 			 { yylval.hval = atof(yytext)   ; return HEX ; }
0b[0-1]+  		  			 { yylval.bval = strdup(yytext) ; return BIN ; }
\"[a-zA-Z0-9]+\"  		     { yylval.sval = strdup(yytext) ; return STRING ; }
%%
