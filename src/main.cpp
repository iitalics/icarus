#include <iostream>
#include <utf8.h>
#include <boost/format.hpp>
#include "syntax/Input.h"
#include "syntax/Token.h"

int main (void)
{
    auto inp = InputSrc::ptr_from_input("Hello \xe2\x9e\xaf world!");

    Span span(inp);
    inp->span_here(span);

    lex::Token tok1('(', span);
    lex::Token tok2('[', span);

    std::cout << boost::format("expected %s, got %s\n") % tok1 % tok2;

    return 0;
}
