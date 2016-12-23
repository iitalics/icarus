#include <iostream>
#include <utf8.h>
#include <boost/format.hpp>
#include "syntax/Input.h"

int main (void)
{
    auto inp = InputSrc::ptr_from_input("Hello \xe2\x9e\xaf world!");

    while (!inp->eof) {
        auto word = inp->take_while([] (rune rn) { return rn != ' '; });
        std::cout << "word: `" << word << "'" << std::endl;

        inp->take_while([] (rune rn) { return rn == ' '; });
    }
    std::cout << "done" << std::endl;
    return 0;
}
