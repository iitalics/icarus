#include "Instruction.h"

namespace bytecode {

Instruction Instruction::mov (int dst, int src)
{
    return (Instruction)
        { .kind = Kind::MovReg, .data = { .move = { src, dst, 0 } } };
}
Instruction Instruction::mov_fxn (int dst, Fixnum fxn)
{
    return (Instruction)
        { .kind = Kind::MovFxn, .data = { .move = { 0, dst, fxn } } };
}
Instruction Instruction::load ()
{
    return (Instruction) { .kind = Kind::Load };
}
Instruction Instruction::store ()
{
    return (Instruction) { .kind = Kind::Store };
}
Instruction Instruction::call (run::Function* fn, int first_reg, int argc)
{
    return (Instruction)
        { .kind = Kind::Call, .data = { .call = { fn, first_reg, argc } } };
}
Instruction Instruction::tail_call (run::Function* fn, int first_reg, int argc)
{
    return (Instruction)
        { .kind = Kind::Tail, .data = { .call = { fn, first_reg, argc } } };
}
Instruction Instruction::jump (int loc)
{
    return (Instruction)
        { .kind = Kind::Jump, .data = { .jmp = { loc } } };
}
Instruction Instruction::branch (int loc_alt)
{
    return (Instruction)
        { .kind = Kind::Branch, .data = { .jmp = { loc_alt } } };
}

}
