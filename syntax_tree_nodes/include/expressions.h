//
// Created by denist on 11/14/22.
//

#pragma once

#include <memory>

#include "syntax_node.h"

class Expression : virtual public SyntaxNode {
public:
    expr_type_t GetExprCategory() const {
        return expr_category_type_;
    }

    const Type *GetType() const {
        return expr_type_;
    }

    virtual ~Expression() = default;

protected:
    Expression(expr_type_t expr_category_type, const Type *expr_type) :
    expr_category_type_(expr_category_type), expr_type_(expr_type) {
        syntax_node_type_ = syntax_node_t::EXPR;
    }

    expr_type_t expr_category_type_;
    const Type *expr_type_;
};

class UnaryOperatorExpr final : public Expression, public ChildSyntaxNode<1> {
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

class BinaryOperatorExpr final : public Expression, ChildSyntaxNode<2> {
public:

    /*
     * idea: restrict available operator_t values
     */
    BinaryOperatorExpr(std::unique_ptr<Expression> left,
                       std::unique_ptr<Expression> right, operator_t op_type) :
            Expression(expr_type_t::OPERATOR, left->GetType()),
            ChildSyntaxNode<2>(std::move(left), std::move(right)), op_type_(op_type) {}

protected:
    operator_t op_type_;
};

class Literal final : public Expression {
public:
    Literal(const std::string& value, prim_type_t type) : Expression(expr_type_t::LITERAL, ),
protected:
    std::string literal_value_;
    prim_type_t literal_name_;
};

class Symbol : public Expression {
    std::string name_;
};

class InputExpr : public Expression {
    size_t num_;
};
