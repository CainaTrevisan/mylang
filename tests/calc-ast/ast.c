#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "ast.h"

// Symbol Table
// Hash a Symbol

static unsigned
symhash ( char *sym ) {
    unsigned hash = 0;
    unsigned c;

    while ( c = *sym++ ) hash = hash*9 ^ c;

    return hash;
}

struct symbol *
lookup ( char *sym ) {
    struct symbol *sp = &symtab[symhash(sym)%NHASH];
    int scount = NHASH;

    while ( --scount >= 0 ) {
        if ( sp->name && !strcmp(sp->name, sym) ) {
            return sp;
        }

        if ( !sp->name ) {
            sp->name = strdup(sym);
            sp->value = 0;
            sp->func = NULL;
            sp->syms = NULL;
            return sp;
        }
    }

    if ( ++sp >= symtab+NHASH ) {
         sp = symtab;
    }

    yyerror("Symbol Table overflow\n");
    abort();
}

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

struct ast *
new_cmp ( int cmptype, struct ast *l, struct ast *r ) {

    struct ast *a = (struct ast *) malloc (sizeof (struct ast));

    if (!a) {
        yyerror("Could not allocate new cmp node");
        exit(0);
    }

    a->nodetype = '0' + cmptype;
    a->l = l;
    a->r = r;

    return a;
}

struct ast *
new_func ( int functype, struct ast *l ) {

    struct fncall *f = (struct fncall *) malloc (sizeof (struct fncall));

    if (!f) {
        yyerror("Could not allocate new func node");
        exit(0);
    }

    f->nodetype = 'F';
    f->functype = functype;
    f->l = l;

    return (struct ast *) f;
}

struct ast *
new_call( struct symbol *s, struct ast *l ) {

    struct ufncall *u = (struct ufncall *) malloc (sizeof (struct ufncall));

    if (!u) {
        yyerror("Could not allocate new call node");
        exit(0);
    }

    u->nodetype = 'C';
    u->l = l;
    u->s = s;

    return (struct ast *) u;
}

struct ast *
new_ref( struct symbol *s ) {

    struct symref *r = (struct symref *) malloc (sizeof (struct symref));

    if (!r) {
        yyerror("Could not allocate new reference node");
        exit(0);
    }

    r->nodetype = 'N';
    r->s = s;

    return (struct ast *) r;
}

struct ast *
new_asgn( struct symbol *s, struct ast *v ) {

    struct symasgn *a = (struct symasgn*) malloc (sizeof (struct symasgn));

    if (!a) {
        yyerror("Could not allocate new assignment node");
        exit(0);
    }

    a->nodetype = '=';
    a->s = s;
    a->v = v;

    return (struct ast *) a;
}

struct ast *
new_flow( int nodetype,  struct ast *cond, struct ast *tl, struct ast *el ) {

    struct flow *f = (struct flow *) malloc (sizeof (struct flow));

    if (!f) {
        yyerror("Could not allocate new flow node");
        exit(0);
    }

    f->nodetype = nodetype;
    f->cond = cond;
    f->tl = tl;
    f->el = el;

    return (struct ast *) f;
}

void
free_ast(struct ast *a) {

    switch ( a->nodetype ) {
        // Two subtrees
        case '+':
        case '-':
        case '*':
        case '/':
        case '1': case '2': case '3': case '4': case '5': case '6':
        case 'L':
            free_ast(a->r);

        // One subtree
        case 'M': case 'P':
        case 'C': case 'F':
            free_ast(a->l);

        // No subtree
        case 'K': case 'N':
            break;

        case '=':
            free( ((struct symasgn *)a)->v );
            break;

        case 'I': case 'W':
            free ( ((struct flow *)a)->cond );
            if ( ((struct flow *)a)->tl )
                free_ast ( ((struct flow *)a)->tl );
            if ( ((struct flow *)a)->el )
                free_ast ( ((struct flow *)a)->el );
            break;

        default: printf("Internal error: free bad node %c\n", a->nodetype);
    }

    free(a);
}

struct symlist *
new_symlist( struct symbol *sym, struct symlist *next) {

    struct symlist *sl  = (struct symlist *) malloc (sizeof(struct symlist));
    if (!sl) {
        yyerror("Could not allocate new symlist");
        exit(0);
    }

    sl->sym = sym;
    sl->next = next;

    return sl;
}

void free_symlist ( struct symlist *sl ) {

    struct symlist *nsl;

    while ( sl ) {
        nsl = sl->next;
        free (sl);
        sl = nsl;
    }
}

static double callbuiltin(struct fncall *);
static double calluser(struct ufncall *);

