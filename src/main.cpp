#include <iostream>
#include <utf8.h>
#include <boost/format.hpp>
#include "syntax/Input.h"
#include "syntax/Lex.h"

int main (void)
{
    auto inp = InputSrc::ptr_from_input
        ("rect% = datatype(width,height)\n"
         "fn perim (r is rect%)\n"
         "  2 * r.width + 2 * r.height\n"
         "end",
         "<example>");

    try {
        lex::Lex lexer(inp);
        for (int i = 0; ; i++) {
            const auto tok = lexer.take1();
            std::cout << boost::format("%2d) %10s  span: %s, line %d, col %d-%d\n")
                % (i + 1) % tok
                % tok.span.input->filename
                % (tok.span.line + 1)
                % (tok.span.col + 1)
                % (tok.span.col + tok.span.len);

            if (tok == lex::Token::EndOfFile)
                break;
        }
    }
    catch (std::runtime_error& err) {
        std::cerr << "error:" << std::endl
                  << err.what() << std::endl;
    }

    return 0;
}
