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
  struct symbol *s;     // which symbol
  struct symlist *sl;   // which function
  int fn;
}

/* Tokens */
%token <d> NUMBER
%token <s> NAME
%token <fn> FUNC
%token EOL

%token IF THEN END ELSE WHILE DO LET

%nonassoc <fn> CMP
%right '='
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS UPLUS

%type <a> exp stmt list explist
%type <sl> symlist

%start calclist

%%

stmt: IF exp '{' list '}' ELSE '{' list '}'
                                  { $$ = new_flow('I', $2, $4, $8)   ;}
    | IF exp '{' list '}'         { $$ = new_flow('I', $2, $4, NULL) ;}
    | WHILE exp  '{' list '}'     { $$ = new_flow('W', $2, $4, NULL) ;}
    | exp
;

list:                           { $$ = NULL ;}
    | stmt ';' list             { if ($3 == NULL)
                                    $$ = $1;
                                  else
                                    $$ = new_ast('L', $1, $3);
                                }
;

exp:  exp CMP exp                 { $$ = new_cmp($2 , $1, $3)   ;}
    | exp '+' exp                 { $$ = new_ast('+', $1, $3)   ;}
    | exp '-' exp                 { $$ = new_ast('-', $1, $3)   ;}
    | exp '*' exp                 { $$ = new_ast('*', $1, $3)   ;}
    | exp '/' exp                 { $$ = new_ast('/', $1, $3)   ;}
    | '(' exp ')'                 { $$ = $2                     ;}
    | '-' exp %prec UMINUS        { $$ = new_ast('M', NULL, $2) ;} /* Unary minus operator */
    | '+' exp %prec UPLUS         { $$ = new_ast('P', NULL, $2) ;} /* Unary plus  operator */
    | NUMBER                      { $$ = new_num($1)            ;}
    | NAME '=' explist            { $$ = new_asgn($1, $3)       ;}
    | FUNC '(' explist ')'        { $$ = new_func($1, $3)       ;}
    | NAME '(' explist ')'        { $$ = new_call($1, $3)       ;}
;

explist: exp
  | exp ',' explist             {  $$ = new_ast('L', $1, $3)  ;}
;

symlist: NAME                   { $$ = new_symlist($1, NULL)  ;}
  | NAME ',' symlist            { $$ = new_symlist($1, NULL)  ;}
;

calclist: /* nothing */
  | calclist stmt '.' { printf("= %4.4g\n> ", eval($2));
                                  free_ast($2);
                                }

  | calclist LET NAME '(' symlist ')' '=' list '.'
                                { printf("we here\n");
                                  dodef($3, $5, $8);
                                  printf("Defined %s\n> ", $3->name);
                                }

  | calclist error '.' { yyerrok; printf("> ")       ;}
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

