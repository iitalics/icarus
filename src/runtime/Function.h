#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include "Cell.h"

namespace ast {
struct FunctionDefn;
}


namespace run {

struct FunctionImpl;
struct State;
using NativeFnPtr = Cell (*)(State*,Cell*);

struct Function
{
    inline Function (std::string n)
        : name(std::move(n))
    {}

    std::string name;
    std::vector<FunctionImpl> implementations;
};


struct FunctionImpl
{
    inline explicit FunctionImpl (size_t argc)
        : arg_count(argc)
        , native_fn_ptr(nullptr)
        , to_be_compiled(nullptr)
    {}
    inline FunctionImpl (size_t argc,
                         NativeFnPtr impl)
        : arg_count(argc)
        , native_fn_ptr(impl)
        , to_be_compiled(nullptr)
    {}

    size_t arg_count;
    NativeFnPtr native_fn_ptr;
    ast::FunctionDefn* to_be_compiled;

    // TODO: compiled bytecode

    Cell call (State* state, Cell* args);
};




}
