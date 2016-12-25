#pragma once
#include "Input.h"
#include "../datatypes.h"

namespace lex {

struct Token
{
    enum Kind {
        // following tokens represented
        // as their ascii values: ( ) + - * / = , :

        /* special */
        EndOfFile = 0x10000,
        Int, String,
        Ident,
        Eq, // ==
        NotEq, // /=

        /* keywords */
        KW_end = 0x20000,
        KW_if,
        KW_then,
        KW_else,
        KW_elseif,
        KW_let,
        KW_fn,
        KW_datatype,
        KW_new,
        KW_is,
    };

    inline Token (Kind k, Span sp)
        : kind(k)
        , span(std::move(sp))
    {}
    inline Token (int k, Span sp)
        : kind(Kind(k))
        , span(std::move(sp))
    {}
    inline Token (Kind k, Span sp, Fixnum fx)
        : kind(k)
        , span(std::move(sp))
        , int_val(fx)
    {}
    inline Token (Kind k, Span sp, std::string str)
        : kind(k)
        , span(std::move(sp))
        , string_val(std::move(str))
    {}

    Kind kind;
    Span span;

    Fixnum int_val;
    std::string string_val;

    operator Kind () const { return kind; }

    static void write (std::ostream& os, Kind kind);
    void write (std::ostream& os) const;
};

template <typename OutStream>
OutStream& operator<< (OutStream& os, const Token& tok)
{
    tok.write(os);
    return os;
}

template <typename OutStream>
OutStream& operator<< (OutStream& os, const Token::Kind& tok)
{
    Token::write(os, tok);
    return os;
}


}
