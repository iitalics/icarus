#include "Token.h"

namespace lex {

void Token::write (std::ostream& os) const
{
    switch (kind) {
    case Ident:
        os << '"' << string_val << '"';
        break;
    default:
        write(os, kind);
    }
}

void Token::write (std::ostream& os, Kind kind)
{
    switch (kind) {
    case EndOfFile: os << "<eof>"; break;
    case Ident:  os << "<ident>"; break;
    case Int:    os << "<integer>"; break;
    case String: os << "<string>"; break;
    case Eq:     os << "`=='"; break;
    case NotEq:  os << "`/='"; break;

    case KW_end:      os << "`end'"; break;
    case KW_if:       os << "`if'"; break;
    case KW_then:     os << "`then'"; break;
    case KW_else:     os << "`else'"; break;
    case KW_elseif:   os << "`elseif'"; break;
    case KW_let:      os << "`let'"; break;
    case KW_fn:       os << "`fn'"; break;
    case KW_datatype: os << "`datatype'"; break;
    case KW_new:      os << "`new'"; break;
    case KW_is:       os << "`is'"; break;

    default:
        os << "`" << char(kind) << "'";
        break;
    }
}

}
