%option noyywrap nounput

%{ /*指明将要引用的头文件，这部分将被照搬到生成的lex.yy.c文件中*/
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include "ast.h"
#include "build.h"
#define YYSTYPE AstNode*
#include "y.tab.h"



int file_end();
void comment();
void count();
int line=1; //当前行
int column = 0; // 列号

%} 


  // 定义正则表达式
digit       [0-9]
number      [1-9]{digit}*
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+


%%  

"/*"                { comment(); }
"!EOF"              { return file_end(); }
<<EOF>>             { return file_end(); }

"else"              { printf("%s ",yytext); count(); fflush(stdout); return ELSE; }
"if"                { printf("%s ",yytext); count(); fflush(stdout); return IF; }
"int"               { printf("%s ",yytext); count(); fflush(stdout); return INT; }
"return"            { printf("%s ",yytext); count(); fflush(stdout); return RETURN; }
"void"              { printf("%s ",yytext); count(); fflush(stdout); return VOID; }
"while"             { printf("%s ",yytext); count(); fflush(stdout); return WHILE; }

"+"                 { printf("%s ",yytext); count(); fflush(stdout); return ADD; }
"-"                 { printf("%s ",yytext); count(); fflush(stdout); return MINUS; }
"*"                 { printf("%s ",yytext); count(); fflush(stdout); return MULTIPLY; }
"/"                 { printf("%s ",yytext); count(); fflush(stdout); return DIVIDE; }

"<"                 { printf("%s ",yytext); count(); fflush(stdout); return LT; }
"<="                { printf("%s ",yytext); count(); fflush(stdout); return LE; }
">"                 { printf("%s ",yytext); count(); fflush(stdout); return GT; }
">="                { printf("%s ",yytext); count(); fflush(stdout); return GE; }
"=="                { printf("%s ",yytext); count(); fflush(stdout); return EQ; }
"!="                { printf("%s ",yytext); count(); fflush(stdout); return NE; }

"="                 { printf("%s ",yytext); count(); fflush(stdout); return ASSIGN; }
";"                 { printf("%s ",yytext); count(); fflush(stdout); return SC; }
","                 { printf("%s ",yytext); count(); fflush(stdout); return CM; }

"("                 { printf("%s ",yytext); count(); fflush(stdout); return LP; }
")"                 { printf("%s ",yytext); count(); fflush(stdout); return RP; }
"["                 { printf("%s ",yytext); count(); fflush(stdout); return LBRACK; }
"]"                 { printf("%s ",yytext); count(); fflush(stdout); return RBRACK; }
"{"                 { printf("%s ",yytext); count(); fflush(stdout); return LBRACE; }
"}"                 { printf("%s ",yytext); count(); fflush(stdout); return RBRACE; }

{number}            { printf("%s ",yytext); count(); fflush(stdout); return NUM; }
{identifier}        { printf("%s ",yytext); count(); fflush(stdout); return ID; }
{newline}           { printf("\n"); count(); fflush(stdout); line++; }
{whitespace}        { count(); fflush(stdout); }


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
        // 遇到换行列号要清零
        if (yytext[i] == '\n') column = 0;
        // 遇到制表符特殊处理
        else if (yytext[i] == '\t') column += 8 - (column % 8);
        else column++;
    }
}
