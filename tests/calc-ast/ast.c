#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast.h"

struct ast *
new_ast ( int nodetype, struct ast *l, struct ast *r ) {

    struct ast * a = (struct ast *) malloc (sizeof(struct ast));
    if (!a) {
        yyerror("Could not allocate new ast node");
        exit(0);
    }

    a->nodetype = nodetype;
    a->l = l;
    a->r = r;

    return a;
}

struct ast *
new_num ( double d ) {

    struct numval *a = (struct numval *) malloc (sizeof(struct numval));
    if (!a) {
        yyerror("Could not allocate new numval node");
        exit(0);
    }

    a->nodetype = 'K';
    a->value = d;
    return (struct ast *) a;
}

double
eval ( struct ast *a ) {
    double v;

    switch ( a->nodetype ) {
    
        case 'K': v = ((struct numval *)a)->value; break;

        case '+': v = eval(a->l) + eval (a->r) ; break;
        case '-': v = eval(a->l) - eval (a->r) ; break;
        case '*': v = eval(a->l) * eval (a->r) ; break;
        case '/': v = eval(a->l) / eval (a->r) ; break;

        case 'M': v = -eval(a->l)              ; break;
        case 'P': v = +eval(a->l)              ; break;
        default: printf("Internal error: bad node %c\n", a->nodetype);
    }

    return v;
}

void
free_ast(struct ast *a) {

    switch ( a->nodetype ) {
        // Two subtrees
        case '+':
        case '-':
        case '*':
        case '/':
            free_ast(a->r);

        // One subtree
        case 'M':
        case 'P':
            free_ast(a->l);

        // No subtree
        case 'K':
            free(a);
            break;

        default: printf("internal error: free bad node %c\n", a->nodetype);
    }
}
