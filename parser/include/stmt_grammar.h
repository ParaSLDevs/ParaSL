#ifndef PARASL_STMT_GRAMMAR_H
#define PARASL_STMT_GRAMMAR_H

#include <iostream>

#include "statements.h"
#include "skipper.h"
#include "common.h"
#include "expr_grammar.h"

namespace parasl {

// int_ [ std::cout << qi::_1 << std::endl]
//STMTS =
//STMT [ std::cout << qi::_1 << std::endl]
//| STMTS [ std::cout << qi::_1 << std::endl]
//;

template<typename Iterator, typename Skipper>
struct stmt_grammar : qi::grammar<Iterator, node_t(), Skipper> {
    stmt_grammar(error_handler<Iterator>& error_handler) :
                stmt_grammar::base_type{STMTS}, EXPR(error_handler) {
        using error_handler_function = function<parasl::error_handler<Iterator>>;

        NAME =
                !lexeme[keywords_t >> !(alnum | '_')]
                >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
                ;

        VAR_TYPE_WITH_BIT_SIZE = lit("int") >> '(' >> qi::int_ >> ')';

        VAR_BUILTIN_TYPE = lit("int") | lit("char") | lit("double") | lit("float");

        VAR_TYPE =
                VAR_TYPE_WITH_BIT_SIZE
                | VAR_BUILTIN_TYPE
                ;

        INIT_VAR_WITH_TYPE_STMT =
                NAME >> char_(':') >> VAR_TYPE [qi::_1]
                | NAME >> char_(':') >> VAR_TYPE >> char_('=') >> EXPR [qi::_1]
                ;

        INIT_VAR_WITHOUT_TYPE_STMT =
                NAME >> char_('=') >> EXPR [std::cout << qi::_1 << std::endl]
                ;

        STMT =
                INIT_VAR_WITH_TYPE_STMT >> char_(';')
                | INIT_VAR_WITHOUT_TYPE_STMT >> char_(';')
                | EXPR >> char_(';')
                ;

        STMTS = +STMT [qi::_1];

        BOOST_SPIRIT_DEBUG_NODES(
                (INIT_VAR_WITH_TYPE_STMT)
                (VAR_TYPE)
                (VAR_TYPE_WITH_BIT_SIZE)
                (VAR_BUILTIN_TYPE)
                (NAME)
        )

        // Error handling: on error in STMTS, call error_handler.
        on_error<fail>(STMTS,
                       error_handler_function(error_handler)(
                               "Error! Expecting ", _4, _3));
    }

private:
    keywords_table keywords_t;
    expression_grammar<Iterator, Skipper> EXPR;

    qi::rule<Iterator, node_t(), Skipper>
            INIT_VAR_WITH_TYPE_STMT, INIT_VAR_WITHOUT_TYPE_STMT, STMT, STMTS;
    qi::rule<Iterator, std::string(), Skipper> NAME, VAR_TYPE_WITH_BIT_SIZE, VAR_BUILTIN_TYPE, VAR_TYPE;
};

}  // namespace parasl


#endif //PARASL_STMT_GRAMMAR_H
