#ifndef PARASL_STMT_GRAMMAR_H
#define PARASL_STMT_GRAMMAR_H

#include <iostream>

#include "statements.h"
#include "skipper.h"
#include "common.h"
#include "expr_grammar.h"

namespace parasl {

template<typename Iterator, typename Skipper>
struct stmt_grammar : qi::grammar<Iterator, node_t(), Skipper> {
    stmt_grammar(error_handler<Iterator>& error_handler) :
                stmt_grammar::base_type{STMTS}, EXPR(error_handler) {
        using error_handler_function = function<parasl::error_handler<Iterator> >;

        VAR_BUILTIN_TYPE.add
                ("int")
                ("char")
                ("float")
                ("double")
                ;

        NAME =
                    !lexeme[keywords_t >> !(alnum | '_')]
                >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
                ;

        INPUT_DEF =
                    (lit("input") > '(' > int_(0) >> ')')                                      [qi::_1]
                ;

        OUTPUT_STMT =
                    (lit("output") > '(' > int_(0) >> ',' > EXPR > ')') /* EXPR, not NAME? */  [qi::_1]
                ;

        VAR_TYPE_WITH_BRACKETS =
                    (lit("int") >> '(') > qi::int_ > ')'          // int with size decl
                ;

        VAR_TYPE =
                    VAR_TYPE_WITH_BRACKETS
                |   VAR_BUILTIN_TYPE >> !(char_('(') | '[')
                ;

        INIT_VAR =
                    (NAME >> ':' >> !ARR_TYPE) > VAR_TYPE           // var decl with type
                >>  *('=' > (EXPR | INPUT_DEF))                                               [qi::_1]

                |   (NAME >> '=' >> !lexeme[keywords_t])            // var decl without type
                >   EXPR                                                                      [qi::_1]

                |   (NAME >> '=' >> INPUT_DEF)                      // var decl with input(0) : type
                >   char_(':') > VAR_BUILTIN_TYPE                                             [qi::_1]
                ;

        ARR_TYPE =
                    ((VAR_BUILTIN_TYPE >> '[') > qi::int_ > ']')                              [qi::_1]
                |   (lit("vector") > '<' > VAR_BUILTIN_TYPE > ',' > uint_ > '>')              [qi::_1]
//                |   STRUCT_TYPE[NUM]             STRUCT_TYPE ?
//                |   FUNC_TYPE                             ?
                ;

        ARR_DEF_WITH_TYPE =
                    ('{' >> ARR_ANY_DEF_SEQ > '}')
                ;

        ARR_DEF_WITH_INPUT =
                    (lit("input") >> '(' >> uint_ >> "..")                                     [qi::_1]   // uint_?
                >>  (uint_ > ')' > char_(':') > (VAR_BUILTIN_TYPE >> '[') > qi::int_ > ']')
                ;

        ARR_DEF_WITH_REPEAT =
                    (lit("repeat") > '(' > (NAME | int_) > ',' > uint_ > ')')   [qi::_1] // EXPR instead of name?
                ;
        
        ARR_ANY_DEF_SEQ = 
//                    -(FUNC_DEF % ',')            [qi::_1]                  arr of funcs?
//                |   -(STRUCT_DEF % ',')          [qi::_1]
//                |   -(ARR_DEF_WITH_TYPE % ',')   [qi::_1]                  arr of diff length in rows?
                   -(EXPR % ',')      [qi::_1]
                ;
        
        INIT_ARR =
                    ((NAME >> ':') > ARR_TYPE                                                  [qi::_1]
                >>  *('=' > ARR_DEF_WITH_TYPE))

                |   (NAME >> '=')                                                              [qi::_1]
                >>  (ARR_DEF_WITH_INPUT | ARR_DEF_WITH_REPEAT)
                ;

        STMT =
                    OUTPUT_STMT > ';'
                |   INIT_VAR > ';'
                |   INIT_ARR > ';'
                |   EXPR > ';'
                ;

        STMTS = +STMT
                [qi::_1]
                ;

        BOOST_SPIRIT_DEBUG_NODES(
                (INIT_VAR)(INIT_ARR)
                (VAR_TYPE)
                (VAR_TYPE_WITH_BRACKETS)
                (VAR_BUILTIN_TYPE)
                (ARR_ANY_DEF_SEQ)
                (ARR_DEF_WITH_TYPE)
                (ARR_DEF_WITH_INPUT)
                (ARR_DEF_WITH_REPEAT)
                (ARR_TYPE)
                (NAME)(INPUT_DEF)(OUTPUT_STMT)
        )

        // Error handling: on error in STMTS, call error_handler.
        on_error<fail>(STMTS,
                       error_handler_function(error_handler)(
                       "Error while statement parsing! Expecting ", _4, _3));
    }

private:
    keywords_table keywords_t;
    expression_grammar<Iterator, Skipper> EXPR;

    SymGroup VAR_BUILTIN_TYPE;

    qi::rule<Iterator, node_t(), Skipper>
            ARR_ANY_DEF_SEQ, ARR_TYPE, ARR_DEF_WITH_TYPE, ARR_DEF_WITH_INPUT, ARR_DEF_WITH_REPEAT,
            INPUT_DEF, OUTPUT_STMT, INIT_ARR, INIT_VAR, STMT, STMTS;
    qi::rule<Iterator, std::string(), Skipper> 
            NAME, VAR_TYPE_WITH_BRACKETS, VAR_TYPE;
};

}  // namespace parasl


#endif //PARASL_STMT_GRAMMAR_H
