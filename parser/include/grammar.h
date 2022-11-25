#ifndef PARASL_GRAMMAR_H
#define PARASL_GRAMMAR_H

#include <boost/spirit/include/qi.hpp>
#include <iostream>

#include "stmt_grammar.h"

namespace parasl {

using brac_tree = boost::make_recursive_variant<
        std::string, std::vector<boost::recursive_variant_> >::type;

namespace qi = boost::spirit::qi;

template<typename It, typename Skipper>
struct layer0_grammar : qi::grammar<It, brac_tree(), Skipper> {
    layer0_grammar() : layer0_grammar::base_type{brackets} {
        brackets = simple | nested;

        nested = '[' >> *brackets >> ']';

        simple = +~qi::char_("][");

        BOOST_SPIRIT_DEBUG_NODES((simple)(nested)(brackets))
    }

private:
    qi::rule<It, std::string(), Skipper> simple;
    qi::rule<It, std::vector<brac_tree>(), Skipper> nested;
    qi::rule<It, brac_tree(), Skipper> brackets;

};

}  // namespace parasl


#endif //PARASL_GRAMMAR_H
