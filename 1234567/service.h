//
// Created by Z_HAO on 2020/12/29.
//

#ifndef COMPILER_SERVICE_H
#define COMPILER_SERVICE_H

#include "util.h"

Module *module = new Module("main.ll", TheContext);;

CodeBlock *mainBlock = new CodeBlock();

auto printf_ptr = get_printf(module);

Value *intGen(AstNode *root)
{
    return ConstantInt::get(llvm::Type::getInt32Ty(TheContext), *((int *)root->data));
}

Value *intGen(int data)
{
    return ConstantInt::get(llvm::Type::getInt32Ty(TheContext), data);
}

Value *charGen(char *name)
{
    return llvm::ConstantDataArray::getString(TheContext, name);
}

CodeBlock *ifThenGen(Function *func)
{
    //创建一个if.then代码块
    //func表示if所在的函数指针
    auto codeBlock = new CodeBlock();
    codeBlock->block = BasicBlock::Create(TheContext, "if_then", func);
    codeBlock->irBuilder = new IRBuilder<>(codeBlock->block);
    return codeBlock;
}

CodeBlock *ifElseGen(Function *func)
{
    //创建一个if.else代码块
    //func表示if所在的函数指针
    auto codeBlock = new CodeBlock();
    codeBlock->block = BasicBlock::Create(TheContext, "if_else", func);
    codeBlock->irBuilder = new IRBuilder<>(codeBlock->block);
    return codeBlock;
}

CodeBlock *ifEndGen(Function *func)
{
    //创建一个if.condition代码块
    //func表示if所在的函数指针
    auto codeBlock = new CodeBlock();
    codeBlock->block = BasicBlock::Create(TheContext, "if_end", func);
    codeBlock->irBuilder = new IRBuilder<>(codeBlock->block);
    return codeBlock;
}

CodeBlock *functionGen(AstNode *root)
{
    auto codeBlock = new CodeBlock();
    auto func = (Func_call *)root->data;
    const int argsSize = func->args->argsList.size();

    //声明一个函数-----------------------------------
    SmallVector<llvm::Type *, 2> functionArgs;
    //各种类型都是通过get方法来构造对应的实例
    for(int i = 0;i < argsSize;i ++)
    {
        auto variable1 = (Variable *)func->args->argsList[i]->data;
        auto value = getType(variable1->varType);
        functionArgs.push_back(value); //32为整型参数Tina及到vector数组中
    }

    llvm::Type *returnType = getType(func->returnType);//返回值类型

    FunctionType *func_type = FunctionType::get(returnType, functionArgs, false);

    printf("共%d个参数, 返回类型: %d, 函数名: %s\n" ,argsSize, func->returnType->type, func->name);

    Function *fun = Function::Create(func_type , GlobalValue::ExternalLinkage , func->name , module);
    int ii = 0;
    for(auto it = fun->arg_begin();it != fun->arg_end();it ++ , ii ++)
    {
        Value *value = it;
        auto variable1 = (Variable *)func->args->argsList[ii]->data;
        printf("参数名: %s\n" , variable1->variable);
        value->setName(variable1->variable);
        string v_name = variable1->variable;
        pair<string , Value*> pair(variable1->variable , value);
        codeBlock->args.insert(pair);
        cout << v_name << ' ' << value << "入形参符号表" << endl;
    }
    pair<string , Function*>pair(func->name , fun);
    functionMap.insert(pair);
    codeBlock->block = BasicBlock::Create(TheContext, "entry", fun);
    codeBlock->irBuilder = new IRBuilder<>(codeBlock->block);
    return codeBlock;//创建builder实例，每个basicblock对应一个irbulider
}

