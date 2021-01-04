//
// Created by Z_HAO on 2020/12/27.
//

#ifndef COMPILER_BUILD_H
#define COMPILER_BUILD_H

#include "ast.h"
#include <iostream>

AstNode *newNum(string _str)
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    int *_num = (int *)malloc(sizeof(int));
    *_num = atoi(_str.c_str());
    ret->data = _num;
    ret->nodeType = num;
    ret->arrayIndex = -1;
    return ret;
}

AstNode *newId(string _str)
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    ret->data = (char *)malloc(_str.length());
    strcpy((char *)ret->data , _str.c_str());
    //printf("%s" , ret->data);
    ret->nodeType = id;
    ret->arrayIndex = -1;
    return ret;
}

AstNode *newType(string _type)
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    char *temp = const_cast<char *>(_type.c_str());
    ret->data = new VarType(temp);
    ret->nodeType = type;
    ret->arrayIndex = -1;
    return ret;
}

AstNode *newToken(string _token)
{
    auto *ret = (AstNode*)malloc(sizeof(AstNode));
    ret->data = (char *)malloc(_token.length());
    strcpy((char *)ret->data , _token.c_str());
    ret->nodeType = token;
    ret->arrayIndex = -1;
    return ret;
}

AstNode *newToken(AstNode *_token , AstNode *_value)
{
    auto *ret = (AstNode*)malloc(sizeof(AstNode));
    ret->data = (char *)_token->data;
    ret->arrayIndex = *((int *)_value->data);
    ret->nodeType = token;
    return ret;
}

AstNode *addSibling(AstNode *t1 , AstNode *t2)
{
    if(t1 == nullptr)
    {
        t1 = new AstNode();
        t1->nodeType = t2->nodeType;
        t1->arrayIndex = -1;
    }
    return t1->addSibling(t2);
}

AstNode *addChild(AstNode *t1 , AstNode *t2)
{
    return t1->addChild(t2);
}

AstNode *newConstant(AstNode *_varType , AstNode *value)
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    auto constant1 = new Constant((VarType *)_varType->data , *((int *)value->data));
    ret->data = constant1;
    ret->nodeType = constant;
    ret->arrayIndex = -1;
    return ret;
}

AstNode *newVariable(AstNode *varType , AstNode *value)
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    auto variable1 = new Variable((VarType *)varType->data , (char *)value->data);
    ret->data = variable1;
    ret->nodeType = variable;
    ret->arrayIndex = -1;
    return ret;
}

AstNode *newVariable(AstNode *varType , AstNode *value , AstNode *arrayIndex)
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    Variable *variable1;
    if(arrayIndex == nullptr)
    {
        variable1 = new Variable((VarType *)varType->data , (char *)value->data , -1);
    }
    else
    {
        variable1 = new Variable((VarType *)varType->data , (char *)value->data , *((int *)arrayIndex->data));
        ret->arrayIndex = *((int *)arrayIndex->data);
    }
    ret->data = variable1;
    ret->nodeType = variable;
    return ret;
}

AstNode *newCondition(AstNode *left , AstNode *_operators , AstNode *right)
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    auto condition1 = new Condition();
    auto operators = (char *)_operators->data;
    if(operators == nullptr)
        condition1->conditionType = SingleCondition;
    else if(strcmp(operators , ">") == 0)
        condition1->conditionType = Greater;
    else if(strcmp(operators , "<") == 0)
        condition1->conditionType = Less;
    else if(strcmp(operators , ">=") == 0)
        condition1->conditionType = GreaterEqual;
    else if(strcmp(operators , "<=") == 0)
        condition1->conditionType = LessEqual;
    else if(strcmp(operators , "==") == 0)
        condition1->conditionType = Equal;
    else if(strcmp(operators , "!=") == 0)
        condition1->conditionType = NotEqual;
    else if(strcmp(operators , "=") == 0)
        condition1->conditionType = AssignCondition;
    else
        condition1->conditionType = ErrorCondition;
    condition1->leftCondition = left;
    condition1->rightCondition = right;
    ret->data = condition1;
    ret->nodeType = condition;
    return ret;
}

