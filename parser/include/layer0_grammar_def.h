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
            ("+", UnaryOp::PLUS)
            ("-", UnaryOp::MINUS)
            ("!", UnaryOp::INV)
            ;

    MULT_OP.add
            ("*", MultOp::MULT)
            ("/", MultOp::DIV)
            ;

    ADD_OP.add
            ("+", AddOp::ADD)
            ("-", AddOp::SUB)

            ;

    RELATION_OP.add
            ("<", RelOp::LT)
            ("<=", RelOp::LE)
            (">", RelOp::GT)
            (">=", RelOp::GE)
            ;

    EQUALITY_OP.add
            ("==", EqOp::EQ)
            ("!=", EqOp::NE)
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
                (NAME | DOT_EXPR)                          [qi::_1]
            >>  '[' > EXPR > ']'
            ;

    TERM =
                qi::uint_                                  [ASTBuilder::IntegralLiteral()]
            |   FUNC_CALL
            |   SQUARE_BRAKET_EXPR
            |   DOT_EXPR
            |   NAME                                       [ASTBuilder::StringLiteral()]
            |   ENTITY_EXPR                                       [qi::_1]
            |   '(' > EXPR > ')'
            ;

    UNARY_EXPR =
                TERM                                [ASTBuilder::Terminal()]
            |   (UNARY_OP > TERM)                   [ASTBuilder::TerminalWithUnaryOp()]
            ;

    MULT =
            (UNARY_EXPR
            >>  *(MULT_OP > UNARY_EXPR))             [ASTBuilder::Multiplication()]
            ;

    ADD_OR_MINUS_EXPR =
            (MULT
            >>  *(ADD_OP > MULT))               [ASTBuilder::Addition()]
            ;

    LESS_OR_GREATER_EXPR =
            (ADD_OR_MINUS_EXPR
            >>  *(RELATION_OP > ADD_OR_MINUS_EXPR))            [ASTBuilder::Relation()]
            ;

    EQUALITY_EXPR =
            (LESS_OR_GREATER_EXPR
            >>  *(EQUALITY_OP > LESS_OR_GREATER_EXPR))           [ASTBuilder::Equality()]
            ;

    AND_EXPR =
            (EQUALITY_EXPR
            >>  *("&&" > EQUALITY_EXPR))                       [ASTBuilder::LogicAnd()]
            ;

    OR_EXPR =
            (AND_EXPR
            >>  *("||" > AND_EXPR))                             [ASTBuilder::LogicOr()]
            ;

    EXPR =  OR_EXPR
            //>>  *("=" > OR_EXPR)
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

    BIND_EXPR =
                ((lit("bind") >> '(') > NAME > ',' > (EXPR % ',') > ')')                     [qi::_1]
            ;

    GLUE_ARG =
                (EXPR >> -(':' > NAME))
            |   BIND_EXPR
            ;

    FUNC_DEF =
                ('{' >> STMTS > '}')
            |   BIND_EXPR
//            |   EXPR
            ;

    ENTITY_EXPR =
                ARR_ENTITY_EXPR
            |   STRUCT_DEF
            |   FUNC_DEF
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
                (lit("output") > '(' > int_(0) >> ',' > EXPR > ')') /* EXPR, not NAME? */  [ASTBuilder::OutputStatement()]
            ;

    LOOP_IF_BODY =
                ('{' > STMTS > '}')
            |   STMT
            ;

    RANGE =
            ((int_ >> ':') > int_ > -(':' > int_))                                       [ASTBuilder::Range()]
            ;

    LOOP_HEADER =
                (lit("for") > '(' > NAME > lit("in") > (NAME | RANGE) > ')')               [ASTBuilder::ForStatement()]
            |   (lit("while") > '(' > EXPR > ')')                                          [ASTBuilder::WhileStatement()]
            ;

    LOOP_STMT =
                LOOP_HEADER > LOOP_IF_BODY
            ;

    IF_STMT =
            (lit("if") > '(' > EXPR > ')'
            >   LOOP_IF_BODY
            >   -(lit("else") > LOOP_IF_BODY))          [ASTBuilder::IfStatement()]
            ;

    VAR_TYPE_WITH_BRACKETS =
                ((lit("int") >> '(') > qi::int_ > ')')       [ASTBuilder::IntegralTypeWithBitwidth()]     // int with size decl
            ;

    ARR_TYPE =
                (((VAR_BUILTIN_TYPE | STRUCT_TYPE | FUNC_TYPE) >> '[')
            > uint_ > ']')                                                                 [ASTBuilder::ArrayType()]

            |   (lit("vector") > '<' > VAR_BUILTIN_TYPE > ',' > uint_ > '>')               [ASTBuilder::VectorType()]
            ;

    VAR_TYPE =
                VAR_TYPE_WITH_BRACKETS
            |   (VAR_BUILTIN_TYPE >> !(char_('(') | '[') )
            |   ARR_TYPE
            |   STRUCT_TYPE
            |   FUNC_TYPE
            ;

    VAR_BUILTIN_TYPE = VAR_BUILTIN_TYPES [ASTBuilder::BuiltInType()];

    DECL_EXPR =
                (NAME >> ':' >> VAR_TYPE)                         [ASTBuilder::Declaration()]
            ;

    STRUCT_TYPE =
                ('{' >> -(DECL_EXPR % ',') > '}')                         [ASTBuilder::StructType()]
            ;

    FUNC_TYPE =
                '(' >>
                    (-(NAME % ',')                                             [qi::_1]
                     -(DECL_EXPR % ','))
                > ')'
            ;

    ASSIGNMENT =
            ((DECL_EXPR | EXPR)                             // var decl with type
            >>  *('=' >> EXPR))                                            [ASTBuilder::Assignment()]
            ;

    STMT =
                IF_STMT
            |   LOOP_STMT
            |   OUTPUT_STMT > ';'
            |   ASSIGNMENT > ';'
            |   DECL_EXPR > ';'
            ;

    STMTS = (*STMT)                   [ASTBuilder::CompoundStatement()]
            ;

    BOOST_SPIRIT_DEBUG_NODES(
            (ASSIGNMENT)
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
