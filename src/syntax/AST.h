#pragma once
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "../datatypes.h"
#include "Span.h"

namespace ast
{

/*
Defn ::=
  Global(x, expr)
  Function(fn_name, args, body)

Stmt ::=
  Let(x, expr)
  SetVar(x, expr)
  SetField(expr, key, expr)
  Loop(body)
  Break()
  Value(e)

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
struct Expr;
struct Defn;
using StmtPtr = std::unique_ptr<Stmt>;
using ExprPtr = std::unique_ptr<Expr>;
using DefnPtr = std::unique_ptr<Defn>;

using VarName = std::string;
using KeyName = std::string;
using FnName = std::string;

using BodyStmts = std::vector<StmtPtr>;



/*
 * Definitions
 */
struct Defn
{
    inline Defn (Span sp)
        : span(std::move(sp))
    {}
    virtual ~Defn () = 0;
    Span span;
};

/* <global> = <expr> */
struct GlobalDefn : public Defn
{
    inline GlobalDefn (Span span, VarName n, ExprPtr e)
        : Defn(std::move(span))
        , name(std::move(n))
        , init_expr(std::move(e))
    {}
    virtual ~GlobalDefn ();
    VarName name;
    ExprPtr init_expr;
};

/* fn f (x, ..) ... end */
struct FunctionDefn : public Defn
{
    inline FunctionDefn (Span span, FnName fn,
                         std::vector<VarName> args,
                         BodyStmts b)
        : Defn(std::move(span))
        , name(std::move(fn))
        , arg_names(std::move(args))
        , body(std::move(b))
    {}
    virtual ~FunctionDefn ();
    FnName name;
    std::vector<VarName> arg_names;
    BodyStmts body;
};



/*
 * Statements
 */
struct Stmt
{
    Stmt (Span sp)
        : span(std::move(sp))
    {}
    virtual ~Stmt () = 0;
    virtual void write (std::ostream& os);
    virtual void traverse (std::function<void(Expr*)> f) const;
    Span span;
};

/* let <var> = <value> */
struct LetStmt : public Stmt
{
    inline LetStmt (Span span, VarName name, ExprPtr e)
        : Stmt(std::move(span))
        , var_name(std::move(name))
        , init(std::move(e))
    {}
    virtual ~LetStmt ();
    virtual void write (std::ostream& os);
    virtual void traverse (std::function<void(Expr*)> f) const;
    VarName var_name;
    ExprPtr init;
};

/* <var> = <value> */
struct SetVarStmt : public Stmt
{
    inline SetVarStmt (Span span, VarName name, ExprPtr e)
        : Stmt(std::move(span))
        , var_name(std::move(name))
        , to(std::move(e))
    {}
    virtual ~SetVarStmt ();
    virtual void write (std::ostream& os);
    virtual void traverse (std::function<void(Expr*)> f) const;

    VarName var_name;
    ExprPtr to;
};

/* <expr>.<key> = <value> */
struct SetFieldStmt : public Stmt
{
    inline SetFieldStmt (Span span, ExprPtr e1, KeyName k, ExprPtr e2)
        : Stmt(std::move(span))
        , expr(std::move(e1))
        , to(std::move(e2))
        , key(std::move(k))
    {}
    virtual ~SetFieldStmt ();
    virtual void write (std::ostream& os);
    virtual void traverse (std::function<void(Expr*)> f) const;
    ExprPtr expr, to;
    KeyName key;
};

/* loop ... end */
struct LoopStmt : public Stmt
{
    inline LoopStmt (Span span, BodyStmts b)
        : Stmt(std::move(span))
        , body(std::move(b))
    {}
    virtual ~LoopStmt ();
    virtual void write (std::ostream& os);
    virtual void traverse (std::function<void(Expr*)> f) const;
    BodyStmts body;
};

/* break */
struct BreakStmt : public Stmt
{
    inline BreakStmt (Span span)
        : Stmt(std::move(span))
    {}
    virtual void write (std::ostream& os);
    virtual ~BreakStmt ();
};

/* <expr> */
struct ValueStmt : public Stmt
{
    inline ValueStmt (Span span, ExprPtr e)
        : Stmt(std::move(span))
        , expr(std::move(e))
    {}
    virtual ~ValueStmt ();
    virtual void write (std::ostream& os);
    virtual void traverse (std::function<void(Expr*)> f) const;
    ExprPtr expr;
};



/*
 * Expressions
 */
struct Expr
{
    inline Expr (Span sp)
        : span(std::move(sp))
    {}
    virtual ~Expr () = 0;

