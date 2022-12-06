#ifndef PARASL_COMMON_H
#define PARASL_COMMON_H

#include <map>
#include <memory>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

#include "error_handler.h"
#include "ast_builder.h"

namespace parasl {

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

using qi::char_;
using qi::string;
using qi::uint_;
using qi::int_;
using qi::_val;
using qi::raw;
using qi::lexeme;
using qi::lit;
using qi::alpha;
using qi::alnum;

using qi::_1;
using qi::_2;
using qi::_3;
using qi::_4;

using qi::on_error;
using qi::fail;

using phx::function;

using StrIter = std::string::iterator;

enum class UnaryOp{
    PLUS,
    MINUS,
    INV
};

enum class MultOp{
    MULT,
    DIV
};

enum class AddOp{
    ADD,
    SUB
};

enum class RelOp{
    LT,   // less than
    LE,   // less or equal
    GT,   // greater than
    GE    // greater or equal
};

enum class EqOp{
    EQ,  // equal
    NE   // not equal
};

inline operator_t translate(MultOp op){
    return op == MultOp::MULT ? operator_t::MULT : operator_t::DIV;
}

inline operator_t translate(AddOp op){
    return op == AddOp::ADD ? operator_t::PLUS : operator_t::MINUS;
}

inline operator_t translate(RelOp op){
    switch(op){
        case RelOp::LT: return operator_t::LT;
        case RelOp::LE: return operator_t::LE;
        case RelOp::GT: return operator_t::GT;
        case RelOp::GE: return operator_t::GE;
    }
    return operator_t::LT;
}
inline operator_t translate(UnaryOp op) {
    switch(op){
        case UnaryOp::PLUS: return operator_t::PLUS;
        case UnaryOp::MINUS: return operator_t::MINUS;
        case UnaryOp::INV: return operator_t::NOT;
    }
    return  operator_t::MINUS;
}
inline operator_t translate(EqOp op){
    return op == EqOp::EQ ? operator_t::EQ : operator_t::NE;
}

using node_t = basic_syntax_nodes::SyntaxNode*;
using type_t = types::Type const*;

enum class Keywords{
    LAYER,
    INPUT,
    OUTPUT,
    REPEAT,
    GLUE,
    BIND,
    IF,
    ELSE,
    FOR,
    IN,
    RETURN,
    CHAR,
    INT,
    FLOAT,
    DOUBLE
};
struct keywords_list : qi::symbols<char, Keywords> {
    keywords_list() {
        add
                ("layer", Keywords::LAYER)
                ("input", Keywords::INPUT)
                ("output", Keywords::OUTPUT)
                ("repeat", Keywords::REPEAT)
                ("glue", Keywords::GLUE)
                ("bind", Keywords::BIND)
                ("if", Keywords::IF)
                ("else", Keywords::ELSE)
                ("for", Keywords::FOR)
                ("in", Keywords::IN)
                ("return", Keywords::RETURN)
                ("char", Keywords::CHAR)
                ("int", Keywords::INT)
                ;
    }
};

enum class BuiltInType{
    INT,
    CHAR,
    FLOAT,
    DOUBLE
};
struct prim_types_list : qi::symbols<char, BuiltInType> {
    prim_types_list() {
        add
                ("int", BuiltInType::INT)
                ("char", BuiltInType::CHAR)
                ("float", BuiltInType::FLOAT)
                ("double", BuiltInType::DOUBLE)
                ;
    }
};

    namespace ASTBuilder{

        extern ast::Builder* builderCtx;

        template<typename Operation>
        using OperationSequence = boost::fusion::vector<node_t, std::vector<boost::fusion::vector<Operation, node_t>>>;

        // Handy base class for common exception handling / debugging
        template<typename Derived>
        struct ActionBase{
            template<typename Input, typename Context>
            void operator()(Input& input, Context &ctx, bool& pass) const {
                try {
                    static_cast<Derived const *>(this)->Derived::impl(input, ctx, pass);
                } catch (ast::SemaError& e){
                    std::cerr << "Semantic error: "<< e.what() << std::endl;
                    pass = false;
                }
            }

        };

        struct IntegralLiteral: public ActionBase<IntegralLiteral>{
            template<typename Context>
            void impl(unsigned int& num, Context &ctx, qi::unused_type) const {
                auto lit = builderCtx->createIntegralLiteral(num);
                boost::fusion::at_c<0>(ctx.attributes) = lit;
            }
        };

        struct Subterm : public ActionBase<Subterm> {

            struct AccessChainVisitor : public boost::static_visitor<node_t>
            {
                node_t operator()(node_t expr) const
                {
                    return builderCtx->createSubscriptAccess(acc, expr);
                }

                node_t operator()(const std::string & str) const
                {
                    return builderCtx->createMemberAccess(acc, str);
                }
                node_t acc;
            } mutable acv;

