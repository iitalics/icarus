#pragma once
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "../datatypes.h"

namespace ast
{

/*
Stmt ::=
  Let(x, expr)
  SetVar(x, expr)
  SetField(expr, key, expr)
  Loop(body)
  Break()

Expr ::=
  IntLiteral(n)
  StringLiteral(s)
  Var(x)
  App(fn_name, args)
  If(expr, body1, body2)
  Field(e, key)
  DataType(keys)
  New(type_expr, args)
*/

struct Stmt;
using StmtPtr = std::unique_ptr<Stmt>;
struct Expr;
using ExprPtr = std::unique_ptr<Expr>;

using VarName = std::string;
using KeyName = std::string;
using FnName = std::string;

/*
 * Statements
 */
struct Stmt
{
    virtual ~Stmt () = 0;
    virtual void traverse (std::function<void(Expr*)> f) const;
};

/* let <var> = <value> */
struct LetStmt : public Stmt
{
    inline LetStmt (VarName name, ExprPtr e)
        : var_name(std::move(name))
        , init(std::move(e))
    {}
    virtual ~LetStmt ();
    virtual void traverse (std::function<void(Expr*)> f) const;
    VarName var_name;
    ExprPtr init;
};

/* <var> = <value> */
struct SetVarStmt : public Stmt
{
    inline SetVarStmt (VarName name, ExprPtr e)
        : var_name(std::move(name))
        , to(std::move(e))
    {}
    virtual ~SetVarStmt ();
    virtual void traverse (std::function<void(Expr*)> f) const;

    VarName var_name;
    ExprPtr to;
};

/* <expr>.<key> = <value> */
struct SetFieldStmt : public Stmt
{
    inline SetFieldStmt (ExprPtr e1, KeyName k, ExprPtr e2)
        : expr(std::move(e1))
        , to(std::move(e2))
        , key(std::move(k))
    {}
    virtual ~SetFieldStmt ();
    virtual void traverse (std::function<void(Expr*)> f) const;
    ExprPtr expr, to;
    KeyName key;
};

/* loop ... end */
struct LoopStmt : public Stmt
{
    inline LoopStmt (std::vector<StmtPtr> s)
        : body(std::move(s))
    {}
    virtual ~LoopStmt ();
    virtual void traverse (std::function<void(Expr*)> f) const;
    std::vector<StmtPtr> body;
};

/* break */
struct BreakStmt : public Stmt
{
    inline BreakStmt () {}
    virtual ~BreakStmt ();
};


/*
 * Expressions
 */
struct Expr
{
    virtual ~Expr () = 0;
    virtual void traverse (std::function<void(Expr*)> f) const;
};

/* 1234 */
struct IntExpr : public Expr
{
    inline IntExpr (Fixnum fx)
        : val(fx)
    {}
    virtual ~IntExpr ();
    Fixnum val;
};

/* "abcdef" */
struct StringExpr : public Expr
{
    inline StringExpr (std::string s)
        : val(s)
    {}
    virtual ~StringExpr ();
    std::string val;
};

/* <var> */
struct VarExpr : public Expr
{
    inline VarExpr (VarName n)
        : var_name(std::move(n))
    {}
    virtual ~VarExpr ();
    VarName var_name;
};

/* <fn>(<expr>, ..) */
struct AppExpr : public Expr
{
    inline AppExpr (FnName fn, std::vector<ExprPtr> es)
        : fn_name(std::move(fn))
        , args(std::move(es))
    {}
    virtual ~AppExpr ();
    virtual void traverse (std::function<void(Expr*)> f) const;
    FnName fn_name;
    std::vector<ExprPtr> args;
};

/* if <expr> then ... else ... end */
struct IfExpr : public Expr
{
    inline IfExpr (ExprPtr e,
                   std::vector<StmtPtr> body1,
                   std::vector<StmtPtr> body2)
        : cond(std::move(e))
        , then_body(std::move(body1))
        , else_body(std::move(body2))
    {}
    virtual ~IfExpr ();
    virtual void traverse (std::function<void(Expr*)> f) const;
    ExprPtr cond;
    std::vector<StmtPtr> then_body;
    std::vector<StmtPtr> else_body;
};

/* <expr>.<key> */
struct FieldExpr : public Expr
{
    inline FieldExpr (ExprPtr e, KeyName k)
        : expr(std::move(e))
        , key(std::move(k))
    {}
    virtual ~FieldExpr ();
    virtual void traverse (std::function<void(Expr*)> f) const;
    ExprPtr expr;
    KeyName key;
};

/* datatype(...) */
struct DataTypeExpr : public Expr
{
    inline DataTypeExpr (std::vector<KeyName> ks)
        : keys(std::move(ks))
    {}
    virtual ~DataTypeExpr ();
    std::vector<KeyName> keys;
};

/* new <expr>(<expr>, ..) */
struct NewExpr : public Expr
{
    inline NewExpr (ExprPtr t, std::vector<ExprPtr> es)
        : type(std::move(t))
        , args(std::move(es))
    {}
    virtual ~NewExpr ();
    virtual void traverse (std::function<void(Expr*)> f) const;
    ExprPtr type;
    std::vector<ExprPtr> args;
};


}
