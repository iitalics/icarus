#include "Lex.h"

namespace lex {

Token Lex::take1 ()
{
    if (buffer_.empty())
        read_();

    auto tok = buffer_.front();
    buffer_.pop_front();
    return tok;
}

void Lex::take (size_t n)
{
    if (n > buffer_.size()) {
        buffer_.clear();
    }
    else {
        buffer_.erase(buffer_.begin(),
                      buffer_.begin() + n);
    }
}


void Lex::read_ ()
{
    Span span(src);
    src->span_here(span, 0);
    buffer_.push_back(Token(Token::EndOfFile, span));
}

Token Lex::read_ident_ ()
{
    throw std::runtime_error("invalid token");
}
Token Lex::read_number_ ()
{
    throw std::runtime_error("invalid token");
}
Token Lex::read_symbol_ ()
{
    throw std::runtime_error("invalid token");
}




}