double
eval ( struct ast *a ) {
    double v;

    if (!a) {
        yyerror("internal error, null eval");
        return .0;
    }

    switch ( a->nodetype ) {

        // Constant
        case 'K': v = ((struct numval *)a)->value; break;

        // Name Reference
        case 'N': v = ((struct symref *)a)->s->value; break;

        // Assignment
        case '=': v = ((struct symasgn *)a)->s->value =
                  eval ( ((struct symasgn *)a)->v ); break;

        // Expressions
        case '+': v = eval(a->l) + eval (a->r)    ; break;
        case '-': v = eval(a->l) - eval (a->r)    ; break;
        case '*': v = eval(a->l) * eval (a->r)    ; break;
        case '/': v = eval(a->l) / eval (a->r)    ; break;

        // Unary expressions
        case 'M': v = -eval(a->l)                 ; break;
        case 'P': v = +eval(a->l)                 ; break;

        // Comparisons
        case '1': v = (eval(a->l)) >  eval((a->r))  ? 1 : 0  ; break;
        case '2': v = (eval(a->l)) <  eval((a->r))  ? 1 : 0  ; break;
        case '3': v = (eval(a->l)) != eval((a->r))  ? 1 : 0  ; break;
        case '4': v = (eval(a->l)) ==  eval((a->r)) ? 1 : 0  ; break;
        case '5': v = (eval(a->l)) >=  eval((a->r)) ? 1 : 0  ; break;
        case '6': v = (eval(a->l)) <=  eval((a->r)) ? 1 : 0  ; break;

        // Control Flow
        //
        // if/then/else
        case 'I':
            if ( eval( ((struct flow *)a)->cond ) != 0 ) {
                if ( ((struct flow *)a)->tl ) {
                    v = eval( ((struct flow *)a)->tl );
                } else {
                    v = .0;
                }
            } else {
                if ( ((struct flow *)a)->el ) {
                     v = eval( ((struct flow *)a)->el );
                } else {
                    v = .0;
                }
            }
            break;

        // while/do
        case 'W':
            v = .0;

                if ( ((struct flow *)a)->tl ) {
                    while( eval ( ((struct flow *)a)->cond) != 0 ) {
                        v = eval( ((struct flow *)a)->tl );
                    }
                }
                break;

        // List of Statements
        case 'L': eval(a->l); v = eval(a->r); break;

        case 'F': v = callbuiltin( (struct fncall *)a ); break;

        case 'C': calluser( (struct ufncall *)a ); break;

        default: printf("Internal error: bad node %c\n", a->nodetype);
    }

    return v;
}

static double
callbuiltin ( struct fncall *f ) {

    enum bifs functype = f->functype;
    double v = eval(f->l);

    switch ( functype ) {
        case B_sqrt:
            return sqrt(v);
        case B_exp:
            return exp(v);
        case B_pow:
            return pow(2,v);
        case B_log:
            return log(v);
        case B_print:
            printf(" = %4.4g\n> ", v);
            return v;
        default:
            yyerror("Unknown built-in function %d", functype);
            return .0;
    }
}

void
dodef ( struct symbol *name, struct symlist *syms, struct ast *func ) {

    if ( name->syms ) {
        free_symlist(name->syms);
    }

    if ( name->func) {
        free_ast(name->func);
    }

    name->syms = syms;
    name->func = func;
}

static double
calluser ( struct ufncall *f ) {

    struct symbol *fn = f->s;      // function name
    struct symlist *sl;            // dummy arguments
    struct ast *args = f->l;       // actual arguments
    double *oldval, *newval;       // saved arg values
    double v;
    int nargs;
    int i;

    if ( !fn->func ) {
         yyerror("Call to undefined function", fn->name);
         return .0;
    }

    sl = fn->syms;
    for ( nargs = 0 ; sl ; sl = sl->next ) {
        nargs++;
    }

    oldval = (double *) malloc (nargs * sizeof(double));
    newval = (double *) malloc (nargs * sizeof(double));

    if ( !oldval || !newval ) {
         yyerror("Out of space in %s", fn->name);
         return .0;
    }

    for ( i = 0 ; i < nargs ; ++i ) {
        if (!args) {
            yyerror("too few arguments in call to %s", fn->name);
            free(oldval);
            free(newval);
            return .0;
        }

        if ( args->nodetype == 'L' ) {
            newval[i] = eval(args->l);
            args = args->r;
        } else {
            newval[i] = eval(args);
            args = NULL;
        }
    }

    sl = fn->syms;
    for ( i = 0 ; i < nargs ; ++i ) {

        struct symbol *s = sl->sym;

        oldval[i] = s->value;
        s->value = newval[i];
        sl = sl->next;
    }

    free(newval);

    v=eval(fn->func);

    sl = fn->syms;
    for ( i = 0 ; i< nargs ; ++i ) {
         struct symbol *s = sl->sym;

         s->value = oldval[i];
         sl = sl->next;
    }

    free(oldval);
    return v;
}
