#ifndef PARASL_COMMON_H
#define PARASL_COMMON_H

#include <boost/spirit/include/qi.hpp>

namespace parasl {

namespace qi = boost::spirit::qi;

using qi::char_;
using qi::uint_;
using qi::_val;
using qi::raw;
using qi::lexeme;
using qi::alpha;
using qi::alnum;

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
