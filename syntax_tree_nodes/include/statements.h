//
// Created by denist on 11/14/22.
//

#pragma once

#include "syntax_node.h"
#include "declarations.h"


class Statement : public SyntaxNode {
    stmt_type_t stmt_type;
};

class UnaryOperatorStmt : public Statement {
    operator_t op;
    Expression *expr;
};

class BinaryOperatorStmt : public Statement {
    operator_t op;
    Expression *left;
    Expression *right;
};

class AssignWithDeclStmt : public Statement {
    Declaration *left;
    Expression *right;
};

class DeclStmt : virtual public Declaration, Statement {
    /*
     * Don't know what to write here, it seems like all semantics described in parent classes
     */
};

class VarDeclStmt : public DeclStmt, VarDecl {
    /*
     * Don't know what to write here, it seems like all semantics described in parent classes
     */
};

class ArrayDeclStmt : public DeclStmt, ArrayDecl {
    /*
     * Don't know what to write here, it seems like all semantics described in parent classes
     */
};

class StructDeclStmt : public DeclStmt, StructDecl {
    /*
     * Don't know what to write here, it seems like all semantics described in parent classes
     */
};

class FuncDeclStmt : public  DeclStmt, FuncDecl {
    /*
     * Don't know what to write here, it seems like all semantics described in parent classes
     */
};

class IfStmt : public Statement {
    Expression *condition_;
    std::vector<Statement> body_;
    /*
     * idea:
     * We could use not just vector of Statements, but create another
     * class Scope, which can also contains, for example, SymbolTable
     */
};

class LoopStmt : public Statement {
    /*
     * Don't know what to write here, it seems like all semantics described in parent and child classes
     */
};

class WhileLoop : public LoopStmt {
    Expression *condition_;
    std::vector<Statement> body_;
    /*
     * idea:
     * We could use not just vector of Statements, but create another
     * class Scope, which can also contains, for example, SymbolTable
     */
};

class ForLoop : public LoopStmt {
    Expression *init_expr_;
    Expression *condition_;
    Expression *after_loop_expr_;
    std::vector<Statement> body_;
    /*
     * idea:
     * We could use not just vector of Statements, but create another
     * class Scope, which can also contains, for example, SymbolTable
     */
};

class RetStmt : public Statement {
    Expression *expr;
};

class OutputStmt : public Statement {
    Expression *expr;
};

