#include "Input.h"
#include <utf8.h>
#include <sstream>
#include <boost/format.hpp>
#include <iostream>


InputSrcPtr InputSrc::ptr_from_file (std::string filename,
                                     bool soft_fail)
{
    std::ifstream ifs;
    ifs.open(filename, decltype(ifs)::in);
    if (!ifs) {
        if (soft_fail)
            return nullptr;

        auto fmt = boost::format("could open file `%s'") % filename;
        throw std::runtime_error(fmt.str());
    }

    return std::make_shared<InputSrc>
        (std::move(filename), std::move(ifs));
}

InputSrcPtr InputSrc::ptr_from_input (const std::string& input,
                                      std::string filename)
{
    std::istringstream iss(input);
    return std::make_shared<InputSrc>
        (std::move(filename), std::move(iss));
}

rune InputSrc::take ()
{
    rune old = head;
    fill_buffer_();
    if (buffer_.empty()) {
        eof = true;
        head = '\0';
    }
    else {
        /* read the next rune out of buffer_ */
        auto buf_pos = buffer_.begin();
        head = utf8::next(buf_pos, buffer_.end());
        /* erase this rune from buffer_ */
        auto cpw = buf_pos - buffer_.begin();
        buffer_.erase(buffer_.begin(), buf_pos);
        /* advance pos_, line_ & col_ */
        pos_ += cpw; // pos_ is position directly following 'head'
        col_++;
        if (old == '\n') {
            line_++;
            col_ = 0;
            bol_ = pos_ - cpw;
        }
    }
    return old;
}

std::string InputSrc::take_while (std::function<bool(rune)> pred,
                                  Span& span_out)
{
    span_here(span_out, 0);

    std::string str;
    for (; !eof && pred(head); take()) {
        utf8::append(head, std::back_inserter(str));
        span_out.len++;
    }
    return str;
}

void InputSrc::fill_buffer_ ()
{
    int c;
    while (buffer_.size() <= 4) {
        c = stream->get();
        if (c == -1)
            break;
        else
            buffer_.push_back(c);
    }
}

void InputSrc::span_here (Span& span_out, size_t len)
{
    span_out.line = line_;
    span_out.col = col_;
    span_out.len = len;
    span_out.bol = bol_;
}

std::string InputSrc::get_line (tpos bol)
{
    auto old_pos = stream->tellg();
    std::string line;
    stream->seekg(bol, std::istream::beg);
    std::getline(*stream, line);
    stream->seekg(old_pos, std::istream::beg);
    return line;
}


std::runtime_error span_error (Span span, const std::string& msg)
{
    auto fmt = boost::format("%s:%d:%d: %s")
        % span.input->filename
        % (span.line + 1)
        % (span.col + 1)
        % msg;
    return std::runtime_error(fmt.str());
}
