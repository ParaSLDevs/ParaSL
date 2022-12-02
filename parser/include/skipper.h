#ifndef PARASL_SKIPPER_H
#define PARASL_SKIPPER_H

#include <boost/spirit/include/qi.hpp>

#include "common.h"

namespace parasl {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

// The skipper grammar
template <typename Iterator>
struct Skipper : qi::grammar<Iterator> {
    Skipper() : Skipper::base_type(start) {
        ascii::space_type space;
        qi::eol_type eol;

        start =
                    space                                 // tab/space/cr/lf
                |   ("//" >> *(char_ - eol) >> eol)       // C-style comments
                ;
    }

private:
    qi::rule<Iterator> start;
};

}  // namespace parasl

#endif //PARASL_SKIPPER_H
