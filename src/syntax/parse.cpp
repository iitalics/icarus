#include "parse.h"
#include "Lex.h"
#include <algorithm>

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
    case '<': case '>': case T::Eq: case T::NotEq:
        // op
        return std::string(1, lx.take1().kind);
    default:
        lx.expect("function name");
    }
}





StmtPtr parse_stmt (Lex& lx)
{
    // let <var> = <init>
    if (lx.at(0) == T::KW_let) {
        auto span = lx.take1().span;
        auto name = lx.eat(T::Ident).string_val;
        lx.eat(T::Kind('='));
        auto init_expr = parse_expr(lx);
        return StmtPtr(new LetStmt(span, std::move(name), std::move(init_expr)));
    }

    // loop ... end
    if (lx.at(0) == T::KW_loop) {
        auto span = lx.take1().span;
        auto body = parse_stmts(lx);
        lx.eat(T::KW_end);
        return StmtPtr(new LoopStmt(span, std::move(body)));
    }

    // break
    if (lx.at(0) == T::KW_break) {
        auto span = lx.take1().span;
        return StmtPtr(new BreakStmt(span));
    }

    // <expr>
    // <lhs> = <rhs>
    auto lhs = parse_expr(lx);
    if (lx.at(0) == '=') {
        auto span = lx.take1().span;
        auto rhs = parse_expr(lx);
        // attempt to make assignment
        if (auto stmt = lhs->make_assignment(span, std::move(rhs))) {
            return stmt;
        }
        else {
            throw span_error(lhs->span, "cannot assign to this kind of expression");
        }
    }
    else {
        auto span = lhs->span;
        return StmtPtr(new ValueStmt(span, std::move(lhs)));
    }
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



namespace {
template <size_t N, typename ParseFn>
ExprPtr parse_left_infix (Lex& lx, const int (&ops)[N], ParseFn parse_inner)
{
    auto accum = parse_inner(lx);
    std::vector<ExprPtr> args;
    const auto ops_end = ops + N;

    for (;;) {
        auto span = lx.at(0).span;
        auto op = lx.at(0).kind;
        if (std::find(ops, ops_end, op) != ops_end) {
            lx.take1();
            auto rhs = parse_inner(lx);
            args.reserve(2);
            args.clear();
            args.push_back(std::move(accum));
            args.push_back(std::move(rhs));
            accum = ExprPtr(new AppExpr(span, std::string(1, op), std::move(args)));
        }
        else
            break;
    }

    return std::move(accum);
}
}


ExprPtr parse_expr (Lex& lx)
{
    // if A then B else(??)
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

    return parse_left_infix(lx, (int[]) { '<', '>', T::Eq, T::NotEq },
                            parse_coterm);
}

ExprPtr parse_coterm (Lex& lx)
{
    return parse_left_infix(lx, (int[]) { '+', '-' }, parse_term);
}

ExprPtr parse_term (Lex& lx)
{
    return parse_left_infix(lx, (int[]) { '*', '/' }, parse_factor);
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
    // o.f(x,y) => f(o, x, y)
    if (lx.at(0) == '.' && lx.at(1) == T::Ident && lx.at(2) == '(') {
        lx.take1();
        auto span = lx.at(0).span;
        auto fn_name = lx.take1().string_val;
        auto args = parse_args(lx);
        args.insert(args.begin(), std::move(expr));
        expr.reset(new AppExpr(span, std::move(fn_name), std::move(args)));
        return true;
    }

    // o.x
    if (lx.at(0) == '.') {
        auto span0 = lx.at(0).span;
        auto span1 = lx.at(1).span;
        lx.take1();
        auto key = parse_key(lx);
        expr.reset(new FieldExpr(span0 + span1, std::move(expr), std::move(key)));
        return true;
    }

    return false;
}

BodyStmts parse_else (Lex& lx)
{
    switch (lx.at(0).kind) {
        // if A then B else C end
    case T::KW_else:
        {
            lx.take1();
            auto stmts = parse_stmts(lx);
            lx.eat(T::KW_end);
            return stmts;
        }

        // elseif A else B end
        // == eqv. to ==
        // else if A else B end end
    case T::KW_elseif:
        {
            auto span = lx.take1().span;
            auto cond = parse_expr(lx);
            lx.eat(T::KW_then);
            auto then_stmts = parse_stmts(lx);
            auto else_stmts = parse_else(lx);
            auto if_expr =
                ExprPtr(new IfExpr(span, std::move(cond),
                                   std::move(then_stmts), std::move(else_stmts)));
            auto val_stmt =
                StmtPtr(new ValueStmt(span, std::move(if_expr)));
            BodyStmts stmts;
            stmts.push_back(std::move(val_stmt));
            return stmts;
        }

        // if A then B end
        // == eqv. to ==
        // if A then B else end
    case T::KW_end:
        {
            lx.take1();
            return BodyStmts {};
        }

    default:
        lx.expect("`else', `elseif' or `end'");
    }
}





KeyName parse_key (Lex& lx)
{
    return lx.eat(T::Ident).string_val;
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
