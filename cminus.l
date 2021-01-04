%option noyywrap nounput

%{ /*指明将要引用的头文件，这部分将被照搬到生成的lex.yy.c文件中*/
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "build.h"
#define YYSTYPE AstNode*
#include "y.tab.h"



int file_end();
void comment();
void count();
int line=1; //行号
int column = 0; //列号

%} 


  // 定义正则表达式
digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+


%%  

"/*"                { comment(); }
"!EOF"              { return file_end(); }
<<EOF>>             { return file_end(); }

"else"              { printf("%s ",yytext); count(); return ELSE; }
"if"                { printf("%s ",yytext); count(); return IF; }
"int"               { printf("%s ",yytext); count(); return INT; }
"return"            { printf("%s ",yytext); count(); return RETURN; }
"void"              { printf("%s ",yytext); count(); return VOID; }
"while"             { printf("%s ",yytext); count(); return WHILE; }

"+"                 { printf("%s ",yytext); count(); return ADD; }
"-"                 { printf("%s ",yytext); count(); return MINUS; }
"*"                 { printf("%s ",yytext); count(); return MULTIPLY; }
"/"                 { printf("%s ",yytext); count(); return DIVIDE; }

"<"                 { printf("%s ",yytext); count(); return LT; }
"<="                { printf("%s ",yytext); count(); return LE; }
">"                 { printf("%s ",yytext); count(); return GT; }
">="                { printf("%s ",yytext); count(); return GE; }
"=="                { printf("%s ",yytext); count(); return EQ; }
"!="                { printf("%s ",yytext); count(); return NE; }

"="                 { printf("%s ",yytext); count(); return ASSIGN; }
";"                 { printf("%s ",yytext); count(); return SC; }
","                 { printf("%s ",yytext); count(); return CM; }

"("                 { printf("%s ",yytext); count(); return LP; }
")"                 { printf("%s ",yytext); count(); return RP; }
"["                 { printf("%s ",yytext); count(); return LBRACK; }
"]"                 { printf("%s ",yytext); count(); return RBRACK; }
"{"                 { printf("%s ",yytext); count(); return LBRACE; }
"}"                 { printf("%s ",yytext); count(); return RBRACE; }

{number}            { printf("%s ",yytext); count(); return NUM; }
{identifier}        { printf("%s ",yytext); count(); return ID; }
{newline}           { printf("\n"); count(); line++; }
{whitespace}        { count(); }


%%

 // 匹配到文件末尾
int file_end() 
{
    printf("syntax analysis finished. no issue found.\n");
    yyterminate();
}


 // 处理多行注释
void comment() 
{
    char c;
    char p = '\0';
    do {
        c = yyinput();  // 从输入源读取下一个字符
        if (c == EOF) break;  // 文件末尾
        if (c == '\n') line++;  // 换行，行数+1
        if (p == '*' && c == '/') break;  // 注释结束
        p = c;
    } while (1);
}

void count() 
{
    int i;
    for (i = 0; yytext[i] != '\0'; i++)
    {
        if (yytext[i] == '\n') column = 0; 
        else if (yytext[i] == '\t') column += 8 - (column % 8);
        else column++;
    }
}
