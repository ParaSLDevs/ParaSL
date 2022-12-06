#pragma once

#include <vector>
#include <string_view>
#include <map>
#include <optional>

namespace types{
    class Type;
}

#include "syntax_node.h"
#include "types.h"

namespace parasl::ast{

    class SemaError: public std::runtime_error{
    public:
        SemaError(std::string what): std::runtime_error(std::move(what)){
        }
    };


    template<typename Key, typename T>
    class SymbolTable{
    public:

        SymbolTable(){ pushScope(); }

        std::optional<T> getSymbol(Key const& key) const{
            std::optional<T> ret;
            std::find_if(m_table.rbegin(), m_table.rend(),
                                      [&key, &ret, this](auto& elem){
                auto found = elem.find(key);
                if(found != elem.end()){
                    ret.template emplace(found->second);
                    return true;
                }
                return false;
            });

            return ret;
        }
        bool registerSymbol(Key const&key, T&& symbol){
            if(m_table.back().contains(key))
                return false;
            m_table.back().template emplace(key, symbol);
            return true;
        }

        void pushScope() {
            m_table.template emplace_back();
        }
        void popScope() {
            m_table.pop_back();
        }

        void flush(){
            while(!m_table.empty())
                m_table.pop_back();

            pushScope();
        }

    private:

        std::vector<std::map<Key, T>> m_table;
    };

    class Context{
    public:

        Context();
        using TypeRef = basic_syntax_nodes::Ref<types::Type>;
        // built-in types
        types::Type const* getIntegralType(unsigned bitwidth);
        types::Type const* getCharType();
        types::Type const* getDoubleType();
        types::Type const* getFloatType();

        // compound types
        types::Type const* getArrayType(types::Type const* type, unsigned length);
        types::Type const* getVectorType(types::Type const* type, unsigned length);
        types::Type const* getStructType(std::vector<std::pair<std::string, types::Type const*>> const& fields);

        //function types
        types::Type const* getFunctionType(std::vector<std::pair<std::string_view, types::Type const*>> const& args, types::Type const* retType);

        // type cast
        types::Type const* typeOfOperatorExpression(types::Type const* lhs, types::Type const* rhs, operator_t op);

        void clear();


        virtual ~Context();
    private:
        std::map<unsigned, TypeRef> m_integral_types;
        TypeRef m_float_type, m_double_type, m_char_type;
        std::map<std::pair<types::Type const*, unsigned>, TypeRef> m_array_types;
        std::map<std::pair<types::Type const*, unsigned>, TypeRef> m_vector_types;
        std::multimap<unsigned long long, TypeRef> m_structure_types;
        std::multimap<std::pair<unsigned long long, types::Type const*>, TypeRef> m_function_types;

    protected:
        SymbolTable<std::string, basic_syntax_nodes::SyntaxNode*> m_symbol_table;
    };

    class Builder: public Context{
    public:
        using Node = basic_syntax_nodes::SyntaxNode*;

        Node createIntegralLiteral(unsigned int value);
        Node createUnaryOpExpr(Node expr, operator_t op);
        Node createBinaryOpExpr(Node lhs, Node rhs, operator_t op);
        Node createCompoundStatement(std::vector<Node>&& statements);
        Node createIfStatement(Node condition, Node then_clause, Node else_clause);
        Node createAssignStatement(Node assign);
        Node createReference(std::string_view name);
        Node createMemberAccess(Node expr, std::string_view member);
        Node createSubscriptAccess(Node expr, Node id_expr);
        Node createDeclaration(std::string_view id, types::Type const* type = nullptr, Node initializer = nullptr);

        void pushScope() {
            m_symbol_table.pushScope();
        }
        void popScope() {
            m_symbol_table.popScope();
        }


    };


}