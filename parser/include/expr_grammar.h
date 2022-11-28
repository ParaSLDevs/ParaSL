#ifndef PARASL_EXPR_GRAMMAR_H
#define PARASL_EXPR_GRAMMAR_H

#include "common.h"

namespace parasl {

using num_t = std::variant<PrimType, IntType>;
using term_t = std::variant<num_t, node_t>;

template<typename Iterator>
struct expression_grammar : qi::grammar<Iterator, node_t(), Skipper<Iterator>> {
    expression_grammar() : expression_grammar::base_type{EXPR} {
        EXPR =
                EXPR >> char_('=') >> EXPR
                | OR_EXPR
                ;

        OR_EXPR =
                OR_EXPR >> "||" >> OR_EXPR
                | AND_EXPR
                ;

        AND_EXPR =
                AND_EXPR >> "&&" >> AND_EXPR
                | EQUALITY_EXPR
                ;

        EQUALITY_EXPR =
                EQUALITY_EXPR >> "==" >> EQUALITY_EXPR
                | EQUALITY_EXPR >> "!=" >> EQUALITY_EXPR
                | LESS_OR_GREATER_EXPR
                ;

        LESS_OR_GREATER_EXPR =
                LESS_OR_GREATER_EXPR >> '>' >> LESS_OR_GREATER_EXPR
                | LESS_OR_GREATER_EXPR >> '<' >> LESS_OR_GREATER_EXPR
                | LESS_OR_GREATER_EXPR >> ">=" >> LESS_OR_GREATER_EXPR
                | LESS_OR_GREATER_EXPR >> "<=" >> LESS_OR_GREATER_EXPR
                | ADD_OR_MINUS_EXPR
                ;

        ADD_OR_MINUS_EXPR =
                ADD_OR_MINUS_EXPR >> '+' >> ADD_OR_MINUS_EXPR
                | ADD_OR_MINUS_EXPR >> '-' >> ADD_OR_MINUS_EXPR
                | MULT
                ;

        MULT =
                MULT >> '*' >> MULT
                | MULT >> '\\' >> MULT
                | NOT_EXPR
                ;

        NOT_EXPR =
                '!' >> NOT_EXPR
                | DOT_OR_BRAKET_EXPR;

        DOT_OR_BRAKET_EXPR =
                DOT_OR_BRAKET_EXPR >> '.' >> DOT_OR_BRAKET_EXPR
                | DOT_OR_BRAKET_EXPR >> '(' >> -(OR_EXPR % ',') >> ')'
                | DOT_OR_BRAKET_EXPR >> '[' >> OR_EXPR >> ']'
                | TERM
                ;

        TERM =
                NUM
                | lit("input") >> '(' >> int_(0) >> ')'
                | '(' >> EXPR >> ')'
                ;
    }

private:
    qi::rule<Iterator, node_t(), Skipper<Iterator>>
            NUM, TERM, DOT_OR_BRAKET_EXPR, NOT_EXPR, MULT, ADD_OR_MINUS_EXPR,
//            TERM, DOT_OR_BRAKET_EXPR, NOT_EXPR, MULT, ADD_OR_MINUS_EXPR,
            LESS_OR_GREATER_EXPR, EQUALITY_EXPR, AND_EXPR, OR_EXPR, EXPR;
//    qi::rule<Iterator, PrimType(), Skipper<Iterator>> NUM;
};

}  // namespace parasl

#endif //PARASL_EXPR_GRAMMAR_H
