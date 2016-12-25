#include <iostream>
#include <utf8.h>
#include <boost/format.hpp>
#include "syntax/Input.h"
#include "syntax/Lex.h"

int main (void)
{
    auto inp = InputSrc::ptr_from_input("Hello \xe2\x9e\xaf world!");

    lex::Lex lexer(inp);
    std::cout << lexer.take1() << std::endl;

    return 0;
}
