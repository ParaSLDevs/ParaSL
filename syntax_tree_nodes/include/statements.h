//
// Created by denist on 11/14/22.
//

#pragma once

#include "syntax_node.h"
#include "declarations.h"


class Statement : virtual public SyntaxNode {
public:
    // TODO: Write getter
protected:
    Statement(stmt_type_t stmt_type) : SyntaxNode(syntax_node_t::STMT), stmt_type_(stmt_type) {}

    stmt_type_t stmt_type_;
};

class UnaryOperatorStmt : public Statement, UnaryOperatorExpr {
    /*
     * Don't know what to write here, it seems like all semantics described in parent classes
     */
};

class BinaryOperatorStmt : public Statement, BinaryOperatorExpr {
    /*
     * Don't know what to write here, it seems like all semantics described in parent classes
     */
};

class AssignWithDeclStmt : public Statement {
    std::unique_ptr<Declaration> left;
    std::unique_ptr<Expression> right;
};

class DeclStmt : virtual public Declaration, public Statement {
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
    std::unique_ptr<Expression> condition_;
    std::vector<std::unique_ptr<Statement>> body_;
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
    std::unique_ptr<Expression> condition_;
    std::vector<std::unique_ptr<Statement>> body_;
    /*
     * idea:
     * We could use not just vector of Statements, but create another
     * class Scope, which can also contains, for example, SymbolTable
     */
};

class ForLoop : public LoopStmt {
    std::unique_ptr<Expression> init_expr_;
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Expression> after_loop_expr_;
    std::vector<std::unique_ptr<Statement>> body_;
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

