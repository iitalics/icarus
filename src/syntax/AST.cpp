#include "AST.h"
#include <iostream>
#include <sstream>

namespace ast {

Defn::~Defn () {}

GlobalDefn::~GlobalDefn () {}

FunctionDefn::~FunctionDefn () {}




Stmt::~Stmt () {}
void Stmt::write (std::ostream& os)
{
    os << "?";
}
void Stmt::traverse (std::function<void(Expr*)> f) const { (void) f; }

LetStmt::~LetStmt () {}
void LetStmt::write (std::ostream& os)
{
    os << "Let(" << var_name << ", ";
    init->write(os);
    os << ")";
}
void LetStmt::traverse (std::function<void(Expr*)> f) const
{
    f(init.get());
    init->traverse(f);
}

SetVarStmt::~SetVarStmt () {}
void SetVarStmt::write (std::ostream& os)
{
    os << "Set(" << var_name << ", ";
    to->write(os);
    os << ")";
}
void SetVarStmt::traverse (std::function<void(Expr*)> f) const
{
    f(to.get());
    to->traverse(f);
}

SetFieldStmt::~SetFieldStmt () {}
void SetFieldStmt::write (std::ostream& os)
{
    os << "Set(";
    expr->write(os);
    os << "." << key << ", ";
    to->write(os);
    os << ")";
}
void SetFieldStmt::traverse (std::function<void(Expr*)> f) const
{
    f(expr.get());
    expr->traverse(f);
    f(to.get());
    to->traverse(f);
}

LoopStmt::~LoopStmt () {}
void LoopStmt::write (std::ostream& os)
{
    os << "Loop[";
    for (size_t i = 0; i < body.size(); i++) {
        if (i > 0) os << "; ";
        body[i]->write(os);
    }
    os << "]";
}
void LoopStmt::traverse (std::function<void(Expr*)> f) const
{
    for (auto& stmt : body)
        stmt->traverse(f);
}

BreakStmt::~BreakStmt () {}
void BreakStmt::write (std::ostream& os)
{
    os << "Break()";
}

ValueStmt::~ValueStmt () {}
void ValueStmt::write (std::ostream& os)
{
    expr->write(os);
}
void ValueStmt::traverse (std::function<void(Expr*)> f) const
{
    f(expr.get());
    expr->traverse(f);
}




Expr::~Expr () {}
void Expr::traverse (std::function<void(Expr*)> f) const { (void) f; }
void Expr::write (std::ostream& os)
{
    os << "?";
}
StmtPtr Expr::make_assignment (Span span, ExprPtr rhs)
{
    (void) span; (void) rhs;
    return nullptr;
}


std::string Expr::to_str ()
{
    std::ostringstream oss;
    write(oss);
    return oss.str();
}

IntExpr::~IntExpr () {}
void IntExpr::write (std::ostream& os)
{
    os << val;
}

StringExpr::~StringExpr () {}
void StringExpr::write (std::ostream& os)
{
    os << "\"" << val << "\"";
}

VarExpr::~VarExpr () {}
void VarExpr::write (std::ostream& os)
{
    os << var_name;
}
StmtPtr VarExpr::make_assignment (Span sp, ExprPtr rhs)
{
    return StmtPtr(new SetVarStmt(span + sp, var_name, std::move(rhs)));
}

AppExpr::~AppExpr () {}
void AppExpr::traverse (std::function<void(Expr*)> f) const
{
    for (auto& expr : args) {
        f(expr.get());
        expr->traverse(f);
    }
}
void AppExpr::write (std::ostream& os)
{
    os << "App(" << fn_name;
    for (auto& expr : args) {
        os << ", ";
        expr->write(os);
    }
    os << ")";
}
StmtPtr AppExpr::make_assignment (Span sp, ExprPtr rhs)
{
    std::vector<ExprPtr> new_args;
    new_args.reserve(args.size() + 1);
    std::move(args.begin(), args.end(), std::back_inserter(new_args));
    new_args.push_back(std::move(rhs));
    auto new_app = ExprPtr(new AppExpr(span + sp, fn_name + "=",
                                       std::move(new_args)));
    return StmtPtr(new ValueStmt(span + sp, std::move(new_app)));
}


IfExpr::~IfExpr () {}
void IfExpr::traverse (std::function<void(Expr*)> f) const
{
    f(cond.get());
    cond->traverse(f);
    for (auto& stmt : then_body)
        stmt->traverse(f);
    for (auto& stmt : else_body)
        stmt->traverse(f);
}
void IfExpr::write (std::ostream& os)
{
    os << "If(";
    cond->write(os);
    os << ", [";
    for (size_t i = 0; i < then_body.size(); i++) {
        if (i > 0) os << "; ";
        then_body[i]->write(os);
    }
    os << "], [";
    for (size_t i = 0; i < else_body.size(); i++) {
        if (i > 0) os << "; ";
        else_body[i]->write(os);
    }
    os << "])";
}

FieldExpr::~FieldExpr () {}
void FieldExpr::traverse (std::function<void(Expr*)> f) const
{
    f(expr.get());
    expr->traverse(f);
}
void FieldExpr::write (std::ostream& os)
{
    expr->write(os);
    os << "." << key;
}
StmtPtr FieldExpr::make_assignment (Span sp, ExprPtr rhs)
{
    return StmtPtr(new SetFieldStmt(sp, std::move(expr), std::move(key), std::move(rhs)));
}

DataTypeExpr::~DataTypeExpr () {}
void DataTypeExpr::write (std::ostream& os)
{
    os << "DataType()";
}

NewExpr::~NewExpr () {}
void NewExpr::traverse (std::function<void(Expr*)> f) const
{
    f(type.get());
    type->traverse(f);
    for (auto& expr : args) {
        f(expr.get());
        expr->traverse(f);
    }
}
void NewExpr::write (std::ostream& os)
{
    os << "New(";
    type->write(os);
    for (auto& expr : args) {
        os << ", ";
        expr->write(os);
    }
    os << ")";
}

}
