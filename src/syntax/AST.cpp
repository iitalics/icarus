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
    os << "Field(";
    expr->write(os);
    os << ", " << key << ")";
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
