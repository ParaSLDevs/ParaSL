#ifndef PARASL_LAYER0_GRAMMAR_DEF_H
#define PARASL_LAYER0_GRAMMAR_DEF_H

#include "layer0_grammar.h"

namespace parasl {

template<typename Iterator, typename Skipper>
layer0_grammar<Iterator, Skipper>::layer0_grammar(error_handler<Iterator>& error_handler) :
        layer0_grammar::base_type{LAYER0} {
    using error_handler_function = function<parasl::error_handler<Iterator>>;

    /*
     * =============================================
     *          Expression parsing
     * =============================================
     */

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
            |   ENTITY_EXPR                                       [qi::_1]
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

    BOOST_SPIRIT_DEBUG_NODES(
            (TERM)(DOT_EXPR)(SQUARE_BRAKET_EXPR)(MULT)(UNARY_EXPR)(ADD_OR_MINUS_EXPR)
            (LESS_OR_GREATER_EXPR)(EQUALITY_EXPR)(AND_EXPR)(OR_EXPR)(EXPR)
            (ENTITY_EXPR)
    )

    // Error handling: on error in EXPR, call error_handler.
    on_error<fail>(EXPR,
                   error_handler_function(error_handler)(
                   "Error while expression parsing! Expecting ", _4, _3));


    /*
     * This is grammar for Structure, Array, Function, Method and input expressions creating
     * =============================================
     *          Entity expression parsing
     * =============================================
     */

    INPUT_DEF =
                    (lit("input") >> '(' >> uint_)
                >   (".." >>  (uint_ > ')' > char_(':') > (VAR_BUILTIN_TYPE >> '[') > qi::int_ > ']')  // array def
                    | ')' >> -(':' > VAR_BUILTIN_TYPE))                                    [qi::_1]
            ;

    ARR_DEF_WITH_TYPE =
                ('{' >> -(EXPR % ',') > '}')                                               [qi::_1]
            ;

    ARR_DEF_WITH_REPEAT =
                ((lit("repeat") >> '(') > (EXPR | int_) > ',' > uint_ > ')')               [qi::_1]
            ;

    ARR_ENTITY_EXPR =
                ARR_DEF_WITH_TYPE
            |   INPUT_DEF
            |   ARR_DEF_WITH_REPEAT
            ;

    STRUCT_DEF =
                (lit("glue") >> '(' >> -(GLUE_ARG % ',') > ')')                              [qi::_1]
            ;

    GLUE_ARG =
                (EXPR >> -(':' > NAME))
            |   ((lit("bind") >> '(') > NAME > ',' > (EXPR % ',') > ')')                     [qi::_1]
            ;

    ENTITY_EXPR =
                ARR_ENTITY_EXPR
            |   STRUCT_DEF
            ;

    BOOST_SPIRIT_DEBUG_NODES(
            (ENTITY_EXPR)
            (ARR_DEF_WITH_TYPE)
            (INPUT_DEF)
            (ARR_DEF_WITH_REPEAT)
    )

    // Error handling: on error in EXPR, call error_handler.
    on_error<fail>(ENTITY_EXPR,
                   error_handler_function(error_handler)(
                           "Error while entity expression parsing! Expecting ", _4, _3));

    /*
     * =============================================
     *              Statement parsing
     * =============================================
     */

    OUTPUT_STMT =
                (lit("output") > '(' > int_(0) >> ',' > EXPR > ')') /* EXPR, not NAME? */  [qi::_1]
            ;

    LOOP_IF_BODY =
                ('{' > STMTS > '}')
            |   STMT
            ;

    RANGE =
                (int_ >> ':') > int_ > -(':' > int_)                                       [qi::_1]
            ;

    LOOP_HEADER =
                (lit("for") > '(' > NAME > lit("in") > (NAME | RANGE) > ')')               [qi::_1]
            |   (lit("while") > '(' > EXPR > ')')                                          [qi::_1]
            ;

    LOOP_STMT =
                LOOP_HEADER > LOOP_IF_BODY
            ;

    IF_STMT =
                lit("if") > '(' > EXPR > ')'
            >   LOOP_IF_BODY
            >   -(lit("else") > LOOP_IF_BODY)
            ;

    VAR_TYPE_WITH_BRACKETS =
                ((lit("int") >> '(') > qi::int_ > ')')       [qi::_1]     // int with size decl
            ;

    ARR_TYPE =
                (((VAR_BUILTIN_TYPE | STRUCT_TYPE /*| FUNC_TYPE*/) >> '[')
            > uint_ > ']')                                                                 [qi::_1]

            |   (lit("vector") > '<' > VAR_BUILTIN_TYPE > ',' > uint_ > '>')               [qi::_1]
            ;

    VAR_TYPE =
                VAR_TYPE_WITH_BRACKETS
            |   VAR_BUILTIN_TYPE >> !(char_('(') | '[')                        [qi::_1]
            |   ARR_TYPE
            |   STRUCT_TYPE
//            |   FUNC_TYPE
            ;

    LVAL_EXPR =
                (NAME >> ':' >> VAR_TYPE)                         [qi::_1]
            |   NAME                                              [qi::_1]
            ;

    STRUCT_TYPE =
                ('{' >> -(LVAL_EXPR % ',') > '}')                         [qi::_1]
            ;

    INIT_VAR =
                LVAL_EXPR                         // var decl with type
            >   -('=' > EXPR)                                                              [qi::_1]
            ;

    STMT =
                IF_STMT
            |   LOOP_STMT
            |   OUTPUT_STMT > ';'
            |   INIT_VAR > ';'
            |   EXPR > ';'
            ;

    STMTS = +STMT
            [qi::_1]
            ;

    BOOST_SPIRIT_DEBUG_NODES(
            (INIT_VAR)
            (VAR_TYPE)
            (VAR_TYPE_WITH_BRACKETS)
            (VAR_BUILTIN_TYPE)
            (ARR_TYPE)
            (NAME)(OUTPUT_STMT)
    )


    LAYER0 =
//                lit["layer"] >> '(' >> int_ >> char_(',') >> string >> ')' >> char_('{') >> STMTS >> char_('}')
                STMTS
            ;

    // Error handling: on error in STMTS, call error_handler.
    on_error<fail>(LAYER0,
                   error_handler_function(error_handler)(
                           "Error while statement parsing! Expecting ", _4, _3));
}

} // namespace parasl

#endif //PARASL_LAYER0_GRAMMAR_DEF_H
