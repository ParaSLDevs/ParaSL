#pragma once


#include "syntax_node.h"
#include "expressions.h"
#include "statements.h"
#include <cassert>

namespace parasl::ast{


    template<typename Derived>
    class ast_visitor{
    protected:

        void visit_impl(basic_syntax_nodes::SyntaxNode const* node){
            auto& derived = *static_cast<Derived*>(this);
            if(!node) {
                derived.Derived::operator()(nullptr);
                return;
            }

            switch (node->GetNodeType()) {
                case syntax_node_t::EXPR: {
                    auto* expr = dynamic_cast<const expressions::Expression*>(node);
                    assert(expr && "expected expression");
                    auto expr_type = expr->GetExprCategory();

                    switch (expr_type) {
                        case expr_type_t::OPERATOR: {
                            auto *op = dynamic_cast<const expressions::OperatorExpression *>(expr);
                            assert(op && "expected operator expression");
                            derived.Derived::operator()(op);
                            break;
                        }
                        case expr_type_t::SYMBOL: {
                            auto *op = dynamic_cast<const expressions::Identifier *>(expr);
                            assert(op && "expected identifier expression");
                            derived.Derived::operator()(op);
                            break;
                        }
                        case expr_type_t::LITERAL: {
                            auto *literal = dynamic_cast<const expressions::Literal *>(expr);
                            assert(literal && "expected literal expression");
                            derived.Derived::operator()(literal);
                            break;
                        }
                        case expr_type_t::REFERENCE: {
                            auto *reference = dynamic_cast<const expressions::Reference *>(expr);
                            assert(reference && "expected reference expression");
                            derived.Derived::operator()(reference);
                            break;
                        }
                        case expr_type_t::MEMBER_ACCESS: {
                            auto *memberAccess = dynamic_cast<const expressions::MemberAccess *>(expr);
                            assert(memberAccess && "expected member access expr");
                            derived.Derived::operator()(memberAccess);
                            break;
                        }
                        case expr_type_t::INPUT:{
                            auto *input = dynamic_cast<const expressions::InputExpr *>(expr);
                            assert(input && "expected input expression");
                            derived.Derived::operator()(input);
                            break;
                        }
                    }
                    break;
                }
                case syntax_node_t::STMT:{
                    auto* stmt = dynamic_cast<const statements::Statement*>(node);
                    assert(stmt && "expected statement");
                    auto stmt_type = stmt->GetStmtType();

                    switch (stmt_type) {
                        case stmt_type_t::IF_STMT:{
                            auto* if_stmt= dynamic_cast<const statements::IfStatement*>(stmt);
                            assert(if_stmt && "expected if statement");
                            derived.Derived::operator()(if_stmt);
                            break;
                        }
                        case stmt_type_t::FOR_STMT:{
                            auto* for_stmt = dynamic_cast<const statements::ForLoop*>(stmt);
                            assert(for_stmt && "expected for statement");
                            derived.Derived::operator()(for_stmt);
                            break;
                        }
                        case stmt_type_t::ASSIGNMENT:{
                            auto* assign_stmt = dynamic_cast<const statements::AssignmentStatement*>(stmt);
                            assert(assign_stmt && "expected assignment statement");
                            derived.Derived::operator()(assign_stmt);
                            break;
                        }
                        case stmt_type_t::COMPOUND_STMT:{
                            auto* compound_stmt = dynamic_cast<const statements::CompoundStatement*>(stmt);
                            assert(compound_stmt && "expected compound statement");
                            derived.Derived::operator()(compound_stmt);
                            break;
                        }
                        case stmt_type_t::DECL:{
                            auto* decl = dynamic_cast<const statements::DeclarationStatement*>(node);
                            assert(decl && "expected declaration statement");
                            derived.Derived::operator()(decl);
                            break;
                        }
                        case stmt_type_t::RET_STMT:{
                            auto* ret = dynamic_cast<const statements::RetStmt*>(node);
                            assert(ret && "expected return statement");
                            derived.Derived::operator()(ret);
                            break;
                        }
                        case stmt_type_t::OUTPUT_STMT:{
                            auto* out = dynamic_cast<const statements::OutputStmt*>(node);
                            assert(out && "expected output statement");
                            derived.Derived::operator()(out);
                            break;
                        }
                    }
                    break;
                }
            }
        }

    public:

        virtual ~ast_visitor() = default;

    };


    template<typename Derived>
    class depth_first_visitor{
    public:
        void visit(basic_syntax_nodes::SyntaxNode const* node){
            auto& derived = *static_cast<Derived*>(this);
            derived.Derived::PreAction(node);
            if(node)
                for(auto i = 0u; i < node->GetChildsNum(); ++i)
                    visit(node->GetChildAt(i));

            derived.Derived::PostAction(node);
        }
    };

}
