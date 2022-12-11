#pragma once

#include <iostream>

#include "syntax_node.h"
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

    class ForHeader: public Statement, public basic_syntax_nodes::ChildedSyntaxNode<2>{
    public:
        ForHeader(basic_syntax_nodes::Ref<statements::DeclarationStatement> inductive_var,
                  basic_syntax_nodes::Ref<expressions::RangeExpr> range):
                Statement(stmt_type_t::FOR_HEADER), basic_syntax_nodes::ChildedSyntaxNode<2>{
            std::move(inductive_var), std::move(range)
        }{

        }

        statements::DeclarationStatement const* inductiveVar() const{
            return dynamic_cast<statements::DeclarationStatement const*>(GetChildAt(0));
        }

        expressions::RangeExpr const* range() const{
            return dynamic_cast<expressions::RangeExpr const*>(GetChildAt(1));
        }
    };

    class ForLoop : public Statement, protected basic_syntax_nodes::ChildedSyntaxNode<2> {
    public:
        ForLoop(basic_syntax_nodes::Ref<ForHeader> header,
                basic_syntax_nodes::Ref<CompoundStatement> body) :
        Statement(stmt_type_t::FOR_STMT),
        ChildedSyntaxNode<2>(std::move(header), std::move(body)) {
        }

        const ForHeader *GetHeader() const {
            return dynamic_cast<const ForHeader *>(GetChildAt(0));
        }

        const CompoundStatement *GetBody() const {
            return dynamic_cast<const CompoundStatement *>(GetChildAt(3));
        }
    };

    class WhileLoop : public Statement, protected basic_syntax_nodes::ChildedSyntaxNode<2> {
    public:
        WhileLoop(basic_syntax_nodes::Ref<expressions::Expression> condition,
                basic_syntax_nodes::Ref<CompoundStatement> body) :
                Statement(stmt_type_t::WHILE_STMT),
                ChildedSyntaxNode<2>(std::move(condition), std::move(body)) {
        }

        const expressions::Expression *GetCondition() const {
            return dynamic_cast<const expressions::Expression *>(GetChildAt(0));
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
