#ifndef PARASL_STMT_GRAMMAR_H
#define PARASL_STMT_GRAMMAR_H

#include <iostream>
#include <variant>

#include "statements.h"
#include "skipper.h"
#include "common.h"
#include "expression.h"

namespace parasl {

using PrimType = std::variant<int, double, char, float>;
using IntType = std::variant<int8_t, int16_t, int32_t, int64_t>;

template<typename Iterator>
struct stmt_grammar : qi::grammar<Iterator, DeclStmt(), Skipper<Iterator>> {
    stmt_grammar() : stmt_grammar::base_type{INIT_VAR_WITH_TYPE_STMT} {

        INIT_VAR_WITH_TYPE_STMT =
                NAME >> ':' >> VAR_TYPE
                | NAME >> ':' >> VAR_TYPE >> '=' >> EXPR
                ;

        VAR_TYPE =
                VAR_BUILTIN_TYPE
                | VAR_TYPE_WITH_BIT_SIZE
                ;

        VAR_TYPE_WITH_BIT_SIZE = qi::int_ >> '(' >> qi::int_ >> ')';

        VAR_BUILTIN_TYPE = qi::int_ | qi::double_ | qi::char_ | qi::float_;

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
    qi::rule<Iterator, DeclStmt(), Skipper<Iterator>> INIT_VAR_WITH_TYPE_STMT;
    qi::rule<Iterator, std::variant<PrimType, IntType>(), Skipper<Iterator>> VAR_TYPE;
    qi::rule<Iterator, IntType(), Skipper<Iterator>> VAR_TYPE_WITH_BIT_SIZE;
    qi::rule<Iterator, PrimType(), Skipper<Iterator>> VAR_BUILTIN_TYPE;
    qi::rule<Iterator, std::string(), Skipper<Iterator>> NAME;
};

}  // namespace parasl


#endif //PARASL_STMT_GRAMMAR_H
