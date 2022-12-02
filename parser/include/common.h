#ifndef PARASL_COMMON_H
#define PARASL_COMMON_H

#include <variant>
#include <map>
#include <memory>
#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/spirit/include/support_extended_variant.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

#include "error_handler.h"

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
using qi::_val_type;

using qi::_1;
using qi::_2;
using qi::_3;
using qi::_4;

using qi::on_error;
using qi::fail;

using phx::function;

using SymGroup = qi::symbols<char>;

using StrIter = std::string::iterator;
using PrimType = std::variant<int, char, double, float>;
using IntType = std::variant<int8_t, int16_t, int32_t, int64_t>;

//class node_t
//        : public boost::spirit::extended_variant<
//                SyntaxNode> {
//public:
//    using value_type = SyntaxNode *;
//
//    node_t(node_t const & rhs) : base_type(node_t.getNode()) {}
//
//
//    base_type::variant_type       & getNode()       { return base_type::get(); }
//    base_type::variant_type const & getNode() const { return base_type::get(); }
//};
//using node_t = boost::variant<
//        SyntaxNode,
//        boost::recursive_wrapper<SyntaxNode>
//>;

using node_t = ChildSyntaxNode<> *;
//class node_t {
//public:
//    using value_type = SyntaxNode *;
//    value_type node;
//
//    node_t() : node(nullptr) {}
//    node_t(const std::string& str) : node(nullptr) {}
//    node_t(const int& i) : node(nullptr) {}
//    node_t(StrIter beg, StrIter end) : node(nullptr) {}
//
//    bool empty() const {
//        return true;
//    }
//
//    void insert(StrIter it, char val) {
//
//    }
//
//    StrIter end() {
//        return {};
//    }
//};

//using node_t = boost::make_recursive_variant<
//        std::string, std::vector<boost::recursive_variant_> >::type;

struct keywords_table : qi::symbols<char> {
    keywords_table() {
        add
                ("layer")
                ("input")
                ("output")
                ("repeat")
                ("glue")
                ("bind")
                ("if")
                ("else")
                ("for")
                ("in")
                ("return")
                ("char")
                ("int")
                ;
    }
};

}  // namespace parasl


#endif //PARASL_COMMON_H
