#pragma once

#include <memory>

#include "syntax_node.h"

namespace expressions {
    class Expression : virtual public basic_syntax_nodes::SyntaxNode {
    public:
        expr_type_t GetExprCategory() const {
            return expr_category_type_;
        }

        const types::Type *GetType() const {
            return expr_type_;
        }

        virtual ~Expression() = default;

    protected:
        Expression(expr_type_t expr_category_type, const types::Type *expr_type) :
        expr_category_type_(expr_category_type), expr_type_(expr_type) {
            syntax_node_type_ = syntax_node_t::EXPR;
        }

        expr_type_t expr_category_type_;
        const types::Type *expr_type_;
    };

    class UnaryOperatorExpr : public Expression, 
                              public basic_syntax_nodes::ChildSyntaxNode<1> {
    public:
        operator_t GetOperatorType() const {
            return op_type_;
        }

        bool IsPostfix() const {
            return is_postfix_;
        }
        /*
        * idea: restrict available operator_t values
        */

        UnaryOperatorExpr(std::unique_ptr<Expression> opnd, bool is_postfix, operator_t op_type) :
        Expression(expr_type_t::OPERATOR, opnd->GetType()),
        ChildSyntaxNode<1>(std::move(opnd)), op_type_(op_type), is_postfix_(is_postfix) {}

    protected:

        operator_t op_type_;
        bool is_postfix_;
    };

    class BinaryOperatorExpr : public Expression, basic_syntax_nodes::ChildSyntaxNode<2> {
    public:

        /*
        * idea: restrict available operator_t values
        */
        BinaryOperatorExpr(std::unique_ptr<Expression> left,
                        std::unique_ptr<Expression> right, operator_t op_type) :
                Expression(expr_type_t::OPERATOR, left->GetType()),
                basic_syntax_nodes::ChildSyntaxNode<2>(std::move(left), std::move(right)), 
                op_type_(op_type) {}

    protected:
        operator_t op_type_;
    };

    class Literal final : public Expression, basic_syntax_nodes::ChildSyntaxNode<0> {
    public:
        Literal(const std::string &value, const types::Type *type) : Expression(expr_type_t::LITERAL, type),
        ChildSyntaxNode<0>(), literal_value_(value) {}

        /*
        * idea: somehow return value with actual type of literal
        */
        std::string GetLiteralValue() const {
            return literal_value_;
        }

    private:
        std::string literal_value_;
    };

    class Symbol final : public Expression, basic_syntax_nodes::ChildSyntaxNode<0> {
    public:
        Symbol(const std::string &name, const types::Type *type) : Expression(expr_type_t::SYMBOL, type),
        ChildSyntaxNode<0>(), name_(name) {}

        std::string GetSymbolName() const {
            return name_;
        }

    private:
        std::string name_;
    };

    class InputExpr : public Expression, basic_syntax_nodes::ChildSyntaxNode<0> {
    public:
        InputExpr(size_t num, const types::Type *type) : Expression(expr_type_t::INPUT, type),
        ChildSyntaxNode<0>(), num_(num) {}

        [[nodiscard]] size_t GetInputNum () const {
            return num_;
        }

    private:
        size_t num_;
    };
}
