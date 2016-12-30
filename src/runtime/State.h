#pragma once
#include <unordered_map>
#include <memory>
#include "Cell.h"

#include "Function.h"
#include "GC.h"

namespace run {

struct Environment
{
    Environment ();

    std::unordered_map<std::string,
                       std::unique_ptr<Function>> functions;

    void load_std_lib ();

    Function* get_function (const std::string& name,
                            bool create_if_not_found = false);

    FunctionImpl& impl_function (const std::string& name, size_t argc);
};



struct State
{
    State ();

    Environment env;
    GC gc;
};


}
