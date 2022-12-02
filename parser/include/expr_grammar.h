#ifndef PARASL_EXPR_GRAMMAR_H
#define PARASL_EXPR_GRAMMAR_H

#include "common.h"

namespace parasl {

//using num_t = std::variant<PrimType, IntType>;
//using term_t = std::variant<num_t, node_t>;[ std::cout << qi::_1 << std::endl]

template<typename Iterator, typename Skipper>
struct expression_grammar : qi::grammar<Iterator, node_t(), Skipper> {
    expression_grammar(error_handler<Iterator>& error_handler) : expression_grammar::base_type{EXPR} {
        using error_handler_function = function<parasl::error_handler<Iterator>>;

        NAME =
                !lexeme[keywords_t >> !(alnum | '_')]
                >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
                ;

        TERM =
                qi::uint_ [qi::_1]
                | NAME  [qi::_1]
                | lit("input") >> '(' >> int_(0) >> ')'
                | char_('(') > EXPR_ > char_(')')
                ;

        DOT_OR_BRAKET_EXPR_ =
                TERM >> '.' >> TERM
                | TERM >> '(' >> -(OR_EXPR % ',') >> ')'
                | TERM >> '[' >> TERM >> ']'
                | TERM
                ;

        DOT_OR_BRAKET_EXPR =
                DOT_OR_BRAKET_EXPR_ >> '.' >> DOT_OR_BRAKET_EXPR_
                | DOT_OR_BRAKET_EXPR_ >> '(' >> -(OR_EXPR % ',') >> ')'
                | DOT_OR_BRAKET_EXPR_ >> '[' >> OR_EXPR >> ']'
                | DOT_OR_BRAKET_EXPR_
                ;

        NOT_EXPR_ =
                '!' >> DOT_OR_BRAKET_EXPR
                | DOT_OR_BRAKET_EXPR
                ;

        NOT_EXPR =
                '!' >> NOT_EXPR_
                | NOT_EXPR_
                ;

        MULT_ =
                NOT_EXPR >> '*' >> NOT_EXPR
                | NOT_EXPR >> '\\' >> NOT_EXPR
                | NOT_EXPR
                ;

        MULT =
                MULT_ >> '*' >> MULT_
                | MULT_ >> '\\' >> MULT_
                | MULT_
                ;

        ADD_OR_MINUS_EXPR_ =
                MULT >> '+' >> MULT
                | MULT >> '-' >> MULT
                | MULT
                ;

        ADD_OR_MINUS_EXPR =
                ADD_OR_MINUS_EXPR_ >> '+' >> ADD_OR_MINUS_EXPR_
                | ADD_OR_MINUS_EXPR_ >> '-' >> ADD_OR_MINUS_EXPR_
                | ADD_OR_MINUS_EXPR_
                ;

        LESS_OR_GREATER_EXPR_ =
                ADD_OR_MINUS_EXPR >> '>' >> ADD_OR_MINUS_EXPR
                | ADD_OR_MINUS_EXPR >> '<' >> ADD_OR_MINUS_EXPR
                | ADD_OR_MINUS_EXPR >> ">=" >> ADD_OR_MINUS_EXPR
                | ADD_OR_MINUS_EXPR >> "<=" >> ADD_OR_MINUS_EXPR
                | ADD_OR_MINUS_EXPR
                ;

        LESS_OR_GREATER_EXPR =
                LESS_OR_GREATER_EXPR_ >> '>' >> LESS_OR_GREATER_EXPR_
                | LESS_OR_GREATER_EXPR_ >> '<' >> LESS_OR_GREATER_EXPR_
                | LESS_OR_GREATER_EXPR_ >> ">=" >> LESS_OR_GREATER_EXPR_
                | LESS_OR_GREATER_EXPR_ >> "<=" >> LESS_OR_GREATER_EXPR_
                | LESS_OR_GREATER_EXPR_
                ;

        EQUALITY_EXPR_ =
                LESS_OR_GREATER_EXPR >> "==" >> LESS_OR_GREATER_EXPR
                | LESS_OR_GREATER_EXPR >> "!=" >> LESS_OR_GREATER_EXPR
                | LESS_OR_GREATER_EXPR
                ;

        EQUALITY_EXPR =
                EQUALITY_EXPR_ >> "==" >> EQUALITY_EXPR_
                | EQUALITY_EXPR_ >> "!=" >> EQUALITY_EXPR_
                | EQUALITY_EXPR_
                ;

        AND_EXPR_ =
                EQUALITY_EXPR >> "&&" >> EQUALITY_EXPR
                | EQUALITY_EXPR
                ;

        AND_EXPR =
                AND_EXPR_ >> "&&" >> AND_EXPR_
                | AND_EXPR_
                ;

        OR_EXPR_ =
                AND_EXPR >> "||" >> AND_EXPR
                | AND_EXPR
                ;

        OR_EXPR =
                OR_EXPR_ >> "||" >> OR_EXPR_
                | OR_EXPR_
                ;

        EXPR_ =
                OR_EXPR >> char_('=') >> OR_EXPR
                | OR_EXPR
                ;

        EXPR =
                EXPR_ >> char_('=') >> EXPR_
                | EXPR_
                ;

        BOOST_SPIRIT_DEBUG_NODES(
                (TERM)(DOT_OR_BRAKET_EXPR)(NOT_EXPR)(MULT)(ADD_OR_MINUS_EXPR)
                (LESS_OR_GREATER_EXPR)(EQUALITY_EXPR)(AND_EXPR)(OR_EXPR)(EXPR)
                (DOT_OR_BRAKET_EXPR_)(NOT_EXPR_)(MULT_)(ADD_OR_MINUS_EXPR_)
                (LESS_OR_GREATER_EXPR_)(EQUALITY_EXPR_)(AND_EXPR_)(OR_EXPR_)(EXPR_)
                (NUM)
        )

        // Error handling: on error in EXPR, call error_handler.
        on_error<fail>(EXPR,
                       error_handler_function(error_handler)(
                               "Error! Expecting ", _4, _3));
    }

private:
    keywords_table keywords_t;
    qi::rule<Iterator, node_t(), Skipper>
            TERM, DOT_OR_BRAKET_EXPR, NOT_EXPR, MULT, ADD_OR_MINUS_EXPR,
            LESS_OR_GREATER_EXPR, EQUALITY_EXPR, AND_EXPR, OR_EXPR, EXPR;
    qi::rule<Iterator, node_t(), Skipper>
            DOT_OR_BRAKET_EXPR_, NOT_EXPR_, MULT_, ADD_OR_MINUS_EXPR_,
            LESS_OR_GREATER_EXPR_, EQUALITY_EXPR_, AND_EXPR_, OR_EXPR_, EXPR_;
    qi::rule<Iterator, uint32_t(), Skipper> NUM;
    qi::rule<Iterator, std::string(), Skipper> NAME;
};

}  // namespace parasl