Value *beFunctionedGen(CodeBlock *block , AstNode *root)
{
    CodeBlock *blockGen(CodeBlock *codeBlock , AstNode *root);
    cout << "进入被调用函数生成" << endl;
    auto func = (Func_call *)root->data;
    cout << func->args << endl;
    const int argsSize = func->args->argsList.size();
    printf("函数名: %s\n" , func->name);
    cout << "共有" << argsSize << "个参数" << endl;
    auto argsList = func->args->argsList;
    if(strcmp(func->name , "output") == 0)
    {
        cout << "调用printf函数" << endl;
        cout << argsList[0]->nodeType << endl;
        string name = (char *)argsList[0]->data;
        if(argsList[0]->arrayIndex != -1)
        {
            name = name + string("[") + to_string(argsList[0]->arrayIndex) + string("]");
        }
        cout << "输出变量: " << name << endl;
        bool isArgs = false;
        auto _var = findValue(block , name , isArgs);
        if(_var == nullptr)
        {
            cout << "printf输出变量未定义\n" << endl;
        }
        auto var = block->irBuilder->CreateLoad(_var);

        //输出int变量
        block->irBuilder->CreateCall(printf_ptr,
                                     {block->irBuilder->CreateGlobalStringPtr("value = %d\n"), var});
        return intGen(1);
    }
    std::vector<Value *> putArgs;
    for(int i = 0;i < argsSize;i ++)
    {
        if(argsList[i]->nodeType == NodeType::variable)
        {
            string name = (char *)argsList[i]->data;
            if(argsList[i]->arrayIndex != -1)
            {
                name = name + string("[") + to_string(argsList[i]->arrayIndex) + string("]");
            }
            bool isArgs = false;
            auto value = findValue(block , name , isArgs);
            cout << "参数名: " << name << endl;
            if(value == nullptr)
            {
                cout << "未定义的参数" << endl;
                return nullptr;
            }
            else
            {
                if(isArgs)
                {
                    cout << "形参作为参数" << endl;
                    putArgs.push_back(value);
                }
                else
                {
                    cout << "变量作为参数" << endl;
                    auto var = block->irBuilder->CreateLoad(value);
                    putArgs.push_back(var);
                }
            }
        }
        else if(argsList[i]->nodeType == NodeType::num)
        {
            putArgs.push_back(ConstantInt::get(llvm::Type::getInt32Ty(TheContext), *((int *)argsList[i]->data)));
        }
        else if(argsList[i]->nodeType == NodeType::id || argsList[i]->nodeType == NodeType::token)
        {
            bool isArgs = false;
            string name = (char *)argsList[i]->data;
            if(argsList[i]->arrayIndex != -1)
            {
                name = name + string("[") + to_string(argsList[i]->arrayIndex) + string("]");
            }
            auto value = findValue(block , name , isArgs);
            cout << "参数名: " << name << endl;
            if(value == nullptr)
            {
                cout << "未定义的参数" << endl;
                return nullptr;
            }
            else
            {
                if(isArgs)
                {
                    cout << "形参作为参数" << endl;
                    putArgs.push_back(value);
                }
                else
                {
                    cout << "变量作为参数" << endl;
                    auto var = block->irBuilder->CreateLoad(value);
                    putArgs.push_back(var);
                }
            }
        }
        else if(argsList[i]->nodeType == NodeType::operators)
        {
            block = blockGen(block , argsList[i]);
            if(block == nullptr || block->returnValue == nullptr)
            {
                cout << "参数运算错误" << endl;
                return nullptr;
            }
            putArgs.push_back((Value *)block->returnValue);
        }
        else
        {
            cout << "参数类型: " << argsList[i]->nodeType << endl;
        }
    }
    ArrayRef<Value*> argsRef(putArgs);
    auto func_p = functionMap.find(func->name);
    if(func_p != functionMap.end())
    {
        return block->irBuilder->CreateCall(func_p->second, argsRef);
    }
    else
    {
        cout << "函数名未定义" << endl;
        return nullptr;
    }
}

