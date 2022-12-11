#pragma once

#include <memory>
#include <variant>

#include "syntax_node.h"

namespace expressions {

    class Expression : public basic_syntax_nodes::TypedSyntaxNode {
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
        TypedSyntaxNode(syntax_node_t::EXPR), expr_category_type_(expr_category_type), expr_type_(expr_type) {}

        expr_type_t expr_category_type_;
        const types::Type *expr_type_;
    };

    class OperatorExpression: public Expression{
    public:

        operator_t GetOperatorType() const {
            return op_type_;
        }

    protected:
        OperatorExpression(operator_t optype, const types::Type * type):
        Expression(expr_type_t::OPERATOR, type), op_type_(optype){}

    private:
        operator_t op_type_;
    };

    class UnaryOperatorExpr : public OperatorExpression,
                              public basic_syntax_nodes::ChildedSyntaxNode<1> {
    public:


        bool IsPostfix() const {
            return is_postfix_;
        }
        /*
        * idea: restrict available operator_t values
        */

        UnaryOperatorExpr(basic_syntax_nodes::Ref<Expression> opnd, const types::Type * type, bool is_postfix, operator_t op_type) :
                OperatorExpression(op_type, type),
                ChildedSyntaxNode<1>(std::move(opnd)), is_postfix_(is_postfix) {}

    protected:

        bool is_postfix_;
    };

    class BinaryOperatorExpr : public OperatorExpression, public basic_syntax_nodes::ChildedSyntaxNode<2> {
    public:

        /*
        * idea: restrict available operator_t values
        */
        BinaryOperatorExpr(basic_syntax_nodes::Ref<Expression> left,
                           basic_syntax_nodes::Ref<Expression> right, const types::Type * type, operator_t op_type) :
                OperatorExpression(op_type, type),
                basic_syntax_nodes::ChildedSyntaxNode<2>(std::move(left), std::move(right)) {}

    };

    class MemberAccess : public Expression, public basic_syntax_nodes::ChildedSyntaxNode<1>{
    public:

        MemberAccess(basic_syntax_nodes::Ref<Expression> expr, std::string_view member):
                Expression(expr_type_t::MEMBER_ACCESS, inferType(*expr, member)),
                basic_syntax_nodes::ChildedSyntaxNode<1>(std::move(expr)), m_member(member){

        };


        std::string_view member() const{
            return m_member;
        }
    private:

        types::Type const* inferType(Expression const& expr, std::string_view member){
            auto* expr_type = expr.GetType();
            if(!expr_type)
                return nullptr;

            auto* casted_type = dynamic_cast<types::StructType const*>(expr_type);
            if(!casted_type)
                throw std::runtime_error("Expression type expected to be a structure");

            auto foundMember = std::find_if(casted_type->begin(), casted_type->end(), [member](auto& elem){
                return elem.first == member;});

            if(foundMember == casted_type->end())
                throw std::runtime_error("Expected member not found in type");

            return foundMember->second;
        }
        std::string m_member;
    };


    class Literal final : public Expression, basic_syntax_nodes::LeafNode {
    public:
        template<typename T>
        Literal(T &&value, const types::Type *type) :
            Expression(expr_type_t::LITERAL, type),
            ChildedSyntaxNode<0>(), literal_value_(std::forward<T>(value)) {

            }

        template<typename T>
        T GetLiteralValue() const {
            return std::get<T>(literal_value_);
        }

    private:
        std::variant<unsigned int, int, std::string> literal_value_;
    };

    class Identifier final : public Expression, basic_syntax_nodes::LeafNode {
    public:
        Identifier(std::string_view name, const types::Type *type) : Expression(expr_type_t::SYMBOL, type),
                                                                       ChildedSyntaxNode<0>(), name_(name) {}

        std::string GetSymbolName() const {
            return name_;
        }

    private:
        std::string name_;
    };

    class Reference: public Expression, basic_syntax_nodes::LeafNode{
    public:
        Reference(Identifier const* identifier):
                Expression(expr_type_t::REFERENCE, identifier->GetType()), m_identifier{identifier}{};

        Identifier const* identifier() const{
            return m_identifier;
        }
    private:
        Identifier const* m_identifier;
    };

    class InputExpr : public Expression, basic_syntax_nodes::LeafNode {
    public:
        InputExpr(size_t num, const types::Type *type) : Expression(expr_type_t::INPUT, type),
                                                         ChildedSyntaxNode<0>(), num_(num) {}

        [[nodiscard]] size_t GetInputNum () const {
            return num_;
        }

    private:
        size_t num_;
    };

    class InitializationList: public Expression, public basic_syntax_nodes::ChildedSyntaxNode<>{
    public:
        InitializationList(types::Type const* type, auto begin, auto end):
                Expression(expr_type_t::INIT_LIST, type),
                basic_syntax_nodes::ChildedSyntaxNode<>{begin, end}{};
    };

    class RepeatExpr: public Expression, public basic_syntax_nodes::ChildedSyntaxNode<1>{
    public:
        RepeatExpr(types::Type const* type, basic_syntax_nodes::Ref<Expression> expr, unsigned times):
                Expression(expr_type_t::REPEAT, type),
                basic_syntax_nodes::ChildedSyntaxNode<1>{std::move(expr)}, m_times(times){};

        unsigned times() const{
            return m_times;
        }
    private:
        unsigned m_times;
    };

    class GlueExpr: public Expression, public basic_syntax_nodes::ChildedSyntaxNode<>{
    public:
        GlueExpr(types::Type const* type, auto begin, auto end):
                Expression(expr_type_t::GLUE, type),
                basic_syntax_nodes::ChildedSyntaxNode<>{begin, end}{};
    };

    class BindExpr: public Expression, public basic_syntax_nodes::ChildedSyntaxNode<2>{
    public:
        BindExpr(types::Type const* type, basic_syntax_nodes::Ref<Expression> func, basic_syntax_nodes::Ref<Expression> member):
                Expression(expr_type_t::BIND, type),
                basic_syntax_nodes::ChildedSyntaxNode<2>{std::move(func), std::move(member)}{};
    };
}
