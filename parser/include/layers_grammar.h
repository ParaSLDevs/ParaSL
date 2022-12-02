#ifndef PARASL_LAYERS_GRAMMAR_H
#define PARASL_LAYERS_GRAMMAR_H

#include <boost/spirit/include/qi.hpp>
#include <iostream>

#include "stmt_grammar.h"

namespace parasl {

template<typename Iterator, typename Skipper>
struct layers_grammar : qi::grammar<Iterator, node_t(), Skipper> {
    layers_grammar(error_handler<Iterator>& error_handler)
                    : layers_grammar::base_type{LAYERS}, STMTS(error_handler) {
        using error_handler_function = function<parasl::error_handler<Iterator>>;

        LAYER =
//                lit["layer"] >> '(' >> int_ >> char_(',') >> string >> ')' >> char_('{') >> STMTS >> char_('}')
                STMTS
                ;

        LAYERS =
                LAYER
//                | LAYERS
                ;

        BOOST_SPIRIT_DEBUG_NODES(
                (LAYERS)
                (LAYER)
                (STMTS)
        )

        // Error handling: on error in LAYERS, call error_handler.
        on_error<fail>(LAYERS,
                       error_handler_function(error_handler)(
                               "Error! Expecting ", _4, _3));
    }

private:
    stmt_grammar<Iterator, Skipper> STMTS;
    qi::rule<Iterator, node_t(), Skipper> LAYERS, LAYER;
};

}  // namespace parasl


#endif //PARASL_LAYERS_GRAMMAR_H