            template<typename Context>
            void impl(boost::fusion::vector<node_t , std::vector<boost::variant<node_t, std::string>>>& term, Context &ctx, qi::unused_type) const {
                auto acc = boost::fusion::at_c<0>(term);
                auto& accessChain = boost::fusion::at_c<1>(term);

                if(accessChain.empty()){
                    boost::fusion::at_c<0>(ctx.attributes) = acc;
                    return;
                }

                acc = std::accumulate(accessChain.begin(), accessChain.end(), acc, [this](node_t node, auto& a){
                    acv.acc = node;
                    return boost::apply_visitor( acv, a );
                });

                boost::fusion::at_c<0>(ctx.attributes) = acc;
            }
        };

        struct TerminalWithUnaryOp : public ActionBase<TerminalWithUnaryOp>{
            template<typename Context>
            void impl(boost::fusion::vector<UnaryOp, node_t>& term, Context &ctx, qi::unused_type) const {
                auto unexpr = builderCtx->createUnaryOpExpr(boost::fusion::at_c<1>(term), translate(boost::fusion::at_c<0>(term)));
                boost::fusion::at_c<0>(ctx.attributes) = unexpr;
            }


        };

        struct Pass : public ActionBase<Pass>{
            template<typename Context>
            void impl(node_t term, Context &ctx, qi::unused_type) const {
                boost::fusion::at_c<0>(ctx.attributes) = term;
            }
        };
        struct ScopeOpen : public ActionBase<ScopeOpen>{
            template<typename Context>
            void impl(boost::spirit::unused_type, Context &ctx, qi::unused_type) const {
                builderCtx->pushScope();
            }
        };

        struct ScopeClose : public ActionBase<ScopeClose>{
            template<typename Context>
            void impl(boost::spirit::unused_type, Context &ctx, qi::unused_type) const {
                    builderCtx->popScope();
            }
        };

        template<typename Op>
        struct BinaryOp : public ActionBase<BinaryOp<Op>>{
            template<typename Context>
            void impl(OperationSequence<Op>& term, Context &ctx, qi::unused_type) const {
                auto& sequence = boost::fusion::at_c<1>(term);

                if(sequence.empty()) {
                    boost::fusion::at_c<0>(ctx.attributes) = std::move(boost::fusion::at_c<0>(term));
                    return;
                }

                auto acc = std::move(boost::fusion::at_c<0>(term));

                for(auto&& elem: sequence){
                    auto op = translate(boost::fusion::at_c<0>(elem));
                    acc = builderCtx->createBinaryOpExpr(std::move(acc), std::move(boost::fusion::at_c<1>(elem)), op);
                }
                boost::fusion::at_c<0>(ctx.attributes) = std::move(acc);
            }
        };

        struct LogicAnd : public ActionBase<LogicAnd>{
            template<typename Context>
            void impl(boost::fusion::vector<node_t, std::vector<node_t>>& term, Context &ctx, qi::unused_type) const {
                auto& sequence = boost::fusion::at_c<1>(term);

                if(sequence.empty()) {
                    boost::fusion::at_c<0>(ctx.attributes) = boost::fusion::at_c<0>(term);
                    return;
                }

                auto acc = boost::fusion::at_c<0>(term);

                for(auto&& elem: sequence){
                    acc = builderCtx->createBinaryOpExpr(acc, elem, operator_t::AND);
                }
                boost::fusion::at_c<0>(ctx.attributes) = acc;
            }
        };

        struct LogicOr : public ActionBase<LogicOr> {
            template<typename Context>
            void impl(boost::fusion::vector<node_t, std::vector<node_t>>& term, Context &ctx, qi::unused_type) const {
                auto& sequence = boost::fusion::at_c<1>(term);

                if(sequence.empty()) {
                    boost::fusion::at_c<0>(ctx.attributes) = boost::fusion::at_c<0>(term);
                    return;
                }
                auto acc = boost::fusion::at_c<0>(term);

                for(auto&& elem: sequence){
                    acc = builderCtx->createBinaryOpExpr(acc, elem, operator_t::OR);
                }
                boost::fusion::at_c<0>(ctx.attributes) = acc;
            }
        };

        struct Identifier : public ActionBase<Identifier>{
            template<typename Context>
            void impl(std::string const &str, Context &ctx, qi::unused_type) const {
                boost::fusion::at_c<0>(ctx.attributes) = builderCtx->createReference(str);
            }
        };

        struct IntegralTypeWithBitwidth : public ActionBase<IntegralTypeWithBitwidth>{
            template<typename Context>
            void impl(unsigned int width, Context &ctx, qi::unused_type) const {
                boost::fusion::at_c<0>(ctx.attributes) = builderCtx->getIntegralType(width);
            }
        };

        struct BuiltInType : public ActionBase<BuiltInType>{
            template<typename Context>
            void impl(parasl::BuiltInType type, Context &ctx, qi::unused_type) const {
                auto& restype = boost::fusion::at_c<0>(ctx.attributes);
                switch(type){
                    case parasl::BuiltInType::INT: restype = builderCtx->getIntegralType(32); break;
                    case parasl::BuiltInType::FLOAT: restype = builderCtx->getFloatType(); break;
                    case parasl::BuiltInType::DOUBLE: restype = builderCtx->getDoubleType(); break;
                    case parasl::BuiltInType::CHAR: restype = builderCtx->getCharType(); break;
                }
            }
        };

