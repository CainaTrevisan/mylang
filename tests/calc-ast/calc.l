%{
#include "ast.h"
#include "calc.tab.h"
%}

EXP ([Ee][+-]?[0-9]+)

%%
"+" |
"-" |
"*" |
"/" |
"(" |
")"                       { return yytext[0]; } 

[0-9]+"."[0-9]*{EXP}? |
"."[0-9]+{EXP}?       |  
[0-9]+                    { yylval.d = atof(yytext); return NUMBER; } 


\n                        { return EOL; }
"//".*                    /* comment */
[ \t]                     { /* ignore whitespace */}
.                         { yyerror("Unknown character %c\n", *yytext); }

%%
