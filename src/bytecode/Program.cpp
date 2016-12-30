#include "Program.h"
#include "../runtime/State.h"
#include <algorithm>
#include <boost/format.hpp>

namespace bytecode {

run::Cell Program::execute (run::State* state, run::Cell* argv) const
{
    /* create register, copy arguments */
    run::Cell acc = run::Cell::nil();
    std::vector<run::Cell> regs;
    regs.resize(reg_count, run::Cell::nil());
    for (size_t i = 0; i < arg_count; i++)
        regs[i] = argv[i];
    // TODO: add to GC roots


    /* interpret instructions */
    int ip = 0;
    bool ret = false;
    while (!ret) {
        auto ins = instructions[ip++];
        switch (ins.kind) {
        case Instruction::Fxn:
            acc = run::Cell::from_fixnum(ins.data.fxn);
            break;

        case Instruction::Load:
            acc = regs[ins.data.src];
            break;

        case Instruction::Store:
            regs[ins.data.dst] = acc;
            break;

        case Instruction::Call:
        case Instruction::Tail:
            {
                auto fn = ins.data.call.fn;
                auto& impls = fn->implementations;
                size_t argc = ins.data.call.argc;
                auto it = std::find_if(impls.begin(), impls.end(),
                                       [argc] (run::FunctionImpl& impl)
                                       { return impl.arg_count == argc; });
                if (it == impls.end()) {
                    auto fmt = boost::format
                        ("no matching implementation of function `%s' with %d argument(s)")
                        % fn->name % argc;
                    throw std::runtime_error(fmt.str());
                }

                acc = it->call(state, regs.data() + ins.data.call.first_reg);
                if (ins.kind == Instruction::Tail)
                    ret = true;
                break;
            }

        case Instruction::Jump:
            ip = ins.data.jmp_loc;
            break;

        case Instruction::Branch:
            if (acc.obj == run::Cell::false_object.obj)
                ip = ins.data.jmp_loc;
            break;

        default:
        case Instruction::Return:
            ret = true;
            break;
        }
    }
    return acc;
}

}
