#pragma once
#include <deque>
#include "Input.h"
#include "Token.h"

namespace lex {

struct Lex
{
    inline Lex (InputSrcPtr isrc)
        : src(std::move(isrc))
    {}

    InputSrcPtr src;

    const Token& at (size_t i) const { return buffer_[i]; }
    Token take1 ();
    void take (size_t n);

private:
    std::deque<Token> buffer_;

    void read_ ();
    Token read_ident_ ();
    Token read_number_ ();
    Token read_symbol_ ();
};


}
