//
// Created by Z_HAO on 2020/12/26.
//

#ifndef COMPILER_ASR_H
#define COMPILER_ASR_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unordered_map>
using namespace std;

//0-12
//13-
enum NodeType{error , root , token , id , num , type , variable , function , beFunctioned , constant , operators , condition , args ,
    stmt_while , stmt_if , stmt_return , stmt_func , local_declare , state_declare};

enum Type {Int , Dou , Void , ErrorType};
enum ConditionType {Less , Greater , LessEqual , GreaterEqual , Equal , NotEqual , SingleCondition , AssignCondition , ErrorCondition};
enum ReturnType {cons , var , express};
enum OperatorsType {Plus , Minus , Multi , Divide , Assign , Error};

struct VarType
{
    Type type;

    VarType() = default;

    explicit VarType(char *_type)
    {
        if(_type == nullptr)
            type = ErrorType;
        else if(strcmp(_type , "int") == 0)
            type = Int;
        else if(strcmp(_type , "void") == 0)
            type = Void;
        else if(strcmp(_type , "double") == 0)
            type = Dou;
        else
            type = ErrorType;
    }
};

struct Variable
{
    VarType *varType;
    char *variable;
    bool isArray;       // 判断变量是否是数组类型
    int arrayIndex = 0;

    Variable(VarType *_varType , char *_variable)
    {
        isArray = false;
        varType = _varType;
        variable = (char*)malloc(strlen(_variable));
        strcpy(variable , _variable);
    }

    Variable(VarType *_varType , char *_variable , int _arrayIndex)
    {
        isArray = true;
        varType = _varType;
        variable = (char*)malloc(strlen(_variable));
        arrayIndex = _arrayIndex;
        strcpy(variable , _variable);
    }
};

struct Constant
{
    VarType *varType;
    void *value;

    explicit Constant(VarType *_varType , int _value)
    {
        varType = _varType;
        value = (int*)malloc(sizeof(int));
        *(int*)value = _value;
    }
    explicit Constant(VarType *_varType , double _value)
    {
        varType = _varType;
        value = (double*)malloc(sizeof(double));
        *(double*)value = _value;
    }
};

struct AstNode
{
    // 采用树数据结构存储抽象语法树
    AstNode *siblings = nullptr;      //指向下一个兄弟节点
    AstNode *children = nullptr;      //指向第一个孩子节点
    AstNode *parent = nullptr;        //指向父亲节点
    int childNumber = 0;              //孩子节点的数量
    NodeType nodeType = error;        //节点类型
    void *data = nullptr;             //存储节点信息，void* 可以被转换为任何类型
    int arrayIndex = -1;

    AstNode();
    static AstNode *newNode();
    AstNode *addChild(AstNode *child);
    AstNode *addSibling(AstNode *sibling);
};

struct Condition
{
    ConditionType conditionType;
    AstNode *leftCondition;
    AstNode *rightCondition;
};

struct Operators
{
    OperatorsType operatorsType;
    AstNode *left;
    AstNode *right;
};

struct Stmt_if
{
    AstNode *condition;
    AstNode *if_body;
    AstNode *else_body;
};

struct Stmt_while
{
    AstNode *condition;
    AstNode *while_body;
};

struct Stmt_return
{
    AstNode *returnValue;
};

struct Args
{
    vector<AstNode *> argsList;

    void addArgs(AstNode *variable1)
    { argsList.push_back(variable1); }
};

struct Func_call
{
    VarType *returnType;
    char *name = nullptr;
    Args *args = new Args();
    AstNode *func_stmt = nullptr;
};

struct Func_stmt
{
    AstNode *local_declarations;
    AstNode *statement_list;
};

AstNode::AstNode() = default;

AstNode *AstNode::newNode()
{
    auto astNode = (AstNode*)malloc(sizeof(AstNode));
    return astNode;
}

AstNode *AstNode::addChild(AstNode *child)
{
    if(this->children == nullptr)
    {
        //cout << child << endl;
        this->children = child;
        //cout << this->children << endl;
        this->childNumber ++;
        return this;
    }
    AstNode *pos = this->children;
    for(;pos->siblings != nullptr;pos = pos->siblings);
    pos->siblings = child;
    child->parent = this;
    pos->childNumber ++;
    return this;
}

AstNode *AstNode::addSibling(AstNode *sibling)
{
    if(this->siblings == nullptr)
    {
        this->siblings = sibling;
        siblings->parent = this->parent;
        if(this->parent != nullptr)
        {
            this->parent->childNumber ++;
        }
        return this;
    }
    AstNode *pos = this->siblings;
    for(;pos->siblings != nullptr;pos = pos->siblings);
    pos->siblings = sibling;
    sibling->parent = this->parent;
    if(pos->parent != nullptr)
    {
        pos->parent->childNumber ++;
    }
    return this;
}

AstNode *newRoot(AstNode *root)
{
    auto ret = (AstNode *)malloc(sizeof(AstNode));
    ret->children = root;
    ret->childNumber ++;
    ret->nodeType = NodeType::root;
    return ret;
}

#endif //COMPILER_ASR_H
