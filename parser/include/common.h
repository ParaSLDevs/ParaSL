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

using SymGroup = qi::symbols<char>;
using StrIter = std::string::iterator;
using node_t = basic_syntax_nodes::ChildSyntaxNode<> *;

struct keywords_list : SymGroup {
    keywords_list() {
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

struct prim_types_list : SymGroup {
    prim_types_list() {
        add
                ("int")
                ("char")
                ("float")
                ("double")
                ;
    }
};

}  // namespace parasl


#endif //PARASL_COMMON_H
