#include <iostream>
#include <vector>
#include <unordered_map>
#include "build.h"
#include "service.h"
#include "cminus.tab.c"

using namespace std;


int main(int argc, char ** argv)
{
    if ( argc > 1 )
    {
        if (!(yyin = fopen(argv[1], "r" )))
        {
            perror(argv[1]);
            return 1;
        }
        printf("[I]>>>Read data from file:%s\n", argv[1]);
    }
    else
    {
        printf("[I]>>>Read data from stdin.\n");
    }

    AstNode *root = parse();

    AstNode *p = root->children;
    auto rootBlock = new CodeBlock();
    while(p != nullptr)
    {
        auto codeBlock = new CodeBlock();
        codeBlock = blockGen(codeBlock , p);
        if(codeBlock == nullptr)
        {
            cout << "编译错误" << endl;
            return 0;
        }
        codeBlock->parentBlock = rootBlock;
        p = p->siblings;
    }

    auto Filename = "main.ll";
    error_code EC;

    raw_fd_ostream dest(Filename, EC, sys::fs::F_None);
    if (EC)
    {
        errs() << "Could not open file: " << EC.message();
        return 1;
    }
    // TheModule->print(errs(), nullptr); // print IR to terminal
    module->print(dest, nullptr);

    delete module;

    return 0;
}