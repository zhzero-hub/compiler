%{
int num_lines = 0 , num_chars = 0 , sb_times = 0;
%}

%option noyywrap

%%
\n  ++ num_lines; ++ num_chars;
.   ++ num_chars;
"sb" ++ sb_times;
%%

int main()
{
    yylex();
    printf("# of lines = %d , # of chars = %d\n" , num_lines , num_chars);
    printf("%d of sb times" , sb_times);
}
