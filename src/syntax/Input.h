#pragma once
#include "Span.h"
#include <string>
#include <functional>
#include <fstream>

struct InputSrc
{
    template <typename InputStream>
    inline InputSrc (std::string fn, InputStream is)
        : filename(std::move(fn))
        , stream(new InputStream(std::move(is)))
        , eof(false)
        , head('\0')
        , pos_(0)
        , line_(0)
        , bol_(0)
    {
        take();
        col_ = 0;
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

    std::istream::pos_type pos_;
    tpos line_, col_, bol_;
};
