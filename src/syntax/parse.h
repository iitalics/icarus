#include "AST.h"
#include <vector>

namespace lex {
struct Lex;
}

namespace parse {

// definitions
ast::DefnPtr parse_defn (lex::Lex& lexer);
std::vector<ast::DefnPtr> parse_top (lex::Lex& lexer);
ast::FnName parse_fn_name (lex::Lex& lexer);

// statements
ast::StmtPtr parse_stmt (lex::Lex& lexer);
ast::BodyStmts parse_stmts (lex::Lex& lexer);

// expressions
ast::ExprPtr parse_expr (lex::Lex& lexer);
ast::ExprPtr parse_coterm (lex::Lex& lexer);
ast::ExprPtr parse_term (lex::Lex& lexer);
ast::ExprPtr parse_factor (lex::Lex& lexer);
ast::ExprPtr parse_factor_base (lex::Lex& lexer);
bool parse_factor_suffix (lex::Lex& lexer, ast::ExprPtr& e);
ast::BodyStmts parse_else (lex::Lex& lexer);

// etc.
ast::KeyName parse_key (lex::Lex& lexer);
std::vector<ast::VarName> parse_arg_names (lex::Lex& lexer);
std::vector<ast::ExprPtr> parse_args (lex::Lex& lexer);

}
