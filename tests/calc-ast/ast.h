/*
 Header for an ast
*/

/* Flex things */
extern int yylineno;
void yyerror(char *s, ...);

/* Symbol Table */
struct symbol {
    char *name;
    double value;
    struct ast *func;       // stmt for the function
    struct symlist *syms;   // list of dummy args
};

#define NHASH 9997
struct symbol symtab[NHASH];

struct symbol *lookup(char*);

struct symlist {
    struct symbol *sym;
    struct symlist *next;
};

struct symlist *new_symlist(struct symbol *sym, struct symlist *next);
void free_symlist(struct symlist *sl);

/* Node Types
 + - * /
 0-7 Comparison ops, bit coded 04 equal, 02 less, 01 greater
 M  Unary Minus
 P  Unary Plus
 L  Expression or Statement List
 I  If Statement
 W  While Statement
 N  Symbol Ref
 =  Assignment
 S  List of Symbols
 F  Built-in Function Call
 C  User Function Call
*/

enum bifs {
    B_sqrt = 1,
    B_exp,
    B_pow,
    B_log,
    B_print
};

/* ast Node */
struct ast{
    int nodetype;
    struct ast *l;      // Left Node
    struct ast *r;      // Right Node
};

struct fncall {         // built-in functions
    int nodetype;       // type F
    struct ast *l;
    enum bifs functype;
};

struct ufncall {        // User defined functions
    int nodetype;       // type C
    struct ast *l;      // list of arguments
    struct symbol *s;
};

struct flow {
    int nodetype;       // type I or W
    struct ast *cond;   // condition
    struct ast *tl;     // then branch or do list
    struct ast *el;     // optional else branch

};

struct numval {
    int nodetype;       // type K for constant
    double value;
};

struct symref {
    int nodetype;       // type N
    struct symbol *s;
};

struct symasgn {
    int nodetype;       // type =
    struct symbol *s;
    struct ast *v;
};

// Create ast
struct ast *new_ast(int nodetype, struct ast *l, struct ast *r);
struct ast *new_num(double d);
struct ast *new_cmp(int cmptype, struct ast *l, struct ast *r);
struct ast *new_func(int functype, struct ast *l);
struct ast *new_call(struct symbol *s, struct ast *l);
struct ast *new_ref(struct symbol *s);
struct ast *new_asgn(struct symbol *s, struct ast *v);
struct ast *new_flow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el);

// Define Function
void dodef (struct symbol *name, struct symlist *syms, struct ast *stmts);

// Evaluate ast
double eval(struct ast *);

// Delete and Free ast
void free_ast(struct ast *);
