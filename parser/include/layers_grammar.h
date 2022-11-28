#ifndef PARASL_LAYERS_GRAMMAR_H
#define PARASL_LAYERS_GRAMMAR_H

#include <boost/spirit/include/qi.hpp>
#include <iostream>

#include "stmt_grammar.h"

namespace parasl {

template<typename Iterator>
struct layers_grammar : qi::grammar<Iterator, node_t(), Skipper<Iterator>> {
    layers_grammar() : layers_grammar::base_type{LAYERS} {
        LAYERS =
                LAYER
                | LAYERS
                ;

        LAYER =
//                lit["layer"] >> '(' >> int_ >> char_(',') >> string >> ')' >> char_('{') >> STMTS >> char_('}')
                STMTS
                ;

        BOOST_SPIRIT_DEBUG_NODES(
                (LAYER)
                (LAYERS)
                (STMTS)
        )
    }

private:
    stmt_grammar<Iterator> STMTS;
    qi::rule<Iterator, node_t(), Skipper<Iterator>> LAYER, LAYERS;
};

}  // namespace parasl


#endif //PARASL_LAYERS_GRAMMAR_H
