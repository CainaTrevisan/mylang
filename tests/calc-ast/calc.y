%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast.h" 

extern int yylineno;
extern int yylex();
extern int yyparse();
%}

%union {
  struct ast *a;
  double d;
}

/* Tokens */

%token <d> NUMBER 
%token EOL

%left '+' '-'
%left '*' '/'
%nonassoc UMINUS UPLUS

%type <a> exp 

%%
calclist:
  | calclist exp EOL {
      printf("= %4.4g\n", eval($2));
      free_ast($2);
      printf("> ");
    }

  | calclist EOL { printf("> "); }
;

exp: 
  | exp '+' exp { $$ = new_ast('+', $1, $3); }
  | exp '-' exp { $$ = new_ast('-', $1, $3); }
  | exp '*' exp { $$= new_ast('*', $1, $3); }
  | exp '/' exp { $$= new_ast('/', $1, $3); }
  | '(' exp ')' { $$ = $2; } 
  | '-' exp %prec UMINUS{ $$ = new_ast('M', NULL, $2); } /* Unary minus operator */
  | '+' exp %prec UPLUS { $$ = new_ast('P', NULL, $2); } /* Unary plus  operator */
  | NUMBER { $$ = new_num($1); }
;
%%

void
yyerror(char *s, ...) {

    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d:error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

int
main () {
    printf("> ");
    return yyparse();
}

