#include "ast_builder.h"
#include <numeric>
#include <cassert>
#include <sstream>
#include "expressions.h"
#include "statements.h"

namespace parasl::ast{

    Context::Context():
            m_float_type(std::make_unique<types::VarType>(prim_type_t::FLOAT, 0)),
            m_double_type(std::make_unique<types::VarType>(prim_type_t::DOUBLE, 0)),
            m_char_type(std::make_unique<types::VarType>(prim_type_t::CHAR, 0)){

    }

    types::Type const* Context::getIntegralType(unsigned bitwidth){
        if(m_integral_types.contains(bitwidth))
            return m_integral_types.at(bitwidth).get();

        return m_integral_types.emplace(bitwidth,
                                        std::make_unique<types::VarType>(prim_type_t::INT, bitwidth)).first->second.get();
    }

    types::Type const* Context::getDoubleType(){
        return m_double_type.get();
    }

    types::Type const* Context::getFloatType(){
        return m_float_type.get();
    }


    types::Type const* Context::getArrayType(types::Type const* type, unsigned length){
        auto key = std::make_pair(type, length);
        if(m_array_types.contains(key))
            return m_array_types.at(key).get();
        return m_array_types.emplace(std::make_pair(type, length),
                                     std::make_unique<types::ArrayType>(type, length)).first->second.get();
    }

    types::Type const* Context::getVectorType(types::Type const* type, unsigned length){
        auto key = std::make_pair(type, length);
        if(m_vector_types.contains(key))
            return m_vector_types.at(key).get();
        return m_vector_types.emplace(std::make_pair(type, length),
                                     std::make_unique<types::VectorType>(type, length)).first->second.get();
    }

    namespace {

        unsigned long long hash(std::vector<std::pair<std::string, types::Type const*>> const& args){
            unsigned long long acc = 0;
            return std::accumulate(args.begin(), args.end(), acc, [](unsigned long long a,
                    std::pair<std::string_view, types::Type const*> const& arg){
                auto arg_hash_f = std::hash<std::string_view>{}(arg.first);
                auto arg_hash_s = std::hash<types::Type const*>{}(arg.second);
                auto arg_hash = std::hash<decltype(arg_hash_f)>{}(arg_hash_f + arg_hash_s);
                return std::hash<decltype(arg_hash + a)>{}(a + arg_hash);
            });
        }
    }
    types::Type const* Context::getStructType(std::vector<std::pair<std::string, types::Type const*>> const& fields){
        auto h = hash(fields);
        if(m_structure_types.contains(h)){
            auto eq_range = m_structure_types.equal_range(h);
            auto tmp = types::StructType(fields.begin(), fields.end());
            auto found = std::find_if(eq_range.first, eq_range.second, [&tmp](auto const& type){
                return *static_cast<types::StructType*>(type.second.get()) == tmp;
            });
            if(found != eq_range.second)
                return found->second.get();

        }
        return m_structure_types.emplace(h, std::make_unique<types::StructType>(fields.begin(), fields.end()))->second.get();
    }

    Context::~Context(){}

    types::Type const *Context::getCharType() {
        return m_char_type.get();
    }

    types::Type const* Context::typeOfOperatorExpression(types::Type const* lhs, types::Type const* rhs, operator_t op){

        // If any of types is unspecified - type of all expression is unspecified
        if(!lhs || !rhs)
            return nullptr;

        switch (op) {
            // TODO: proper type casting
            case operator_t::ASSIGN:
            case operator_t::MULT:
            case operator_t::DIV:
            case operator_t::PLUS:
            case operator_t::MINUS:{
                if(lhs != rhs)
                    return nullptr;
                return lhs;
            }
#if 0
            // subscript access lhs[rhs]
            // In structures we require rhs be constant expr
            case operator_t::SQUARE_BR:
            {

                break;
            }
#endif
            default:
                return lhs;
        }

        assert(0 && "Unhandled case");
        return nullptr;
    }
    Builder::Node Builder::createIntegralLiteral(unsigned int value){
        auto literal = std::make_unique<expressions::Literal>(value, getIntegralType(32));
        return literal.release();
    }

    Builder::Node Builder::createUnaryOpExpr(Builder::Node expr, operator_t op){

        auto* casted_expr = dynamic_cast<expressions::Expression *>(expr);

        assert((bool)expr == (bool)casted_expr && "Broken subexpression");

        auto unaryExpr = std::make_unique<expressions::UnaryOperatorExpr>(
                std::unique_ptr<expressions::Expression>(casted_expr),
                casted_expr ? casted_expr->GetType() : nullptr, // TODO: implement type checking
                false, op);
        return unaryExpr.release();
    }

    Builder::Node Builder::createBinaryOpExpr(Builder::Node lhs, Builder::Node rhs, operator_t op){

        auto* casted_lhs = dynamic_cast<expressions::Expression *>(lhs);
        auto* casted_rhs = dynamic_cast<expressions::Expression *>(rhs);

        assert((bool)lhs == (bool)casted_lhs && "Broken lhs node");
        assert((bool)rhs == (bool)casted_rhs && "Broken rhs node");

        auto* type = typeOfOperatorExpression(casted_lhs->GetType(), casted_rhs->GetType(), op);

        if(!type)
            throw SemaError("Operand type mismatch in expression");

        auto binaryExpr = std::make_unique<expressions::BinaryOperatorExpr>(
                std::unique_ptr<expressions::Expression>(casted_lhs),
                std::unique_ptr<expressions::Expression>(casted_rhs),
                type,
                op);
        return binaryExpr.release();
    }

