#pragma once
#include "Instruction.h"
#include <vector>


namespace run {
struct State;
}


namespace bytecode {

struct Program
{
    Program (size_t argc)
        : arg_count(argc)
        , reg_count(argc)
    {}

    size_t arg_count;
    size_t reg_count;
    std::vector<Instruction> instructions;

    run::Cell execute (run::State* state, run::Cell* argv) const;
};

}
