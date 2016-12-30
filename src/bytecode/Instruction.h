#pragma once
#include "../runtime/Cell.h"

namespace run {
struct Function;
}

namespace bytecode {

struct Instruction
{
    enum Kind {
        MovReg, // mov <rd>, <rs>       [ rd <- rs ]
        MovFxn,  // mov <rd>, #<fixnum> [ rd <- (N) ]
        Load,   // lod <rs>             [ X0 <- rs ]
        Store,  // sto <rd>             [ rd <- X0 ]
        Call,   // call F (rk..r{k+n})  [ X0 <- F(rk, .. r{k+n}) ]
        Tail,   // tcall F (rk..r{k+n}) [ X0 <- F(rk, .. r{k+n}); return X0 ]
        Return, // ret                  [ return X0 ]
        Jump,   // jmp Lk               [ goto Lk ]
        Branch, // br Lk                [ if ! X0 { goto Lk } ]
    };
    Kind kind;

    Instruction mov (int dst, int src);
    Instruction mov_fxn (int dst, Fixnum fxn);
    Instruction load ();
    Instruction store ();
    Instruction call (run::Function* fn, int first_reg, int argc);
    Instruction tail_call (run::Function* fn, int first_reg, int argc);
    Instruction jump (int loc);
    Instruction branch (int loc_alt);

    union {
        struct {
            int src, dst;
            Fixnum fxn;
        } move;

        struct {
            run::Function* fn;
            int first_reg;
            int argc;
        } call;

        struct {
            int loc;
        } jmp;

    } data;

};

}
