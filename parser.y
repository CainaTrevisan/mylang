%{
#include <cstdio>
using namespace std;

// stuff from flex that bison needs to know about:
extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;
 
void yyerror(const char *s);
%}

// Bison fundamentally works by asking flex to get the next token, which it
// returns as an object of type "yystype".  But tokens could be of any
// arbitrary data type!  So we deal with that in Bison by defining a C union
// holding each of the types of tokens that Flex could return, and have Bison
// use that union instead of "int" for the definition of "yystype":
%union {
	int ival;
	float fval;
	char *sval;
	int hval;
	char *bval;
}

// define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the union:
%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING
%token <hval> HEX
%token <bval> BIN 

%%
// this is the actual grammar that bison will parse, but for right now it's just
// something silly to echo to the screen what bison gets from flex.  We'll
// make a real one shortly:
snazzle:
	snazzle INT 	 { printf("bison found an int: %d\n",   $2) ; }
	| snazzle HEX 	 { printf("bison found a hex: %x\n", 	$2) ; }
	| snazzle BIN 	 { printf("bison found a bin: %s\n", 	$2) ; }
	| snazzle FLOAT  { printf("bison found a float: %f\n",  $2) ; }
	| snazzle STRING { printf("bison found a string: %s\n", $2) ; }
	| INT            { printf("bison found an int: %d\n",   $1) ; }
	| HEX 			 { printf("bison found a hex: %x\n", 	$1) ; }
	| BIN 			 { printf("bison found a bin: %s\n", 	$1) ; }
	| FLOAT          { printf("bison found a float: %f\n",  $1) ; }
	| STRING         { printf("bison found a string: %s\n", $1) ; }
	;
%%

int main(int, char**) {
	do {
		yyparse();
	} while (!feof(yyin)) ;
}

void yyerror(const char *s) {
	fprintf( stderr, "EEK, parse error!  Message: %s\n", s) ;
	// might as well halt now:
	exit(-1);
}
