#ifndef PARASL_LAYERS_GRAMMAR_H
#define PARASL_LAYERS_GRAMMAR_H

#include <boost/spirit/include/qi.hpp>
#include <iostream>

#include "layer0_grammar_def.h"

namespace parasl {

template<typename Iterator, typename Skipper>
struct layers_grammar : qi::grammar<Iterator, node_t(), Skipper> {
    layers_grammar(error_handler<Iterator>& error_handler)
                    : layers_grammar::base_type{LAYERS}, LAYER0(error_handler) {
        using error_handler_function = function<parasl::error_handler<Iterator>>;

        LAYERS =
                    LAYER0
                ;

        BOOST_SPIRIT_DEBUG_NODES(
                (LAYERS)
                (LAYER0)
        )

        // Error handling: on error in LAYERS, call error_handler.
        on_error<fail>(LAYERS,
                       error_handler_function(error_handler)(
                               "Error! Expecting ", _4, _3));
    }

private:
    layer0_grammar<Iterator, Skipper> LAYER0;
    qi::rule<Iterator, node_t(), Skipper> LAYERS;
};

}  // namespace parasl


#endif //PARASL_LAYERS_GRAMMAR_H