        struct ArrayType : public ActionBase<ArrayType>{
            template<typename Context>
            void impl(boost::fusion::vector<type_t, unsigned int> const& type, Context &ctx, qi::unused_type) const {
                boost::fusion::at_c<0>(ctx.attributes) = builderCtx->getArrayType(boost::fusion::at_c<0>(type),
                        boost::fusion::at_c<1>(type));
            }
        };

        struct VectorType : public ActionBase<VectorType>{
            template<typename Context>
            void impl(boost::fusion::vector<type_t, unsigned int> const& type, Context &ctx, qi::unused_type) const {
                boost::fusion::at_c<0>(ctx.attributes) = builderCtx->getVectorType(boost::fusion::at_c<0>(type),
                                                                                  boost::fusion::at_c<1>(type));
            }
        };

        struct StructType : public ActionBase<StructType>{
            template<typename Context>
            void impl(boost::optional<std::vector<boost::fusion::vector<std::string, boost::optional<type_t>>>> const& members, Context &ctx, qi::unused_type) const {
                std::vector<std::pair<std::string, types::Type const*>> members_proc;
                if(members.has_value())
                    std::transform(members->begin(), members->end(), std::back_inserter(members_proc),
                                   [](auto const& elem){ return std::make_pair(boost::fusion::at_c<0>(elem),
                                                                          boost::fusion::at_c<1>(elem).has_value() ?
                                                                          boost::fusion::at_c<1>(elem).get() :
                                                                          nullptr);});

                boost::fusion::at_c<0>(ctx.attributes) = builderCtx->getStructType(members_proc);
            }
        };

        struct CompoundStatement : public ActionBase<CompoundStatement>{
            template<typename Context>
            void impl(std::vector<node_t>& statements, Context &ctx, qi::unused_type) const {
                boost::fusion::at_c<0>(ctx.attributes) = builderCtx->createCompoundStatement(std::move(statements));
            }

            template<typename Context>
            void impl(node_t statements, Context &ctx, qi::unused_type) const {
                boost::fusion::at_c<0>(ctx.attributes) = builderCtx->createCompoundStatement({statements});
            }
        };

        struct IfStatement : public ActionBase<IfStatement>{
            template<typename Context>
            void impl(boost::fusion::vector<node_t, node_t, boost::optional<node_t>>& if_stmt, Context &ctx, qi::unused_type) const {
                boost::fusion::at_c<0>(ctx.attributes) = builderCtx->createIfStatement(
                        boost::fusion::at_c<0>(if_stmt),
                        boost::fusion::at_c<1>(if_stmt),
                        boost::fusion::at_c<2>(if_stmt).has_value() ?
                        boost::fusion::at_c<2>(if_stmt).get() : nullptr
                );
            }
        };

        struct ForStatement : public ActionBase<ForStatement>{
            template<typename Context>
            void impl(boost::fusion::vector<std::string, boost::variant<std::string, node_t>> const&, Context &ctx, qi::unused_type) const {
            }
        };


        struct WhileStatement : public ActionBase<WhileStatement>{
            template<typename Context>
            void impl(node_t const& condition, Context &ctx, qi::unused_type) const {
            }
        };


        struct Range : public ActionBase<Range>{
            template<typename Context>
            void impl(boost::fusion::vector<int, int, boost::optional<int>> const& range, Context &ctx, qi::unused_type) const {
            }
        };


        struct Assignment : public ActionBase<Assignment>{
            template<typename Context>
            void impl(boost::fusion::vector<node_t, node_t> const& assignments, Context &ctx, qi::unused_type) const {
                boost::fusion::at_c<0>(ctx.attributes) = builderCtx->createBinaryOpExpr(
                        boost::fusion::at_c<0>(assignments),
                        boost::fusion::at_c<1>(assignments),
                        operator_t::ASSIGN);
            }
        };

        struct AssignmentStatement : public ActionBase<AssignmentStatement>{
            template<typename Context>
            void impl(node_t assignment, Context &ctx, qi::unused_type) const {
                boost::fusion::at_c<0>(ctx.attributes) = builderCtx->createAssignStatement(assignment);
            }
        };

        struct Declaration : public ActionBase<Declaration>{
            template<typename Context>
            void impl(boost::fusion::vector<std::string, boost::optional<type_t>, boost::optional<node_t>> const& decl, Context &ctx, qi::unused_type) const {
                auto& decl_type = boost::fusion::at_c<1>(decl);
                auto& initializer = boost::fusion::at_c<2>(decl);

                boost::fusion::at_c<0>(ctx.attributes) =
                        builderCtx->createDeclaration( boost::fusion::at_c<0>(decl), decl_type ? *decl_type : nullptr,
                                initializer ? *initializer : nullptr);
            }
        };


        struct OutputStatement : public ActionBase<OutputStatement>{
            template<typename Context>
            void impl(boost::fusion::vector<int, node_t> const& output, Context &ctx, qi::unused_type) const {
            }
        };

    };

}  // namespace parasl

#endif //PARASL_COMMON_H
