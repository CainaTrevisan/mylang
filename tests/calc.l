%{
#include "calc.tab.h" 
%}

clcomm "*/"

%%
"+"    { return ADD;  	  		}
"-"    { return SUB;  	  		}
"*"    { return MUL; 	  		}
"/"    { return DIV;  	  		}
"%"    { return MOD;  	  		}

"=="   { return EQ;  	  		}
">"    { return GT;  	  		}
"<"    { return LT;  	  		}
">="   { return GE;  	  		}
"<="   { return LE;  	  		}

"||"   { return OR;  	  		}
"&&"   { return AND;  	  		}
"!"    { return NOT;  	  		}

"|"    { return BITOR;    		}
"&"    { return BITAND;  		}
"~"    { return BITNOT;  	  	}
"^"    { return BITXOR;   		}
">>"   { return RSHIFT;   		}
"<<"   { return LSHIFT;   		}

"("    { return LPAREN;   		}
")"    { return RPAREN;   		}
\n     { return EOL; 	  		}
[ \t]  { /* ignore tabs*/ 		}
"//".*\n { /* ignore comments */ }
[0-9]+ { yylval = atoi(yytext) ;  return INT; }
0x[a-fA-F0-9]+ { yylval = strtol(yytext, NULL, 16) ;  return HEX; }
.      { printf("Mystery character %c\n", *yytext); }
%%

