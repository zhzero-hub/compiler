//
// Created by Z_HAO on 2020/12/29.
//

#ifndef COMPILER_UTIL_H
#define COMPILER_UTIL_H

#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Constants.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/Debug.h>

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>

#include "build.h"
using namespace llvm;

static LLVMContext TheContext;

unordered_map<string , Value*> valueMap;      // 符号表

unordered_map<string , Function*> functionMap;


struct CodeBlock
{
    BasicBlock *block;
    IRBuilder<> *irBuilder;
    void *returnValue;
    unordered_map<string, Value *> locals;
    unordered_map<string , Value *> args;
    CodeBlock *nextBlock;
    CodeBlock *parentBlock;
};

Module *newModule()
{
    auto module = new Module("main" , TheContext);
    module->setDataLayout("e-m:w-i64:64-f80:128-n8:16:32:64-S128");
    module->setTargetTriple("x86_64-pc-windows-msvc19.13.26129");
    // experiment with module flags
    module->addModuleFlag(Module::ModFlagBehavior::Error, "wchar_size", 2);
    module->setPICLevel(PICLevel::Level::BigPIC);
    MDString *Elts[] = {
            MDString::get(module->getContext(), "clang version 11.0.0")
    };
    MDNode *Node = MDNode::get(module->getContext(), ArrayRef<Metadata *>(Elts[0]));

    NamedMDNode *NMD = module->getOrInsertNamedMetadata("llvm.ident");
    NMD->addOperand(Node);
    return module;
}

llvm::Type *getType(VarType *type)
{
    switch (type->type)
    {
        case Int:
            return (llvm::Type::getInt32Ty(TheContext));
        case Dou:
            return (llvm::Type::getDoubleTy(TheContext));
        case Void:
            return (llvm::Type::getVoidTy(TheContext));
        case ErrorType:
            return nullptr;
    }
}

Value *findValue(CodeBlock *codeBlock , const string& name , bool &isArgs)
{
    auto *p = codeBlock;
    while(p != nullptr)
    {
        auto it = p->locals.find(name);
        if(it != p->locals.end())
            return it->second;
        else
        {
            it = p->args.find(name);
            if(it != p->locals.end())
            {
                isArgs = true;
                return it->second;
            }
        }
        p = p->parentBlock;
    }
    auto it_g = valueMap.find(name);
    if(it_g != valueMap.end())
        return it_g->second;
    cout << name << "在符号表中未定义" << endl;
    return nullptr;
}

Value *findValue(CodeBlock *codeBlock , char *name , bool &isArgs)
{
    string _name(name);
    return findValue(codeBlock , _name , isArgs);
}

llvm::Function *get_printf(llvm::Module *mod)
{
    const char *fun_name = "printf";
    llvm::Function *func = mod->getFunction(fun_name);
    if (func == nullptr)
    {
        llvm::FunctionType *func_type = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(mod->getContext()),
                {llvm::Type::getInt8PtrTy(mod->getContext())},
                true);
        func = llvm::Function::Create(func_type, llvm::GlobalValue::ExternalLinkage, fun_name, mod);
    }
    return func;
}

#endif //COMPILER_UTIL_H
