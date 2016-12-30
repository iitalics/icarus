#pragma once
#include "../runtime/Cell.h"

namespace run {
struct Function;
struct State;
}

namespace bytecode {

struct Instruction
{
    enum Kind {
        Fxn,    // fxn #<n>             [ tmp <- n ]
        Load,   // lod <rs>             [ tmp <- rs ]
        Store,  // sto <rd>             [ rd <- tmp ]
        Call,   // call F (rk..r{k+n})  [ tmp <- F(rk, .. r{k+n}) ]
        Tail,   // tcall F (rk..r{k+n}) [ tmp <- F(rk, .. r{k+n}); return tmp ]
        Return, // ret                  [ return tmp ]
        Jump,   // jmp Lk               [ goto Lk ]
        Branch, // br Lk                [ if ! tmp { goto Lk } ]
    };
    Kind kind;

    static Instruction fxn (Fixnum fxn);
    static Instruction load (int src);
    static Instruction store (int dst);
    static Instruction call (run::Function* fn, int first_reg, size_t argc);
    static Instruction tail_call (run::Function* fn, int first_reg, size_t argc);
    static Instruction jump (int loc);
    static Instruction branch (int loc_alt);
    static Instruction ret ();

    union {
        int src;
        int dst;
        Fixnum fxn;
        int jmp_loc;

        struct {
            run::Function* fn;
            int first_reg;
            size_t argc;
        } call;

    } data;
};

}
