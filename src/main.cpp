#include <iostream>
#include <utf8.h>
#include <boost/format.hpp>
#include "bytecode/Program.h"
#include "runtime/State.h"

int main (void)
{
    try {
        run::State state;
        auto plus = state.env.get_function("+");
        if (!plus) throw std::runtime_error("no standard `+' function");

        /* fn main () 4 + 5 end */
        bytecode::Program p_main(0);
        p_main.reg_count = 2;
        using I = bytecode::Instruction;
        auto& ins = p_main.instructions;
        ins.push_back(I::fxn(4));
        ins.push_back(I::store(0));
        ins.push_back(I::fxn(5));
        ins.push_back(I::store(1));
        ins.push_back(I::call(plus, 0, 2));
        ins.push_back(I::ret());

        auto ret = p_main.execute(&state, nullptr);
        if (ret.is_integer())
            std::cout << "output: " << ret.integer() << std::endl;
        else if (ret.is_null())
            std::cout << "output: null" << std::endl;
        else
            std::cout << "output: {type = " << int(ret.obj->type) << "}" << std::endl;
    }
    catch (std::runtime_error& err) {
        std::cerr << "error:" << std::endl
                  << err.what() << std::endl;
    }

    return 0;
}