    Builder::Node Builder::createCompoundStatement(std::vector<Builder::Node>&& statements){
        // TODO: check if we really got statements

        // TODO: get rid of this useless conversion
        std::vector<std::unique_ptr<basic_syntax_nodes::SyntaxNode>> conv_statements;
        std::transform(statements.begin(), statements.end(), std::back_inserter(conv_statements), [](auto& stmt){
            return std::unique_ptr<basic_syntax_nodes::SyntaxNode>(stmt);
        });
        return std::make_unique<statements::CompoundStatement>(conv_statements.begin(), conv_statements.end()).release();
    }

    Builder::Node Builder::createIfStatement(Builder::Node condition, Builder::Node then_clause, Builder::Node else_clause){
        auto conv_cond = basic_syntax_nodes::Ref<expressions::Expression>(
                dynamic_cast<expressions::Expression*>(condition)
                );
        auto conv_then_clause = basic_syntax_nodes::Ref<statements::CompoundStatement>(
                dynamic_cast<statements::CompoundStatement*>(then_clause)
        );

        auto conv_else_clause = basic_syntax_nodes::Ref<statements::CompoundStatement>(
                dynamic_cast<statements::CompoundStatement*>(else_clause)
        );

        return std::make_unique<statements::IfStatement>(std::move(conv_cond),
                                                         std::move(conv_then_clause),
                                                         std::move(conv_else_clause)).release();
    }

    Builder::Node Builder::createReference(std::string_view name) {
        auto expected_symbol = m_symbol_table.getSymbol(name.data());


        if(!expected_symbol) {
            std::stringstream ss;
            ss << "Symbol \"" << name << "\" has not been declared in this scope";
            throw SemaError(ss.str());
        }

        auto* decl = dynamic_cast<statements::DeclarationStatement*>(expected_symbol.value());

        assert(decl && "Expected declaration statement");

        return std::make_unique<expressions::Reference>(decl->identifier()).release();
    }

    Builder::Node Builder::createMemberAccess(Builder::Node expr, std::string_view member) {

        auto* casted_expr = dynamic_cast<expressions::Expression *>(expr);

        assert(casted_expr && "Expected expression");

        auto* expr_type = dynamic_cast<types::StructType const*>(casted_expr->GetType());

        if(!expr_type) {
            std::stringstream ss;
            ss << "Type \"";
            expr_type->dump(ss);
            ss << "\" is not sctruct -> it has no member \"" << member << "\"";
            throw SemaError("Cannot access member on non-struct type");
        }

        return std::make_unique<expressions::MemberAccess>(std::unique_ptr<expressions::Expression> {casted_expr},
                                                           member).release();
    }

    Builder::Node Builder::createSubscriptAccess(Node expr, Node id_expr) {

        // TODO: support structure member access via [] (Constant folding required)

        auto* casted_expr = dynamic_cast<expressions::Expression *>(expr);
        assert(casted_expr && "Expected expression");

        auto* casted_id_expr = dynamic_cast<expressions::Expression *>(id_expr);
        assert(casted_id_expr && "Expected expression");

        auto* expr_type = casted_expr->GetType();
        auto* id_type = casted_id_expr->GetType();

        bool wrong_type = false;
        if(auto* prim_type = dynamic_cast<types::VarType const*>(id_type)){
            if(prim_type->primType() != prim_type_t::INT)
                wrong_type = true;
        } else
            wrong_type = true;

        if(wrong_type)
            throw SemaError("Expected integral type inside '[ ]' operator");

        auto expr_cat = expr_type->GetEntityType();
        if(expr_cat != entity_type_t::VECTOR && expr_cat != entity_type_t::ARRAY)
            throw SemaError("Invalid type left from '[ ]' operator");

        auto subType = expr_cat == entity_type_t::VECTOR ? dynamic_cast<types::VectorType const*>(expr_type)->GetEltType() :
                       dynamic_cast<types::ArrayType const*>(expr_type)->GetEltType();


        return std::make_unique<expressions::BinaryOperatorExpr>(
                std::unique_ptr<expressions::Expression>(casted_expr),
                std::unique_ptr<expressions::Expression>(casted_id_expr),
                subType, operator_t::SQUARE_BR
                ).release();
    }

    Builder::Node Builder::createDeclaration(std::string_view id, types::Type const* type, Node initializer) {
        expressions::Expression * rhs = nullptr;
        if(initializer){
            rhs = dynamic_cast<expressions::Expression *>(initializer);
            assert(rhs && "Expected expression");
            if(type){
                // TODO: do not restrict type to exact match
                if(rhs->GetType() != type)
                    throw SemaError("Initializer type mismatch with initializer");
            } else{
                type = rhs->GetType();
            }
        } else{
            // default type
            if(!type)
                type = getIntegralType(32);
        }

        if(!m_symbol_table.getSymbol(id.data())){

            if(!rhs){
                // TODO: create default initializer
            }

            auto decl = std::make_unique<statements::DeclarationStatement>(
                                                    std::make_unique<expressions::Identifier>(id, type),
                                                    std::unique_ptr<expressions::Expression>(rhs));
            m_symbol_table.registerSymbol(id.data(), decl.get());
            return decl.release();
        } else{
            // Symbol already registered -> it is plain assignment

            auto ret = createReference(id);

            // Check for redeclaration
            if(type != dynamic_cast<expressions::Reference*>(ret)->GetType()) {
                std::stringstream ss;
                ss << "Redeclaration of \"" << id << "\" with different type";
                throw SemaError(ss.str());
            }


            if(initializer)
                ret = createBinaryOpExpr(ret, initializer, operator_t::ASSIGN);
            return ret;
        }
    }

    Builder::Node Builder::createAssignStatement(Builder::Node assign) {
        auto* casted = dynamic_cast<expressions::Expression*>(assign);
        assert(casted && "expected expression here");

        return std::make_unique<statements::AssignmentStatement>(std::unique_ptr<expressions::Expression>(casted)).release();
    }
}