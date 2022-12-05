#ifndef PARASL_COMMON_H
#define PARASL_COMMON_H

#include <map>
#include <memory>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

#include "error_handler.h"
#include "syntax_node.h"

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

using node_t = basic_syntax_nodes::ChildSyntaxNode<> *;

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
        template<typename Operation>
        using OperationSequence = boost::fusion::vector<node_t, std::vector<boost::fusion::vector<Operation, node_t>>>;


        struct IntegralLiteral {
            template<typename Context>
            void operator()(unsigned int const &num, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed integral literal " << num << std::endl;
                //_val = (node_t) (0xff0f);
                // set output value
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };

        struct Terminal {
            template<typename Context>
            void operator()(node_t const &subexpr, Context &ctx, qi::unused_type) const {
                //std::cout << "Parsed term " << (unsigned long) (subexpr) << std::endl;
            }
        };

        struct TerminalWithUnaryOp {
            template<typename Context>
            void operator()(boost::fusion::vector<UnaryOp, node_t> const&term, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed term " << boost::fusion::at_c<1>(term) << " with op " << (unsigned int)boost::fusion::at_c<0>(term) << std::endl;
            }
        };

        struct Multiplication {
            template<typename Context>
            void operator()(OperationSequence<MultOp> const&term, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed mult sequence of " << (boost::fusion::at_c<1>(term).size() + 1) << std::endl;
            }
        };

        struct Addition {
            template<typename Context>
            void operator()(OperationSequence<AddOp> const&term, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed add sequence of " << (boost::fusion::at_c<1>(term).size() + 1) << std::endl;
            }
        };

        struct Relation {
            template<typename Context>
            void operator()(OperationSequence<RelOp> const&term, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed rel sequence of " << (boost::fusion::at_c<1>(term).size() + 1) << std::endl;
            }
        };

        struct Equality {
            template<typename Context>
            void operator()(OperationSequence<EqOp> const&term, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed eq sequence of " << (boost::fusion::at_c<1>(term).size() + 1) << std::endl;
            }
        };

        struct LogicAnd {
            template<typename Context>
            void operator()(boost::fusion::vector<node_t, std::vector<node_t>> const&term, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed && of " << (boost::fusion::at_c<1>(term).size() + 1) << std::endl;
            }
        };

        struct LogicOr {
            template<typename Context>
            void operator()(boost::fusion::vector<node_t, std::vector<node_t>> const&term, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed || of " << (boost::fusion::at_c<1>(term).size() + 1) << std::endl;
            }
        };

        struct StringLiteral {
            template<typename Context>
            void operator()(std::string const &str, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed string literal \"" << str << "\"" << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };

        struct IntegralTypeWithBitwidth{
            template<typename Context>
            void operator()(unsigned int width, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed integral type with bitwidth " << width  << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };

        struct BuiltInType{
            template<typename Context>
            void operator()(parasl::BuiltInType type, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed builtin type "  << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };

        struct ArrayType{
            template<typename Context>
            void operator()(boost::fusion::vector<node_t, unsigned int> const& type, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed array type with size " << boost::fusion::at_c<1>(type) << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };

        struct VectorType{
            template<typename Context>
            void operator()(boost::fusion::vector<node_t, unsigned int> const& type, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed vector type with size " << boost::fusion::at_c<1>(type) << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };

        struct StructType{
            template<typename Context>
            void operator()(boost::optional<std::vector<node_t>> const& members, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed struct type with " << members.has_value() << "members" << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };

        struct CompoundStatement{
            template<typename Context>
            void operator()(std::vector<node_t> const& statements, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed scope with " << statements.size() << " statements" << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };

        struct IfStatement{
            template<typename Context>
            void operator()(boost::fusion::vector<node_t, node_t, boost::optional<node_t>> const&, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed \'if\' stmt" << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };

        struct ForStatement{
            template<typename Context>
            void operator()(boost::fusion::vector<std::string, boost::variant<std::string, node_t>> const&, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed \'for\' stmt" << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };


        struct WhileStatement{
            template<typename Context>
            void operator()(node_t const& condition, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed \'while\' stmt" << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };


        struct Range{
            template<typename Context>
            void operator()(boost::fusion::vector<int, int, boost::optional<int>> const& range, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed range" << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };


        struct Assignment{
            template<typename Context>
            void operator()(boost::fusion::vector<node_t, std::vector<node_t>> const& assignments, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed assignment" << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };


        struct Declaration{
            template<typename Context>
            void operator()(boost::fusion::vector<std::string, node_t> const& decl, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed decl" << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };


        struct OutputStatement{
            template<typename Context>
            void operator()(boost::fusion::vector<int, node_t> const& output, Context &ctx, qi::unused_type) const {
                std::cout << "Parsed output statement" << std::endl;
                //boost::fusion::at_c<0>(ctx.attributes) = (node_t) (0xff0f);
            }
        };

    };

}  // namespace parasl

#endif //PARASL_COMMON_H
