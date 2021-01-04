%{
#include "ast.h"
#include "build.h"
#include <stdio.h>
#include <stdlib.h>
#include "lex.yy.c"

#define YYSTYPE AstNode*

extern int line;
extern int column;

void yyerror(const char* msg);
AstNode* parse();

// 锟斤法锟斤拷锟斤拷锟节碉拷
AstNode* astRoot = nullptr;

%}

  // 锟斤拷锟脚讹拷锟斤拷
%token ELSE IF INT RETURN VOID WHILE
%token ADD MINUS MULTIPLY DIVIDE
%token LT LE GT GE EQ NE
%token ID NUM
%token ASSIGN SC CM
%token LP RP LBRACK RBRACK LBRACE RBRACE

  // 锟斤拷锟斤拷锟斤拷锟饺硷拷
%left '+' '-'
%left '*' '/'

/* 锟斤拷锟絠f else 锟狡斤拷锟斤拷约锟斤拷突 */
%nonassoc LOWER_ELSE
%nonassoc ELSE

%start program  /* 指锟斤拷锟斤拷始锟斤拷锟斤拷 */
%%
program
        : declarationList
          { astRoot = newRoot($1); }
        ;

declarationList
        : declarationList declaration
          { $$ = addSibling($1, $2); }
          //{ if($2 == nullptr) cout << "nullptr" << endl; else cout << "Not nullptr" << endl; }
        | declaration
          { $$ = $1; }
        ;

declaration
        : var_declaration
          { $$ = $1; }
        | fun_declaration
          //{ cout << "new fun_declaration" << endl; }
          { $$ = $1; }
        ;

var_declaration
        : type _id SC
          { $$ = newVariable($1, $2); }
        | type _id LBRACK _num RBRACK SC
          { $$ = newVariable($1, $2, $4); }
        ;

type
        : INT
          { $$ = newType("int"); }
        | VOID
          { $$ = newType("void"); }

fun_declaration
        : type _id LP params RP compound_stmt
          //{ if($4 == nullptr) cout << "is nullptr" << endl; }
          { $$ = newFunction($1, $2, $4, $6); }
        ;

params
        : param_list
          { $$ = $1; }
        | VOID
          { $$ = nullptr; }
        ;

param_list
        : param_list CM param
          { $$ = addArgs($1, $3); }
        | param
          { $$ = addArgs(nullptr, $1); }
        ;

param
        : type _id
          { $$ = newVariable($1, $2); }
        | type _id LBRACK RBRACK
          { $$ = newVariable($1, $2, nullptr); }
        ;

compound_stmt
        : LBRACE local_declarations statement_list RBRACE
          { $$ = newFunctionStmt($2, $3); }
        ;

local_declarations
        : local_declarations var_declaration
          { $$ = newLocalDeclare($1, $2); }
        | /* empty */
          { $$ = newLocalDeclare(nullptr, nullptr); }
        ;

statement_list
        : statement_list statement
          { $$ = newStateDeclare($1, $2); }
        | /* empty */
          { $$ = newStateDeclare(nullptr, nullptr); }
        ;

statement
        : expression_stmt
          { $$ = $1; }
        | compound_stmt
          { $$ = $1; }
        | selection_stmt
          { $$ = $1; }
        | iteration_stmt
          { $$ = $1; }
        | return_stmt
          { $$ = $1; }
        ;

expression_stmt
        : expression SC
          { $$ = $1; }
        | SC
          { $$ = nullptr; }
        ;

selection_stmt
        : IF LP expression RP statement
          { $$ = newIf($3, $5, nullptr); }
          %prec LOWER_ELSE
        | IF LP expression RP statement ELSE statement
          { $$ = newIf($3, $5, $7); }
        ;

iteration_stmt
        : WHILE LP expression RP statement
          { $$ = newWhile($3, $5); }
        ;

return_stmt
        : RETURN SC
          { $$ = newReturn(nullptr); }
        | RETURN expression SC
          //{ if($2 == nullptr) cout << "return" << endl; else cout << "111" << endl; }
          { $$ = newReturn($2); }
        ;

expression
        : var ASSIGN expression
         { $$ = newOperators($1, newToken("="), $3); }
         //{ if($1 == nullptr) cout << "nullptr" << endl; else cout << "Not nullptr" << endl; }
         //{ $$ = newToken("="); }
        | simple_expression
         //{ cout << "simple expression" << endl; }
         { $$ = $1; }
        ;

var
        : _id
          { $$ = $1; }
        | _id LBRACK expression RBRACK
          { $$ = newToken($1, $3); }
        ;

simple_expression
        : additive_expression relop additive_expression
          { $$ = newCondition($1, $2, $3); }
        | additive_expression
          { $$ = $1; }
        ;

relop
        : LT
          { $$ = newToken("<"); }
        | LE
          { $$ = newToken("<="); }
        | GT
          { $$ = newToken(">"); }
        | GE
          { $$ = newToken(">="); }
        | EQ
          { $$ = newToken("=="); }
        | NE
          { $$ = newToken("!="); }
        ;

additive_expression
        : additive_expression addop term
          { $$ = newOperators($1, $2, $3); }
        | term
          { $$ = $1; }
        ;

addop
        : ADD
          { $$ = newToken("+"); }
        | MINUS
          { $$ = newToken("-"); }

term
        : term mulop factor
          { $$ = newOperators($1, $2, $3); }
        | factor
          { $$ = $1; }
        ;

mulop
        : MULTIPLY
          { $$ = newToken("*"); }
        | DIVIDE
          { $$ = newToken("/"); }
        ;

factor
        : LP expression RP
          { $$ = $2; }
        | var
          { $$ = $1; }
        | call
          { $$ = $1; }
        | _num
          { $$ = $1; }
        ;

call
        : _id LP args RP
          { $$ = newFunction(nullptr, $1, $3, nullptr); }
        ;

args
        : arg_list
          { $$ = $1; }
        | /* empty */
          { $$ = nullptr; }
        ;

arg_list
        : arg_list CM expression
          { $$ = addArgs($1, $3); }
        | expression
          { $$ = addArgs(nullptr , $1); }
        ;

_id
        : ID
          { $$ = newId(yytext); }
        ;

_num
        : NUM
          { $$ = newNum(yytext); }
        ;
%%

 //锟斤拷锟斤拷锟斤拷锟斤拷
void yyerror(const char* msg) 
{
    printf("%s,%s,Line %d column %d\n", yytext, msg, line, column);
}

AstNode* parse()
{
  yyparse();
  return astRoot;
}
