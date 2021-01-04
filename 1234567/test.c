int calc(int arg) 
{
    arg = 100 + arg;
    return arg;
}

int main(void) 
{
    int a;
    int b[2];
    a=1;
    while (a<10)
    {
        a=a+1;
    }
    b[1] = calc(3);
    return 0;
}