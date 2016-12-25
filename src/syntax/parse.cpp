#include "parse.h"
#include "Lex.h"

namespace parse {

using namespace ast;
using namespace lex;
using T = Token;

std::vector<DefnPtr> parse_top (Lex& lx)
{
    std::vector<DefnPtr> defns;
    while (lx.at(0) != T::EndOfFile) {
        defns.push_back(parse_defn(lx));
    }
    return defns;
}


DefnPtr parse_defn (Lex& lx)
{
    auto span = lx.at(0).span;

    // function definition:
    // fn <name> (<args>) <body> end
    if (lx.at(0) == T::KW_fn) {
        lx.take1();
        auto fn_name = parse_fn_name(lx);
        auto arg_names = parse_arg_names(lx);
        auto stmts = parse_stmts(lx);
        lx.eat(T::KW_end);
        return DefnPtr(new FunctionDefn
            (std::move(span), std::move(fn_name),
             std::move(arg_names), std::move(stmts)));
    }

    // global definition:
    // <global> = <expr>
    if (lx.at(0) == T::Ident
             && lx.at(1) == '=') {
        auto glob_name = lx.take1().string_val;
        lx.take1();
        auto init_expr = parse_expr(lx);
        return DefnPtr(new GlobalDefn
            (std::move(span), std::move(glob_name),
             std::move(init_expr)));
    }

    throw span_error(span, "invalid toplevel definition");
}

FnName parse_fn_name (Lex& lx)
{
    switch (int(lx.at(0).kind)) {
    case T::Ident:
        // name =
        if (lx.at(1) == '=') {
            const auto name = lx.at(0).string_val + "=";
            lx.take(2);
            return name;
        }
        // name
        else {
            return lx.take1().string_val;
        }
    case '+': case '-': case '*': case '/':
        // op
        return std::string(1, lx.take1().kind);
    default:
        lx.expect("function name");
    }
}





StmtPtr parse_stmt (Lex& lx)
{
    // <expr>
    auto expr = parse_factor(lx);
    auto span = expr->span;
    auto val_stmt = new ValueStmt
        (std::move(span), std::move(expr));
    return StmtPtr(val_stmt);
}

BodyStmts parse_stmts (Lex& lx)
{
    std::vector<StmtPtr> stmts;
    for (;;) {
        const auto hd = lx.at(0).kind;
        if (hd == T::KW_end
            || hd == T::KW_else
            || hd == T::KW_elseif
            || hd == T::EndOfFile)
            break;
        stmts.push_back(parse_stmt(lx));
    }
    return stmts;
}




ExprPtr parse_expr (Lex& lx)
{
    if (lx.at(0) == T::KW_if) {
        auto span = lx.take1().span;
        auto cond = parse_expr(lx);
        lx.eat(T::KW_then);
        auto then_stmts = parse_stmts(lx);
        auto else_stmts = parse_else(lx);
        return ExprPtr(new IfExpr
                       (std::move(span), std::move(cond),
                        std::move(then_stmts), std::move(else_stmts)));
    }
    // TODO: infix operators
    return parse_term(lx);
}

ExprPtr parse_term (Lex& lx)
{
    // TODO: infix operators
    return parse_factor(lx);
}

ExprPtr parse_factor (Lex& lx)
{
    // factor ::= base (suffix*)
    auto base = parse_factor_base(lx);
    while (parse_factor_suffix(lx, base)) {
        ;
    }
    return base;
}

ExprPtr parse_factor_base (Lex& lx)
{
    ExprPtr expr;
    auto span = lx.at(0).span;

    switch (int(lx.at(0).kind)) {
        // <literals>
    case T::Int:
        return ExprPtr(new IntExpr(span, lx.take1().int_val));
    case T::String:
        return ExprPtr(new StringExpr(span, lx.take1().string_val));

        // ( <expr> )
    case '(':
        lx.take1();
        expr = parse_expr(lx);
        lx.eat(T::Kind(')'));
        return expr;

        // fn ( <args> )
        // <var>
    case T::Ident:
        if (lx.at(1) == '(') {
            auto fn_name = lx.take1().string_val;
            auto args = parse_args(lx);
            return ExprPtr(new AppExpr(span,
                                       std::move(fn_name),
                                       std::move(args)));
        }
        else {
            return ExprPtr(new VarExpr(span, lx.take1().string_val));
        }

    default:
        lx.expect("expression");
    }
}

bool parse_factor_suffix (Lex& lx, ExprPtr& expr)
{
    (void) lx; (void) expr;
    return false;
}

BodyStmts parse_else (Lex& lx)
{
    switch (lx.at(0).kind) {
    case T::KW_else:
        {
            lx.take1();
            auto stmts = parse_stmts(lx);
            lx.eat(T::KW_end);
            return stmts;
        }

    case T::KW_elseif:
        {
            auto span = lx.take1().span;
            auto cond = parse_expr(lx);
            lx.eat(T::KW_then);
            auto then_stmts = parse_stmts(lx);
            auto else_stmts = parse_else(lx);
            // elseif A else B end
            // eqv. to
            // else if A else B end end
            auto if_expr =
                ExprPtr(new IfExpr(span, std::move(cond),
                                   std::move(then_stmts), std::move(else_stmts)));
            auto val_stmt =
                StmtPtr(new ValueStmt(span, std::move(if_expr)));
            BodyStmts stmts;
            stmts.push_back(std::move(val_stmt));
            return stmts;
        }

    case T::KW_end:
        {
            lx.take1();
            return BodyStmts {};
        }

    default:
        lx.expect("`else', `elseif` or `end'");
    }
}





std::vector<VarName> parse_arg_names (Lex& lx)
{
    std::vector<VarName> names;
    lx.eat(T::Kind('('));
    if (lx.at(0) != ')') {
        names.push_back(lx.eat(T::Ident).string_val);
        while (lx.at(0) == ',') {
            lx.take1();
            names.push_back(lx.eat(T::Ident).string_val);
        }
    }
    lx.eat(T::Kind(')'));
    return names;
}

std::vector<ExprPtr> parse_args (Lex& lx)
{
    std::vector<ExprPtr> args;
    lx.eat(T::Kind('('));
    if (lx.at(0) != ')') {
        args.push_back(parse_expr(lx));
        while (lx.at(0) == ',') {
            lx.take1();
            args.push_back(parse_expr(lx));
        }
    }
    lx.eat(T::Kind(')'));
    return args;
}





}
