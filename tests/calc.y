/* simplest version of calculator */
%{
#include <stdio.h>

extern int yylex();
extern int yyparse();
extern FILE *yyin;
void yyerror(char const *s);
%}

/* declare tokens */
%token INT HEX 
%token ADD SUB MUL DIV MOD ABS OR AND NOT 
%token EQ GT GE LT LE
%token BITXOR BITOR BITAND BITNOT RSHIFT LSHIFT
%token EOL
%token LPAREN RPAREN

%%
calclist: /* nothing */                     /*  matches at beginning of input */
 | calclist exp EOL { printf("= %d\t%x\n", $2, $2); } /* EOL is end of an expression */
 ;

exp: factor       /* default $$ = $1 */
 | exp ADD factor { $$ = $1 + $3; }
 | exp SUB factor { $$ = $1 - $3; }
 | SUB factor { $$ = -$2; }
 | ADD factor { $$ = +$2; }

 | exp EQ factor { $$ = $1 == $3; }
 | exp GT factor { $$ = $1 > $3; }
 | exp GE factor { $$ = $1 >= $3; }
 | exp LT factor { $$ = $1 < $3; }
 | exp LE factor { $$ = $1 <= $3; }

 | exp OR factor { $$ = $1 || $3; }
 | exp AND factor { $$ = $1 && $3; }
 | NOT exp { $$ = !$2; }

 | exp BITXOR factor { $$ = $1 ^ $3; }
 | exp BITAND factor { $$ = $1 & $3; }
 | exp BITOR factor { $$ = $1 | $3; }
 | BITNOT exp { $$ = ~$2; }

 | exp LSHIFT factor { $$ = $1 << $3; }
 | exp RSHIFT factor { $$ = $1 >> $3; }

 | BITOR exp BITOR { $$ = $2 >= 0? $2 : - $2; } /* ABS exp ABS */
 ;

factor: term       /* default $$ = $1 */ 
 | factor MUL term { $$ = $1 * $3; }
 | factor DIV term { $$ = $1 / $3; }
 | factor MOD term { $$ = $1 % $3; }
 ;

term: INT  /* default $$ = $1 */
 | HEX
 | LPAREN exp RPAREN { $$ = $2; }
;
%%
void yyerror(char const *s)
{
  fprintf(stderr, "error: %s\n", s);
}

int main(int argc, char **argv)
{
  yyparse();
}

