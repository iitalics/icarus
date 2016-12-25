#pragma once
#include <deque>
#include "Input.h"
#include "Token.h"

namespace lex {

namespace runes {

extern char char_class[128];
inline bool is_whitespace (rune rn)
{ return rn < 128 && (char_class[rn] & 1); }
inline bool is_numeric (rune rn)
{ return rn < 128 && (char_class[rn] & 2); }
inline bool is_delimeter (rune rn)
{ return rn < 128 && (char_class[rn] & 4); }

}

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
