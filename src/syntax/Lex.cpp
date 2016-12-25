#include "Lex.h"
#include <unordered_map>
#include <boost/format.hpp>

namespace lex {

/* front-facing API code */

const Token& Lex::at (size_t i)
{
    while (buffer_.size() <= i)
        read_();
    return buffer_[i];
}

Token Lex::take1 ()
{
    if (buffer_.empty())
        read_();

    const auto tok = buffer_.front();
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




/* text parsing code */

void Lex::trim_ ()
{
    for (;;) {
        const auto trimmed = src->take_while(runes::is_whitespace);
        if (!src->eof && src->head == runes::comment_char)
            src->take_while([] (rune x) { return x != '\n'; });
        else
            break;
    }
}

void Lex::read_ ()
{
    trim_();
    if (src->eof) {
        buffer_.emplace_back(Token::EndOfFile, span_here());
        return;
    }

    const rune hd = src->head;
    std::string hd_str;

    if (runes::is_delimeter(hd)) {
        buffer_.emplace_back(hd, span_here());
        src->take();
    }
    else if (runes::is_numeric(hd)) {
        buffer_.push_back(read_number_());
    }
    else if (runes::is_symbolic(hd)) {
        buffer_.push_back(read_symbol_());
    }
    else {
        buffer_.push_back(read_ident_());
    }
}

namespace {
std::unordered_map<std::string, Token::Kind> wide_symbols =
    {
        {"==", Token::Eq},
        {"/=", Token::NotEq},
    };

std::unordered_map<std::string, Token::Kind> keywords =
    {
        {"end", Token::KW_end},
        {"if", Token::KW_if},
        {"then", Token::KW_then},
        {"else", Token::KW_else},
        {"elseif", Token::KW_elseif},
        {"let", Token::KW_let},
        {"fn", Token::KW_fn},
        {"datatype", Token::KW_datatype},
        {"new", Token::KW_new},
        {"is", Token::KW_is},
    };
}

Token Lex::read_symbol_ ()
{
    Span span(src);
    const auto sym = src->take_while(runes::is_symbolic, span);

    Token::Kind kind;
    if (sym.size() == 1) {
        kind = Token::Kind(sym[0]);
    }
    else if (wide_symbols.find(sym) != wide_symbols.cend()) {
        kind = wide_symbols[sym];
    }
    else {
        auto fmt = boost::format("invalid symbol `%s'") % sym;
        throw span_error(std::move(span), fmt.str());
    }

    return Token(kind, std::move(span));
}

Token Lex::read_ident_ ()
{
    Span span(src);
    const auto id = src->take_while(runes::is_identifier, span);

    Token::Kind kind;
    const auto kw_it = keywords.find(id);
    if (kw_it != keywords.cend()) {
        kind = kw_it->second;
    }
    else {
        kind = Token::Ident;
    }

    return Token(kind, std::move(span), std::move(id));
}

Token Lex::read_number_ ()
{
    Span span(src);
    const auto num_str = src->take_while(runes::is_identifier, span);

    Fixnum fx = 0;
    for (size_t i = 0; i < num_str.size(); i++) {
        const auto chr = num_str[i];
        if ('0' <= chr && chr <= '9') {
            fx = (fx * 10) + (chr - '0');
        }
        else {
            throw span_error(std::move(span), "invalid character in integer");
        }
    }

    return Token(Token::Int, span, fx);
}




}
