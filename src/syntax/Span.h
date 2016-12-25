#pragma once
#include <memory>
#include <cstdint>

struct InputSrc;
using InputSrcPtr = std::shared_ptr<InputSrc>;

/* text position */
using tpos = size_t;
/* unicode rune */
using rune = uint32_t;

struct Span
{
    explicit Span (InputSrcPtr in = nullptr)
        : input(in)
        , pos(0), len(0), line(0)
    {}

    InputSrcPtr input;

    // note:
    //   `pos' and `bol' are byte position in stream
    //   `len' is in >characters<, not bytes
    //   `line' is line #, starting with 0 for first line
    tpos pos, len, line, bol;

    inline bool invalid () const
    { return !input || len == 0; }

    Span operator+ (const Span& union_with) const;
};
