//
// Created by denist on 11/14/22.
//

#pragma once

#include <memory>

#include "syntax_node.h"

class Expression : public SyntaxNode {
    expr_type_t expt_type_;
};

class UnaryOperatorExpr : public Expression {
    operator_t op_type_;
    std::unique_ptr<Expression> opnd_;
    bool is_postfix_;
};

class BinaryOperatorExpr : public Expression {
    operator_t op_type_;
    std::unique_ptr<Expression> left_opnd_;
    std::unique_ptr<Expression> right_opnd_;
};

class Literal : public Expression {
    std::string literal_value_;
    prim_type_t literal_type_;
};

class Symbol : public Expression {
    std::string name_;
};

class InputExpr : public Expression {
    size_t num_;
};
