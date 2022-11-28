#ifndef PARASL_STMT_GRAMMAR_H
#define PARASL_STMT_GRAMMAR_H

#include <iostream>

#include "statements.h"
#include "skipper.h"
#include "common.h"
#include "expr_grammar.h"

namespace parasl {

template<typename Iterator>
struct stmt_grammar : qi::grammar<Iterator, node_t(), Skipper<Iterator>> {
    stmt_grammar() : stmt_grammar::base_type{STMTS} {
        STMTS = 
                STMT 
                | STMTS
                ;
        
        STMT =
                INIT_VAR_WITH_TYPE_STMT >> char_(';')
                | EXPR >> char_(';')
                ;

        INIT_VAR_WITH_TYPE_STMT =
                NAME >> char_(':') >> VAR_TYPE
                | NAME >> char_(':') >> VAR_TYPE >> char_('=') >> EXPR
                ;

        VAR_TYPE =
                VAR_BUILTIN_TYPE
                | VAR_TYPE_WITH_BIT_SIZE
                ;

        VAR_TYPE_WITH_BIT_SIZE = qi::int_ >> '(' >> qi::int_ >> ')';

        VAR_BUILTIN_TYPE = qi::int_ | qi::char_ | qi::double_ | qi::float_;

        NAME =
                !lexeme[keywords_t >> !(alnum | '_')]
                >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
                ;

        BOOST_SPIRIT_DEBUG_NODES(
                (INIT_VAR_WITH_TYPE_STMT)
                (VAR_TYPE)
                (VAR_TYPE_WITH_BIT_SIZE)
                (VAR_BUILTIN_TYPE)
                (NAME)
        )
    }

private:
    expression_grammar<Iterator> EXPR;
    qi::rule<Iterator, node_t(), Skipper<Iterator>>
//            INIT_VAR_WITH_TYPE_STMT, VAR_TYPE, STMT, STMTS;
            NAME, INIT_VAR_WITH_TYPE_STMT, VAR_TYPE, VAR_TYPE_WITH_BIT_SIZE, VAR_BUILTIN_TYPE, STMT, STMTS;
//    qi::rule<Iterator, PrimType(), Skipper<Iterator>> VAR_BUILTIN_TYPE;
//    qi::rule<Iterator, IntType(), Skipper<Iterator>> VAR_TYPE_WITH_BIT_SIZE;
//    qi::rule<Iterator, std::string(), Skipper<Iterator>> NAME;
};

}  // namespace parasl


#endif //PARASL_STMT_GRAMMAR_H