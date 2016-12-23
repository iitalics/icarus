#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <functional>
#include <fstream>

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

struct InputSrc
{
    template <typename InputStream>
    inline InputSrc (std::string fn, InputStream is)
        : filename(std::move(fn))
        , stream(new InputStream(std::move(is)))
        , eof(false)
        , pos_(0), bol_(0), line_(0)
    {
        take();
    }

    static InputSrcPtr ptr_from_file (std::string filename,
                                      bool soft_fail = false);
    static InputSrcPtr ptr_from_input (const std::string& input,
                                       std::string filename = "<input>");

    std::string filename;
    std::unique_ptr<std::istream> stream;

    bool eof;
    rune head;

    rune take ();
    std::string take_while (std::function<bool(rune)> pred,
                            Span& span_out);

    inline std::string take_while (std::function<bool(rune)> pred)
    {
        Span sp;
        return take_while(std::move(pred), sp);
    }

    void span_here (Span& span_out, size_t len = 1);

    std::string get_line (tpos bol);

private:
    void fill_buffer_ ();
    std::string buffer_;

    tpos pos_, bol_, line_;
};
