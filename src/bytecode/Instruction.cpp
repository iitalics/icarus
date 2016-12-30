#include "Instruction.h"

namespace bytecode {

Instruction Instruction::fxn (Fixnum fxn)
{
    return (Instruction)
        { .kind = Kind::Fxn, .data = { .fxn = fxn } };
}
Instruction Instruction::load (int src)
{
    return (Instruction)
        { .kind = Kind::Load, .data = { .src = src } };
}
Instruction Instruction::store (int dst)
{
    return (Instruction)
        { .kind = Kind::Store, .data = { .dst = dst } };
}
Instruction Instruction::call (run::Function* fn, int first_reg, size_t argc)
{
    return (Instruction)
        { .kind = Kind::Call, .data = { .call = { fn, first_reg, argc } } };
}
Instruction Instruction::tail_call (run::Function* fn, int first_reg, size_t argc)
{
    return (Instruction)
        { .kind = Kind::Tail, .data = { .call = { fn, first_reg, argc } } };
}
Instruction Instruction::jump (int loc)
{
    return (Instruction)
        { .kind = Kind::Jump, .data = { .jmp_loc = loc } };
}
Instruction Instruction::branch (int loc_alt)
{
    return (Instruction)
        { .kind = Kind::Branch, .data = { .jmp_loc = loc_alt } };
}
Instruction Instruction::ret ()
{
    return (Instruction)
        { .kind = Kind::Return };
}

}
