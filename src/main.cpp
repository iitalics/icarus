#include <iostream>
#include <utf8.h>
#include <boost/format.hpp>
#include "syntax/Input.h"
#include "syntax/Lex.h"
#include "syntax/AST.h"
#include "syntax/parse.h"

int main (void)
{
    auto inp = InputSrc::ptr_from_input
        ("if x then a elseif y then b else c end",
         "<example>");

    try {

        lex::Lex lx(inp);
        auto expr = parse::parse_expr(lx);
        std::cout << "Parsed: " << expr << std::endl;
        lx.expect(lex::Token::EndOfFile);
    }
    catch (std::runtime_error& err) {
        std::cerr << "error:" << std::endl
                  << err.what() << std::endl;
    }

    return 0;
}
