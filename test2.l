%{
    #include <math.h>
    int lines = 0;
%}

%option noyywrap

DIGHT [0-9]
ID [a-z][a-z0-9]

%%
{DIGHT}+ {
    printf("An integer: %s (%d)\n" , yytext , atoi(yytext));
}

{DIGHT}+"."{DIGHT}* {
    printf("A float: %s (%g)\n" , yytext , atof(yytext));
}

"if"|"then"|"{"|"}" {
    printf("A key word: %s\n" , yytext);
}

"\n" {
    lines += 1;
}

%%

int main(int argc , char **argv)
{
    yyin = fopen("in" , "r");
    /*if(argc > 0)
    {
        yyin = fopen(argv[0] , "r");
    }
    else
    {
        yyin = stdin;
    }*/

    yylex();
    printf("%d lines\n" , lines);
}
