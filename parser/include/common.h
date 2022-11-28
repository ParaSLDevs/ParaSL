#ifndef PARASL_COMMON_H
#define PARASL_COMMON_H

#include <variant>
#include <map>
#include <memory>
#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/spirit/include/support_extended_variant.hpp>

#include <boost/spirit/include/qi.hpp>

namespace parasl {

namespace qi = boost::spirit::qi;

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
////        std::string, int, char, double, float, int8_t, int16_t, int32_t, int64_t, _val_type
////        , PrimType
////        , IntType
//        SyntaxNode,
//        boost::recursive_wrapper<SyntaxNode>
//>;
template <typename T>
class node_t {
public:
    typedef T value_type;

    T x;
};


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
} keywords_t;


}  // namespace parasl


#endif //PARASL_COMMON_H
