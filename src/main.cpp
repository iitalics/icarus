#include <iostream>
#include <utf8.h>
#include <boost/format.hpp>
#include "syntax/Input.h"

int main (void)
{
    auto inp = InputSrc::ptr_from_input("Hello \xe2\x9e\xaf world!");

    std::string chr;
    for (; !inp->eof; inp->take()) {
        chr.clear();
        utf8::append(inp->head, std::back_inserter(chr));
        std::cout << boost::format("-> %s (charpoint = %x)\n") % chr % inp->head;
    }

    return 0;
}
