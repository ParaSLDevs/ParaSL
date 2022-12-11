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

    MEMBER_ACCESS = lit(".") >> NAME;

    SUBSCRIPT = lit("[") >> EXPR >> ']';

    SQUARE_BRAKET_EXPR =
                NAME                         [qi::_1]
            >>  '[' > EXPR > ']'
            ;
    ID = NAME [ASTBuilder::Identifier()];
    SUBTERM = (ID
            >> *(SUBSCRIPT | MEMBER_ACCESS)) [ASTBuilder::Subterm()];

    TERM =
                qi::uint_                                  [ASTBuilder::IntegralLiteral()]
            //|   FUNC_CALL                                  [ASTBuilder::Pass()]
            |   SUBTERM                                       [ASTBuilder::Pass()]
            |   ('(' > EXPR > ')')                         [ASTBuilder::Pass()]
            ;

    UNARY_EXPR =
                TERM                                [ASTBuilder::Pass()]
            |   (UNARY_OP > TERM)                   [ASTBuilder::TerminalWithUnaryOp()]
            ;

    MULT =
            (UNARY_EXPR
            >>  *(MULT_OP > UNARY_EXPR))             [ASTBuilder::BinaryOp<MultOp>()]
            ;

    ADD_OR_MINUS_EXPR =
            (MULT
            >>  *(ADD_OP > MULT))               [ASTBuilder::BinaryOp<AddOp>()]
            ;

    LESS_OR_GREATER_EXPR =
            (ADD_OR_MINUS_EXPR
            >>  *(RELATION_OP > ADD_OR_MINUS_EXPR))            [ASTBuilder::BinaryOp<RelOp>()]
            ;

    EQUALITY_EXPR =
            (LESS_OR_GREATER_EXPR
            >>  *(EQUALITY_OP > LESS_OR_GREATER_EXPR))           [ASTBuilder::BinaryOp<EqOp>()]
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
            (TERM)(SUBTERM)(SQUARE_BRAKET_EXPR)(MULT)(UNARY_EXPR)(ADD_OR_MINUS_EXPR)
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
                ('{' >> (EXPR % ',') > '}')                                               [ASTBuilder::InitializerList()]
            ;

    ARR_DEF_WITH_REPEAT =
                ((lit("repeat") >> '(') > (EXPR) > ',' > uint_ > ')')               [ASTBuilder::RepeatExpression()]
            ;

    ARR_ENTITY_EXPR =
                ARR_DEF_WITH_TYPE                               [ASTBuilder::Pass()]
            |   INPUT_DEF                                       [ASTBuilder::Pass()]
            |   ARR_DEF_WITH_REPEAT                             [ASTBuilder::Pass()]
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
                ('{' >> STMTS > '}')    [ASTBuilder::Pass()]
            |   BIND_EXPR               [ASTBuilder::Pass()]
            //|   EXPR                    [ASTBuilder::Pass()]
            ;

    ENTITY_EXPR =
                ARR_ENTITY_EXPR     [ASTBuilder::Pass()]
            |   STRUCT_DEF          [ASTBuilder::Pass()]
            |   FUNC_DEF            [ASTBuilder::Pass()]
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

    LOOP_IF_BODY =  STMT            [ASTBuilder::CompoundStatement()]
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
                ((PRIMITIVE_TYPE | STRUCT_TYPE | VECTOR_TYPE) >> *('[' > uint_ > ']'))      [ASTBuilder::ArrayType()]
            ;

    VECTOR_TYPE = (lit("vector") > '<' > VAR_BUILTIN_TYPE > ',' > uint_ > '>')              [ASTBuilder::VectorType()];

    VAR_TYPE =
                ARR_TYPE
            |   VECTOR_TYPE
            |   PRIMITIVE_TYPE
            |   STRUCT_TYPE
           // |   FUNC_TYPE
            ;

    PRIMITIVE_TYPE = VAR_TYPE_WITH_BRACKETS
                    | VAR_BUILTIN_TYPE;

    VAR_BUILTIN_TYPE = VAR_BUILTIN_TYPES [ASTBuilder::BuiltInType()];

    DECL_EXPR =
                (NAME >> -(':' >> VAR_TYPE) >> -('=' > (ARR_DEF_WITH_REPEAT | ARR_DEF_WITH_TYPE | ASSIGNMENT_SEQ | EXPR)) >> ';' )   [ASTBuilder::Declaration()]
            ;

    STRUCT_TYPE =
                ('{' >> -((NAME >> ":" >> -VAR_TYPE) % ',') > '}')                         [ASTBuilder::StructType()]
            ;
/*
    FUNC_TYPE =
                '(' >>
                    (-(DECL_EXPR % ','))
                > ')' > ':' > VAR_TYPE
            ;
*/
    ASSIGNMENT_SEQ =
            (EXPR >> '=' >> (ASSIGNMENT_SEQ | EXPR)) [ASTBuilder::Assignment()];

    ASSIGNMENT = ASSIGNMENT_SEQ [ASTBuilder::AssignmentStatement()]
            ;

    STMT =
                IF_STMT             [ASTBuilder::Pass()]
            |   LOOP_STMT           [ASTBuilder::Pass()]
            |   (OUTPUT_STMT > ';') [ASTBuilder::Pass()]
            |   DECL_EXPR           [ASTBuilder::Pass()]
            |   (ASSIGNMENT > ';')  [ASTBuilder::Pass()]
            |   SCOPE [ASTBuilder::Pass()]

            ;

    SCOPE = (lit('{')[ASTBuilder::ScopeOpen()] > STMTS >
             lit('}')[ASTBuilder::ScopeClose()]) [ASTBuilder::Pass()];

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
                STMTS   [ASTBuilder::Pass()]
            ;

    // Error handling: on error in STMTS, call error_handler.
    on_error<fail>(LAYER0,
                   error_handler_function(error_handler)(
                           "Error while statement parsing! Expecting ", _4, _3));
}

} // namespace parasl

#endif //PARASL_LAYER0_GRAMMAR_DEF_H