//        EXPR =
//                EXPR >> char_('=') >> EXPR
//                | OR_EXPR
//                ;
//
//        OR_EXPR =
//                OR_EXPR >> "||" >> OR_EXPR
//                | AND_EXPR
//                ;
//
//        AND_EXPR =
//                AND_EXPR >> "&&" >> AND_EXPR
//                | EQUALITY_EXPR
//                ;
//
//        EQUALITY_EXPR =
//                EQUALITY_EXPR >> "==" >> EQUALITY_EXPR
//                | EQUALITY_EXPR >> "!=" >> EQUALITY_EXPR
//                | LESS_OR_GREATER_EXPR
//                ;
//
//        LESS_OR_GREATER_EXPR =
//                LESS_OR_GREATER_EXPR >> '>' >> LESS_OR_GREATER_EXPR
//                | LESS_OR_GREATER_EXPR >> '<' >> LESS_OR_GREATER_EXPR
//                | LESS_OR_GREATER_EXPR >> ">=" >> LESS_OR_GREATER_EXPR
//                | LESS_OR_GREATER_EXPR >> "<=" >> LESS_OR_GREATER_EXPR
//                | ADD_OR_MINUS_EXPR
//                ;
//
//        ADD_OR_MINUS_EXPR =
//                ADD_OR_MINUS_EXPR >> '+' >> ADD_OR_MINUS_EXPR
//                | ADD_OR_MINUS_EXPR >> '-' >> ADD_OR_MINUS_EXPR
//                | MULT
//                ;
//
//        MULT =
//                MULT >> '*' >> MULT
//                | MULT >> '\\' >> MULT
//                | NOT_EXPR
//                ;
//
//        NOT_EXPR =
//                '!' >> NOT_EXPR
//                | DOT_OR_BRAKET_EXPR;
//
//        DOT_OR_BRAKET_EXPR =
//                DOT_OR_BRAKET_EXPR >> '.' >> DOT_OR_BRAKET_EXPR
//                | DOT_OR_BRAKET_EXPR >> '(' >> -(OR_EXPR % ',') >> ')'
//                | DOT_OR_BRAKET_EXPR >> '[' >> OR_EXPR >> ']'
//                | TERM
//                ;
//
//        TERM =
//                NUM [std::cout << qi::_1 << std::endl]
//                | lit("input") >> '(' >> int_(0) >> ')'
//                | '(' >> EXPR >> ')'
//                ;


#endif //PARASL_EXPR_GRAMMAR_H