CodeBlock *blockGen(CodeBlock *codeBlock , AstNode *root)
{
    if(root == nullptr)
    {
        cout << "当前节点为空指针" << endl;
        return codeBlock;
    }
    else if(root->nodeType == NodeType::error)
    {
        cout << "Node Type error" << endl;
        return nullptr;
    }
    else if(root->nodeType == NodeType::function)
    {
        auto block = functionGen(root);
        block->parentBlock = codeBlock;
        block = blockGen(block , ((Func_call *)root->data)->func_stmt);
        if(codeBlock == nullptr)
        {
            cout << "函数体错误" << endl;
            return nullptr;
        }
        return block;
    }
    else if(root->nodeType == NodeType::stmt_func)
    {
        cout << "进入function stmt" << endl;
        auto fun_stmt = (Func_stmt *)root->data;
        if(fun_stmt->local_declarations != nullptr)
            codeBlock = blockGen(codeBlock , fun_stmt->local_declarations);
        if(codeBlock == nullptr)
        {
            cout << "函数内变量声明错误" << endl;
            return nullptr;
        }
        cout << fun_stmt->statement_list->nodeType << endl;
        if(fun_stmt->statement_list != nullptr)
            codeBlock = blockGen(codeBlock , fun_stmt->statement_list);
        if(codeBlock == nullptr)
        {
            cout << "函数体错误" << endl;
            return nullptr;
        }
        return codeBlock;
    }
    else if(root->nodeType == NodeType::local_declare)
    {
        auto p = root->siblings;
        while(p != nullptr)
        {
            cout << "local declare: " << p->nodeType << endl;
            codeBlock = blockGen(codeBlock , p);
            if(codeBlock == nullptr)
            {
                cout << "函数过程中变量声明错误" << endl;
            }
            p = p->siblings;
        }
    }
    else if(root->nodeType == NodeType::state_declare)
    {
        auto p = root->siblings;
        while(p != nullptr)
        {
            cout << "进入state declare " << p << endl;
            codeBlock = blockGen(codeBlock , p);
            if(codeBlock == nullptr)
            {
                cout << "函数体过程中错误" << endl;
            }
            p = p->siblings;
        }
        return codeBlock;
    }
    else if(root->nodeType == NodeType::beFunctioned)
    {
        auto value = beFunctionedGen(codeBlock , root);
        codeBlock->returnValue = value;
    }
    else if(root->nodeType == NodeType::variable)
    {
        auto variable1 = (Variable *)root->data;
        auto varType = variable1->varType;
        auto name = variable1->variable;
        codeBlock->returnValue = charGen(name);
        switch (varType->type)
        {
            case Int:
            {
                if(codeBlock->locals.find(name) != codeBlock->locals.end())
                {
                    printf("变量%s已经被定义\n" , name);
                    return nullptr;
                }
                if(!variable1->isArray)
                {
                    Value* alloca = codeBlock->irBuilder->CreateAlloca(llvm::Type::getInt32Ty(TheContext));
                    codeBlock->irBuilder->CreateStore(ConstantInt::get(IntegerType::getInt32Ty(TheContext), 0), alloca);
                    pair<string , Value*> pair1(name , alloca);
                    codeBlock->locals.insert(pair1);
                    printf("变量%s已入符号表\n" , name);
                }
                else
                {
                    for(int i = 0;i < variable1->arrayIndex;i ++)
                    {
                        string _name = name + string("[") + to_string(i) + string("]");
                        Value* alloca = codeBlock->irBuilder->CreateAlloca(llvm::Type::getInt32Ty(TheContext));
                        codeBlock->irBuilder->CreateStore(ConstantInt::get(IntegerType::getInt32Ty(TheContext), 0), alloca);
                        pair<string , Value*> _pair(_name , alloca);
                        codeBlock->locals.insert(_pair);
                        printf("变量%s已入符号表\n" , _name.c_str());
                    }
                }
                break;
            }
            case Dou:
            {
                pair<string , Value*> pair(name , ConstantInt::get(llvm::Type::getDoubleTy(TheContext), 0));
                printf("变量%s已入符号表\n" , name);
                codeBlock->locals.insert(pair);
                break;
            }
            case Void:
                break;
            case ErrorType:
            {
                cout << "错误的数据类型" << endl;
                return nullptr;
            }
        }
    }
    else if(root->nodeType == NodeType::constant)
    {
        Value *value = intGen(root);
        codeBlock->returnValue = value;
        return codeBlock;
    }
    else if(root->nodeType == NodeType::token)
    {
        string name((char *)root->data);
        if(root->arrayIndex != -1)
        {
            name = name + string("[") + to_string(root->arrayIndex) + string("]");
        }
        cout << "token: " << name << endl;
        bool isArgs = false;
        auto i = findValue(codeBlock , name , isArgs);
        if(!isArgs)
            codeBlock->returnValue = codeBlock->irBuilder->CreateLoad(i);
        else
            codeBlock->returnValue = i;
        return codeBlock;
    }
    else if(root->nodeType == NodeType::id)
    {
        string name((char *)root->data);
        if(root->arrayIndex != -1)
        {
            name = name + string("[") + to_string(root->arrayIndex) + string("]");
        }
        cout << "id: " << name << endl;
        bool isArgs = false;
        auto i = findValue(codeBlock , name , isArgs);
        if(!isArgs)
            codeBlock->returnValue = codeBlock->irBuilder->CreateLoad(i);
        else
            codeBlock->returnValue = i;
        return codeBlock;
    }
    else if(root->nodeType == NodeType::num)
    {
        cout << "整形数字: " << *((int *)root->data) << endl;
        codeBlock->returnValue = intGen(*((int *)root->data));
        return codeBlock;
    }
    else if(root->nodeType == NodeType::stmt_while)
    {
        auto while_count = new CodeBlock();
        auto while_body = new CodeBlock();
        auto while_end = new CodeBlock();
        while_count->block = BasicBlock::Create(TheContext, "while_count", codeBlock->block->getParent());
        while_body->block = BasicBlock::Create(TheContext, "while_body", codeBlock->block->getParent());
        while_end->block = BasicBlock::Create(TheContext, "while_end", codeBlock->block->getParent());
        while_count->irBuilder = new IRBuilder<>(while_count->block);
        while_body->irBuilder = new IRBuilder<>(while_body->block);
        while_end->irBuilder = new IRBuilder<>(while_end->block);
        while_count->parentBlock = codeBlock;
        while_body->parentBlock = codeBlock;
        while_end->parentBlock = codeBlock;

        //cout << "success" << endl;
        auto stmt_while = (Stmt_while *)root->data;
        while_count = blockGen(while_count , stmt_while->condition);
        if(while_count == nullptr || while_count->returnValue == nullptr)
        {
            cout << "while condition错误" << endl;
        }
        codeBlock->irBuilder->CreateBr(while_count->block);
        //while_count基本块
        codeBlock->irBuilder->SetInsertPoint(while_count->block);
        //根据cmp的值跳转，也就是if条件
        codeBlock->irBuilder->CreateCondBr((Value *)while_count->returnValue, while_body->block, while_end->block);

        while_body = blockGen(while_body , stmt_while->while_body);
        if(while_body == nullptr)
        {
            cout << "while 循环体错误" << endl;
        }
        while_body->irBuilder->CreateBr(while_count->block);
        codeBlock->irBuilder->SetInsertPoint(while_body->block);
        codeBlock->irBuilder->SetInsertPoint(while_end->block);
        return while_end;
    }
    else if(root->nodeType == NodeType::stmt_if)
    {
        cout << "进入stmt if模块" << endl;
        auto stmt = (Stmt_if *)root->data;
        auto endBlock = ifEndGen(codeBlock->block->getParent());
        endBlock->parentBlock = codeBlock;
        CodeBlock *ifBlock = ifThenGen(codeBlock->block->getParent());
        ifBlock->parentBlock = codeBlock;
        ifBlock = blockGen(ifBlock , stmt->if_body);
        if(ifBlock == nullptr)
        {
            cout << "If 模块错误" << endl;
            return nullptr;
        }
        BranchInst::Create(endBlock->block, ifBlock->block);


        CodeBlock *elseBlock = ifElseGen(codeBlock->block->getParent());
        elseBlock->parentBlock = codeBlock;
        elseBlock = blockGen(elseBlock , stmt->else_body);
        if(elseBlock == nullptr)
        {
            cout << "Else 模块错误" << endl;
            return nullptr;
        }
        BranchInst::Create(endBlock->block, elseBlock->block);

        codeBlock = blockGen(codeBlock , stmt->condition);
        if(codeBlock == nullptr)
        {
            cout << "Condition 模块错误" << endl;
            return nullptr;
        }
        BranchInst::Create(ifBlock->block, elseBlock->block, (Value *)codeBlock->returnValue, codeBlock->block);
        codeBlock->irBuilder->SetInsertPoint(endBlock->block);
        return endBlock;
    }
    else if(root->nodeType == NodeType::stmt_return)
    {
        auto return_stmt = (Stmt_return *)root->data;
        if(return_stmt->returnValue != nullptr)
        {
            cout << "数组序号: " << return_stmt->returnValue->arrayIndex << endl;
            switch (return_stmt->returnValue->nodeType)
            {
                case NodeType::token:
                {
                    string name = (char *)return_stmt->returnValue->data;
                    if(return_stmt->returnValue->arrayIndex != -1)
                    {
                        name = name + string("[") + to_string(return_stmt->returnValue->arrayIndex) + string("]");
                    }
                    bool isArgs = false;
                    auto ret = findValue(codeBlock , name , isArgs);
                    if(ret != nullptr)
                    {
                        if(!isArgs)
                        {
                            auto var = codeBlock->irBuilder->CreateLoad(ret);
                            codeBlock->irBuilder->CreateRet(var);
                        }
                        else
                        {
                            codeBlock->irBuilder->CreateRet(ret);
                        }
                    }
                    else
                    {
                        cout << "Return token变量未定义" << endl;
                        return nullptr;
                    }
                    break;
                }
                case NodeType::id:
                {
                    string name = (char *)return_stmt->returnValue->data;
                    if(return_stmt->returnValue->arrayIndex != -1)
                    {
                        name = name + string("[") + to_string(return_stmt->returnValue->arrayIndex) + string("]");
                    }
                    bool isArgs = false;
                    auto ret = findValue(codeBlock , name , isArgs);
                    if(ret != nullptr)
                    {
                        if(!isArgs)
                        {
                            auto var = codeBlock->irBuilder->CreateLoad(ret);
                            codeBlock->irBuilder->CreateRet(var);
                        }
                        else
                        {
                            codeBlock->irBuilder->CreateRet(ret);
                        }
                    }
                    else
                    {
                        cout << "Return id变量未定义" << endl;
                        return nullptr;
                    }
                    break;
                }
                case NodeType::num:
                {
                    cout << "return类型为数字" << endl;
                    codeBlock->irBuilder->CreateRet(intGen(*((int *)return_stmt->returnValue->data)));
                    break;
                }
                case NodeType::operators:
                {
                    codeBlock = blockGen(codeBlock , return_stmt->returnValue);
                    if(codeBlock == nullptr)
                    {
                        cout << "Return Value错误" << endl;
                        return nullptr;
                    }
                    if(codeBlock->returnValue != nullptr)
                        codeBlock->irBuilder->CreateRet((Value *)codeBlock->returnValue);
                    else
                        return codeBlock;
                    break;
                }
                default:
                {
                    cout << "Return 类型错误: " << return_stmt->returnValue->nodeType << endl;
                    return nullptr;
                }
            }
        }
        return codeBlock;
    }
    else if(root->nodeType == NodeType::operators)
    {
        auto _operator = (Operators *)root->data;
        switch (_operator->operatorsType)
        {
            case OperatorsType::Plus:
            {
                codeBlock = blockGen(codeBlock , _operator->left);
                if(codeBlock == nullptr)
                {
                    cout << "Operator 左侧条件错误" << endl;
                    return nullptr;
                }
                auto leftValue = (Value *)codeBlock->returnValue;
                codeBlock = blockGen(codeBlock , _operator->right);
                if(codeBlock == nullptr)
                {
                    cout << "Operator 右侧条件错误" << endl;
                    return nullptr;
                }
                auto rightValue = (Value *)codeBlock->returnValue;
                if(leftValue == nullptr || rightValue == nullptr)
                {
                    cout << "Operator 条件错误" << endl;
                    return nullptr;
                }
                codeBlock->returnValue = codeBlock->irBuilder->CreateAdd(leftValue , rightValue);
                break;
            }
            case OperatorsType::Minus:
            {
                codeBlock = blockGen(codeBlock , _operator->left);
                if(codeBlock == nullptr)
                {
                    cout << "Operator 左侧条件错误" << endl;
                    return nullptr;
                }
                auto leftValue = (Value *)codeBlock->returnValue;
                codeBlock = blockGen(codeBlock , _operator->right);
                if(codeBlock == nullptr)
                {
                    cout << "Operator 右侧条件错误" << endl;
                    return nullptr;
                }
                auto rightValue = (Value *)codeBlock->returnValue;
                if(leftValue == nullptr || rightValue == nullptr)
                {
                    cout << "Operator 条件错误" << endl;
                    return nullptr;
                }
                codeBlock->returnValue = codeBlock->irBuilder->CreateSub(leftValue , rightValue);
                break;
            }
            case OperatorsType::Multi:
            {
                codeBlock = blockGen(codeBlock , _operator->left);
                if(codeBlock == nullptr)
                    return nullptr;
                auto leftValue = (Value *)codeBlock->returnValue;
                codeBlock = blockGen(codeBlock , _operator->right);
                if(codeBlock == nullptr)
                    return nullptr;
                auto rightValue = (Value *)codeBlock->returnValue;
                if(leftValue == nullptr || rightValue == nullptr)
                    return nullptr;
                codeBlock->returnValue = codeBlock->irBuilder->CreateMul(leftValue , rightValue);
                break;
            }
            case OperatorsType::Divide:
            {
                codeBlock = blockGen(codeBlock , _operator->left);
                if(codeBlock == nullptr)
                {
                    cout << "Operator 左侧条件错误" << endl;
                    return nullptr;
                }
                auto leftValue = (Value *)codeBlock->returnValue;
                codeBlock = blockGen(codeBlock , _operator->right);
                if(codeBlock == nullptr)
                {
                    cout << "Operator 右侧条件错误" << endl;
                    return nullptr;
                }
                auto rightValue = (Value *)codeBlock->returnValue;
                if(leftValue == nullptr || rightValue == nullptr)
                {
                    cout << "Operator 条件错误" << endl;
                    return nullptr;
                }
                codeBlock->returnValue = codeBlock->irBuilder->CreateSDiv(leftValue , rightValue);
                break;
            }
            case OperatorsType::Assign:
            {
                cout << "operator left: " << _operator->left->nodeType << endl;
                codeBlock = blockGen(codeBlock , _operator->left);
                if(codeBlock == nullptr)
                {
                    cout << "赋值左侧错误" << endl;
                    return nullptr;
                }
                string _name = (char *)_operator->left->data;
                if(_operator->left->arrayIndex != -1)
                {
                    _name = _name + string("[") + to_string(_operator->left->arrayIndex) + string("]");
                }
                //printf("name: %s" , name);
                cout << "operator right: " << _operator->right->nodeType << endl;
                codeBlock = blockGen(codeBlock , _operator->right);
                if(codeBlock == nullptr)
                {
                    cout << "赋值右侧错误" << endl;
                    return nullptr;
                }
                auto rightValue = (Value *)codeBlock->returnValue;
                if(rightValue == nullptr)
                {
                    cout << "赋值错误" << endl;
                    return nullptr;
                }
                auto p = codeBlock;
                while(p != nullptr)
                {
                    cout << "本地符号表个数" << p->locals.size() << endl;
                    auto it = p->locals.find(_name);
                    if (it != p->locals.end()) {
                        codeBlock->irBuilder->CreateStore(rightValue , p->locals.find(_name)->second);
                        //p->locals.find(_name)->second = rightValue;
                        codeBlock->returnValue = intGen(1);
                        cout << _name << "赋值成功" << endl;
                        break;
                    } else {
                        it = p->args.find(_name);
                        if(it != p->args.end())
                        {
                            p->args.find(_name)->second = rightValue;
                            codeBlock->returnValue = intGen(1);
                            cout << _name << "赋值成功" << endl;
                            break;
                        }
                        else
                            p = p->parentBlock;
                    }
                }
                if(p == nullptr)
                {
                    auto it_g = valueMap.find(_name);
                    if(it_g != valueMap.end())
                    {
                        codeBlock->irBuilder->CreateStore(rightValue , p->locals.find(_name)->second);
                        //valueMap.find(_name)->second = rightValue;
                        codeBlock->returnValue = intGen(1);
                        cout << "赋值成功" << endl;
                    }
                    else
                    {
                        printf("变量: %s未定义\n" , _name.c_str());
                        return nullptr;
                    }
                }
                break;
            }
            case OperatorsType::Error:
            {
                cout << "操作符类型错误" << endl;
                return nullptr;
            }
        }
    }
    else if(root->nodeType == NodeType::condition)
    {
        auto condition = (Condition *)root->data;
        switch (condition->conditionType)
        {
            case Less:
            {
                codeBlock = blockGen(codeBlock , condition->leftCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto leftValue = (Value *)codeBlock->returnValue;
                codeBlock = blockGen(codeBlock , condition->rightCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto rightValue = (Value *)codeBlock->returnValue;
                codeBlock->returnValue = codeBlock->irBuilder->CreateICmpSLT(leftValue , rightValue);
                break;
            }
            case Greater:
            {
                codeBlock = blockGen(codeBlock , condition->leftCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto leftValue = (Value *)codeBlock->returnValue;
                codeBlock = blockGen(codeBlock , condition->rightCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto rightValue = (Value *)codeBlock->returnValue;
                codeBlock->returnValue = codeBlock->irBuilder->CreateICmpSGT(leftValue , rightValue);
                break;
            }
            case LessEqual:
            {
                codeBlock = blockGen(codeBlock , condition->leftCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto leftValue = (Value *)codeBlock->returnValue;
                codeBlock = blockGen(codeBlock , condition->rightCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto rightValue = (Value *)codeBlock->returnValue;
                cout << "less equal" << endl;
                codeBlock->returnValue = codeBlock->irBuilder->CreateICmpSLE(leftValue , rightValue);
                break;
            }
            case GreaterEqual:
            {
                codeBlock = blockGen(codeBlock , condition->leftCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto leftValue = (Value *)codeBlock->returnValue;
                codeBlock = blockGen(codeBlock , condition->rightCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto rightValue = (Value *)codeBlock->returnValue;
                codeBlock->returnValue = codeBlock->irBuilder->CreateICmpSGE(leftValue , rightValue);
                break;
            }
            case Equal:
            {
                codeBlock = blockGen(codeBlock , condition->leftCondition);
                if(codeBlock == nullptr)
                    return nullptr;

                auto leftValue = (Value *)codeBlock->returnValue;
                codeBlock = blockGen(codeBlock , condition->rightCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto rightValue = (Value *)codeBlock->returnValue;
                codeBlock->returnValue = codeBlock->irBuilder->CreateICmpEQ(leftValue , rightValue);
                break;
            }
            case NotEqual:
            {
                codeBlock = blockGen(codeBlock , condition->leftCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto leftValue = (Value *)codeBlock->returnValue;
                codeBlock = blockGen(codeBlock , condition->rightCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto rightValue = (Value *)codeBlock->returnValue;
                codeBlock->returnValue = codeBlock->irBuilder->CreateICmpNE(leftValue , rightValue);
                break;
            }
            case SingleCondition:
            {
                codeBlock = blockGen(codeBlock , condition->leftCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                codeBlock->returnValue = (Value *)codeBlock->returnValue;
                break;
            }
            case AssignCondition:
            {
                codeBlock = blockGen(codeBlock , condition->leftCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                char *name = (char *)codeBlock->returnValue;
                codeBlock = blockGen(codeBlock , condition->rightCondition);
                if(codeBlock == nullptr)
                    return nullptr;
                auto it = codeBlock->locals.find(name);
                if(it != codeBlock->locals.end())
                {
                    codeBlock->locals.find(name)->second = (Value *)codeBlock->returnValue;
                    codeBlock->returnValue = intGen(1);
                }
                else
                {
                    auto it_g = valueMap.find(name);
                    if(it_g != valueMap.end())
                    {
                        valueMap.find(name)->second = (Value *)codeBlock->returnValue;
                        codeBlock->returnValue = intGen(1);
                    }
                    else
                    {
                        codeBlock->returnValue = intGen(0);
                        return nullptr;
                    }
                }
                break;
            }
            case ErrorCondition:
                return nullptr;
        }
    }
    return codeBlock;
}

#endif //COMPILER_SERVICE_H
