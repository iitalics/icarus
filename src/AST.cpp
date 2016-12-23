#include "AST.h"

namespace ast {

Stmt::~Stmt () {}
void Stmt::traverse (std::function<void(Expr*)> f) const { (void) f; }

LetStmt::~LetStmt () {}
void LetStmt::traverse (std::function<void(Expr*)> f) const
{
    f(init.get());
    init->traverse(f);
}

SetVarStmt::~SetVarStmt () {}
void SetVarStmt::traverse (std::function<void(Expr*)> f) const
{
    f(to.get());
    to->traverse(f);
}

SetFieldStmt::~SetFieldStmt () {}
void SetFieldStmt::traverse (std::function<void(Expr*)> f) const
{
    f(expr.get());
    expr->traverse(f);
    f(to.get());
    to->traverse(f);
}

LoopStmt::~LoopStmt () {}
void LoopStmt::traverse (std::function<void(Expr*)> f) const
{
    for (auto& stmt : body)
        stmt->traverse(f);
}

BreakStmt::~BreakStmt () {}




Expr::~Expr () {}
void Expr::traverse (std::function<void(Expr*)> f) const { (void) f; }

IntExpr::~IntExpr () {}

StringExpr::~StringExpr () {}

VarExpr::~VarExpr () {}

AppExpr::~AppExpr () {}
void AppExpr::traverse (std::function<void(Expr*)> f) const
{
    for (auto& expr : args) {
        f(expr.get());
        expr->traverse(f);
    }
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

FieldExpr::~FieldExpr () {}
void FieldExpr::traverse (std::function<void(Expr*)> f) const
{
    f(expr.get());
    expr->traverse(f);
}

DataTypeExpr::~DataTypeExpr () {}

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


}
