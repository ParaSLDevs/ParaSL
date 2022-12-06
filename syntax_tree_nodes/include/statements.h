#pragma once

#include <iostream>

#include "syntax_node.h"
#include "declarations.h"
#include "expressions.h"

namespace statements {
    class Statement : public basic_syntax_nodes::TypedSyntaxNode {
    public:
        stmt_type_t GetStmtType() const {
            return stmt_type_;
        }

    protected:
        Statement(stmt_type_t stmt_type) : TypedSyntaxNode(syntax_node_t::STMT), stmt_type_(stmt_type) {}

        stmt_type_t stmt_type_;
    };
#if 0
    class UnaryOperatorStmt : public Statement, public basic_syntax_nodes::ChildedSyntaxNode<1> {
    public:
        UnaryOperatorStmt(basic_syntax_nodes::Ref<expressions::UnaryOperatorExpr> expr) :
                Statement(stmt_type_t::UNARY_OP),
                basic_syntax_nodes::ChildedSyntaxNode<1>(std::move(expr))
                        {
                        }
    };

    class BinaryOperatorStmt : public Statement, public basic_syntax_nodes::ChildedSyntaxNode<1> {
    public:
        BinaryOperatorStmt(basic_syntax_nodes::Ref<expressions::BinaryOperatorExpr> expr) :
        Statement(stmt_type_t::BINARY_OP),
        basic_syntax_nodes::ChildedSyntaxNode<1>(std::move(expr))
        {
        }
    };

    class DeclStmt : public Statement, public declarations::Declaration {
    public:
        DeclStmt(decl_type_t decl_type, const std::string &name, const types::Type *type) :
        Statement(stmt_type_t::DECL), Declaration(decl_type, name, type) {}    
    };

    class FuncDeclStmt : public  Statement, public declarations::FuncDecl {
    public:
    template <class RandIt,
                    class = std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, 
                                            typename std::iterator_traits<RandIt>::iterator_category>::value>>
        FuncDeclStmt(const std::string &func_name, const types::FuncType *func_type, RandIt begin, RandIt end) : 
        Statement(stmt_type_t::DECL), FuncDecl(func_name, func_type, begin, end) {}

        template <class ...Elt>
        FuncDeclStmt(const std::string &func_name, const types::FuncType *func_type, Elt... elt) : 
        Statement(stmt_type_t::DECL), FuncDecl(func_name, func_type, elt...) {}
    };
#endif

    class AssignmentStatement: public Statement, public basic_syntax_nodes::ChildedSyntaxNode<1>{
    public:
        AssignmentStatement(basic_syntax_nodes::Ref<expressions::Expression> assignment):
                Statement(stmt_type_t::ASSIGNMENT),
                basic_syntax_nodes::ChildedSyntaxNode<1>{std::move(assignment)}{
        }
    };

    class DeclarationStatement: public Statement, public basic_syntax_nodes::ChildedSyntaxNode<2>{
    public:
        DeclarationStatement(basic_syntax_nodes::Ref<expressions::Identifier> id,
                             basic_syntax_nodes::Ref<expressions::Expression> initializer):
                Statement(stmt_type_t::DECL),
                basic_syntax_nodes::ChildedSyntaxNode<2>{std::move(id),
                                                         std::move(initializer)}{

        }
        expressions::Expression const* initializer() const{
            return dynamic_cast<expressions::Expression const*>(GetChildAt(1));
        }

        expressions::Identifier const* identifier() const{
            return dynamic_cast<expressions::Identifier const*>(GetChildAt(0));
        }
    };
    class CompoundStatement: public Statement, public basic_syntax_nodes::ChildedSyntaxNode<>{
    public:
        CompoundStatement(auto begin, auto end):
                Statement(stmt_type_t::COMPOUND_STMT),
                ChildedSyntaxNode<>(begin, end){

        }
    };

    class IfStatement: public Statement, public basic_syntax_nodes::ChildedSyntaxNode<3>{
    public:
        IfStatement(basic_syntax_nodes::Ref<expressions::Expression> cond,
                    basic_syntax_nodes::Ref<CompoundStatement> then_clause,
                    basic_syntax_nodes::Ref<CompoundStatement> else_clause):
                Statement(stmt_type_t::IF_STMT), basic_syntax_nodes::ChildedSyntaxNode<3>{std::move(cond),
                                                                                          std::move(then_clause),
                                                                                          std::move(else_clause)}{

        }

        expressions::Expression const* condition() const{
            return dynamic_cast<const expressions::Expression *>(GetChildAt(0));
        }

        CompoundStatement const* then_clause() const{
            return dynamic_cast<CompoundStatement const*>(GetChildAt(1));
        }

        CompoundStatement const* else_clause() const{
            return dynamic_cast<CompoundStatement const*>(GetChildAt(2));
        }
    };

    class ForLoop : public Statement, protected basic_syntax_nodes::ChildedSyntaxNode<4> {
    public:
        ForLoop(basic_syntax_nodes::Ref<Statement> init,
                basic_syntax_nodes::Ref<expressions::Expression> cond,
                basic_syntax_nodes::Ref<expressions::Expression> after,
                basic_syntax_nodes::Ref<CompoundStatement> body) :
        Statement(stmt_type_t::FOR_STMT),
        ChildedSyntaxNode<4>(std::move(init), std::move(cond), std::move(after), std::move(body)) {
        }

        const Statement *GetInit() const {
            return dynamic_cast<const Statement *>(GetChildAt(0));
        }

        const expressions::Expression *GetCond() const {
            return dynamic_cast<const expressions::Expression *>(GetChildAt(1));
        }

        const expressions::Expression *GetAfterExpr() const {
            return dynamic_cast<const expressions::Expression *>(GetChildAt(2));
        }

        const CompoundStatement *GetBody() const {
            return dynamic_cast<const CompoundStatement *>(GetChildAt(3));
        }
    };

    class RetStmt : public Statement, basic_syntax_nodes::ChildedSyntaxNode<1> {
    public:
        RetStmt(std::unique_ptr<expressions::Expression> opnd) :
                Statement(stmt_type_t::RET_STMT),
                ChildedSyntaxNode<1>(std::move(opnd)) {}

    };

    class OutputStmt : public Statement, basic_syntax_nodes::ChildedSyntaxNode<1> {
    public:
        OutputStmt(std::unique_ptr<expressions::Expression> opnd) :
                Statement(stmt_type_t::OUTPUT_STMT),
                ChildedSyntaxNode<1>(std::move(opnd)) {}

    };
}