    // utilities
    virtual void write (std::ostream& os);
    virtual void traverse (std::function<void(Expr*)> f) const;

    // for parser:
    virtual StmtPtr make_assignment (Span span, ExprPtr rhs);

    Span span;

    std::string to_str ();
};

/* 1234 */
struct IntExpr : public Expr
{
    inline IntExpr (Span span, Fixnum fx)
        : Expr(std::move(span))
        , val(fx)
    {}
    virtual ~IntExpr ();
    virtual void write (std::ostream& os);
    Fixnum val;
};

/* "abcdef" */
struct StringExpr : public Expr
{
    inline StringExpr (Span span, std::string s)
        : Expr(std::move(span))
        , val(s)
    {}
    virtual ~StringExpr ();
    virtual void write (std::ostream& os);
    std::string val;
};

/* <var> */
struct VarExpr : public Expr
{
    inline VarExpr (Span span, VarName n)
        : Expr(std::move(span))
        , var_name(std::move(n))
    {}
    virtual ~VarExpr ();
    virtual void write (std::ostream& os);
    virtual StmtPtr make_assignment (Span span, ExprPtr rhs);
    VarName var_name;
};

/* <fn>(<expr>, ..) */
struct AppExpr : public Expr
{
    inline AppExpr (Span span, FnName fn, std::vector<ExprPtr> es)
        : Expr(std::move(span))
        , fn_name(std::move(fn))
        , args(std::move(es))
    {}
    virtual ~AppExpr ();
    virtual void write (std::ostream& os);
    virtual void traverse (std::function<void(Expr*)> f) const;
    virtual StmtPtr make_assignment (Span span, ExprPtr rhs);
    FnName fn_name;
    std::vector<ExprPtr> args;
};

/* if <expr> then ... else ... end */
struct IfExpr : public Expr
{
    inline IfExpr (Span span, ExprPtr e,
                   BodyStmts body1,
                   BodyStmts body2)
        : Expr(std::move(span))
        , cond(std::move(e))
        , then_body(std::move(body1))
        , else_body(std::move(body2))
    {}
    virtual ~IfExpr ();
    virtual void write (std::ostream& os);
    virtual void traverse (std::function<void(Expr*)> f) const;
    ExprPtr cond;
    BodyStmts then_body;
    BodyStmts else_body;
};

/* <expr>.<key> */
struct FieldExpr : public Expr
{
    inline FieldExpr (Span span, ExprPtr e, KeyName k)
        : Expr(std::move(span))
        , expr(std::move(e))
        , key(std::move(k))
    {}
    virtual ~FieldExpr ();
    virtual void write (std::ostream& os);
    virtual void traverse (std::function<void(Expr*)> f) const;
    virtual StmtPtr make_assignment (Span span, ExprPtr rhs);
    ExprPtr expr;
    KeyName key;
};

/* datatype(...) */
struct DataTypeExpr : public Expr
{
    inline DataTypeExpr (Span span, std::vector<KeyName> ks)
        : Expr(std::move(span))
        , keys(std::move(ks))
    {}
    virtual ~DataTypeExpr ();
    virtual void write (std::ostream& os);
    std::vector<KeyName> keys;
};

/* new <expr>(<expr>, ..) */
struct NewExpr : public Expr
{
    inline NewExpr (Span span, ExprPtr t, std::vector<ExprPtr> es)
        : Expr(std::move(span))
        , type(std::move(t))
        , args(std::move(es))
    {}
    virtual ~NewExpr ();
    virtual void write (std::ostream& os);
    virtual void traverse (std::function<void(Expr*)> f) const;
    ExprPtr type;
    std::vector<ExprPtr> args;
};



template <typename OutStream>
OutStream& operator<< (OutStream& os, const ExprPtr& expr)
{
    expr->write(os);
    return os;
}

template <typename OutStream>
OutStream& operator<< (OutStream& os, const StmtPtr& stmt)
{
    stmt->write(os);
    return os;
}



}