AstNode *newIf(AstNode *condition , AstNode *ifBody , AstNode *elseBody) //没有else时传入nullptr
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    auto stmt_if1 = new Stmt_if();
    stmt_if1->condition = condition;
    stmt_if1->if_body = ifBody;
    stmt_if1->else_body = elseBody;
    ret->data = stmt_if1;
    ret->nodeType = stmt_if;
    return ret;
}

AstNode *newWhile(AstNode *condition , AstNode *whileBody)
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    auto stmt_while1 = new Stmt_while();
    stmt_while1->condition = condition;
    stmt_while1->while_body = whileBody;
    ret->data = stmt_while1;
    ret->nodeType = stmt_while;
    return ret;
}

AstNode *newReturn(AstNode *returnValue)
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    auto stmt_return1 = new Stmt_return();
    stmt_return1->returnValue = returnValue;
    ret->data = stmt_return1;
    ret->nodeType = stmt_return;
    return ret;
}

AstNode *newOperators(AstNode *left , AstNode *_operators , AstNode *right)
{
    //cout << left->nodeType << ' ' << _operators->nodeType << ' ' << right->nodeType << endl;
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    auto operator1 = new Operators();
    operator1->left = left;
    operator1->right = right;
    char __operators = ((char *)_operators->data)[0];
    switch (__operators)
    {
        case '+':
            operator1->operatorsType = Plus;
            break;
        case '-':
            operator1->operatorsType = Minus;
            break;
        case '*':
            operator1->operatorsType = Multi;
            break;
        case '/':
            operator1->operatorsType = Divide;
            break;
        case '=':
            operator1->operatorsType = Assign;
            break;
        default:
            operator1->operatorsType = Error;
            break;
    }
    ret->data = operator1;
    ret->nodeType = operators;
    return ret;
}

AstNode *addArgs(AstNode *_args , AstNode *variable1)
{
    if(_args == nullptr)
    {
        _args = (AstNode*)malloc(sizeof(AstNode));
        _args->data = new Args();
        _args->nodeType = args;
    }
    ((Args *)_args->data)->addArgs(variable1);
    return _args;
}

AstNode *newFunction(AstNode *funcType , AstNode *funcName , AstNode *args , AstNode *funcStmt)
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    auto func = new Func_call();
    if(funcType != nullptr)
        func->returnType = (VarType *)funcType->data;
    //printf("%s" , (char *)funcName->data);
    func->name = (char *)malloc(strlen((char *)funcName->data));

    printf("函数名: %s\n", ((char *)funcName->data));

    strcpy(func->name , ((char *)funcName->data));
    if(args != nullptr && args->data != nullptr)
        func->args = (Args *)args->data;
    func->func_stmt = funcStmt;
    ret->data = func;
    if(funcStmt == nullptr)
        ret->nodeType = beFunctioned;
    else
        ret->nodeType = NodeType::function;
    return ret;
}

AstNode *newFunctionStmt(AstNode *t1 , AstNode *t2)
{
    auto ret = (AstNode*)malloc(sizeof(AstNode));
    auto func = new Func_stmt();
    func->local_declarations = t1;
    func->statement_list = t2;
    ret->data = func;
    ret->nodeType = stmt_func;
    return ret;
}

AstNode *newLocalDeclare(AstNode *t1 , AstNode *t2)
{
    if(t1 == nullptr)
    {
        //cout << "t1 is nullptr" << endl;
        auto *ret = (AstNode*)malloc(sizeof(AstNode));
        ret->nodeType = local_declare;
        return ret;
    }
    else
        return t1->addSibling(t2);
}

AstNode *newStateDeclare(AstNode *t1 , AstNode *t2)
{
    if(t1 == nullptr)
    {
        auto *ret = (AstNode*)malloc(sizeof(AstNode));
        ret->nodeType = state_declare;
        return ret;
    }
    else
        return t1->addSibling(t2);
}

#endif //COMPILER_BUILD_H
