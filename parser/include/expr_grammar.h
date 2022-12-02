#ifndef PARASL_EXPR_GRAMMAR_H
#define PARASL_EXPR_GRAMMAR_H

#include "common.h"

namespace parasl {

template<typename Iterator, typename Skipper>
struct expression_grammar : qi::grammar<Iterator, node_t(), Skipper> {
    expression_grammar(error_handler<Iterator>& error_handler) : expression_grammar::base_type{EXPR} {
        using error_handler_function = function<parasl::error_handler<Iterator>>;

        UNARY_OP.add
                ("+")
                ("-")
                ("!")
                ;

        MULT_OP.add
                ("*")
                ("/")
                ;

        ADD_OP.add
                ("+")
                ("-")
                ;

        RELATION_OP.add
                ("<")
                ("<=")
                (">")
                (">=")
                ;

        EQUALITY_OP.add
                ("==")
                ("!=")
                ;

        NAME =
                    !lexeme[keywords_t >> !(alnum | '_')]
                >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
                ;

        FUNC_CALL =
                    (NAME >> '(')                              [qi::_1]
                >   -(OR_EXPR % ',')    // arg list
                >   ')'
                ;

        DOT_EXPR =
                    (NAME >> '.')
                >   NAME                                       [qi::_1]
                ;

        SQUARE_BRAKET_EXPR =
                    NAME                                       [qi::_1]
                >>  '[' > OR_EXPR > ']'
                ;

        TERM =
                    qi::uint_                                  [qi::_1]
                |   FUNC_CALL
                |   DOT_EXPR
                |   SQUARE_BRAKET_EXPR
                |   NAME                                       [qi::_1]
//                |   lit("input") > '(' > int_(0) >> ')'     // : int  ??  EXPR or STMT?
                |   '(' > EXPR > ')'
                ;

        UNARY_EXPR =
                    TERM
                |   (UNARY_OP > TERM)
                ;

        MULT =
                    UNARY_EXPR
                >>  *(MULT_OP > UNARY_EXPR)
                ;

        ADD_OR_MINUS_EXPR =
                    MULT
                >>  *(ADD_OP > MULT)
                ;

        LESS_OR_GREATER_EXPR =
                    ADD_OR_MINUS_EXPR
                >>  *(RELATION_OP > ADD_OR_MINUS_EXPR)
                ;

        EQUALITY_EXPR =
                    LESS_OR_GREATER_EXPR
                >>  *(EQUALITY_OP > LESS_OR_GREATER_EXPR)
                ;

        AND_EXPR =
                    EQUALITY_EXPR
                >>  *("&&" > EQUALITY_EXPR)
                ;

        OR_EXPR =
                    AND_EXPR
                >>  *("||" > AND_EXPR)
                ;

        EXPR =
                    OR_EXPR
                >>  *("=" > OR_EXPR)
                ;

        // PART OF STMTS TO EXPR
        // WHAT IF glue() - expression inside another expr but not assignment

        BOOST_SPIRIT_DEBUG_NODES(
                (TERM)(DOT_EXPR)(SQUARE_BRAKET_EXPR)(MULT)(UNARY_EXPR)(ADD_OR_MINUS_EXPR)
                (LESS_OR_GREATER_EXPR)(EQUALITY_EXPR)(AND_EXPR)(OR_EXPR)(EXPR)
                (NUM)
        )

        // Error handling: on error in EXPR, call error_handler.
        on_error<fail>(EXPR,
                       error_handler_function(error_handler)(
                       "Error while expression parsing! Expecting ", _4, _3));
    }

private:
    keywords_table keywords_t;

    SymGroup UNARY_OP;
    SymGroup MULT_OP;
    SymGroup ADD_OP;
    SymGroup RELATION_OP;
    SymGroup EQUALITY_OP;

    qi::rule<Iterator, node_t(), Skipper>
            FUNC_CALL, TERM, DOT_EXPR, SQUARE_BRAKET_EXPR, UNARY_EXPR, MULT, ADD_OR_MINUS_EXPR,
            LESS_OR_GREATER_EXPR, EQUALITY_EXPR, AND_EXPR, OR_EXPR, EXPR;
    qi::rule<Iterator, uint32_t(), Skipper> NUM;
    qi::rule<Iterator, std::string(), Skipper> NAME;
};

}  // namespace parasl

#endif //PARASL_EXPR_GRAMMAR_H
