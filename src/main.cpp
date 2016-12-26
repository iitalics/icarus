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
        ("loop"                "\n"
         "  if i > 10 then"    "\n"
         "    break"           "\n"
         "  end"               "\n"
         "  i = i + 1"         "\n"
         "  arr.at(i) = i * 5" "\n"
         "end"                 "\n"
         "",
         "<example>");

    try {

        lex::Lex lx(inp);
        auto stmt = parse::parse_stmt(lx);
        std::cout << "Parsed: " << stmt << std::endl;
        lx.expect(lex::Token::EndOfFile);
    }
    catch (std::runtime_error& err) {
        std::cerr << "error:" << std::endl
                  << err.what() << std::endl;
    }

    return 0;
}
