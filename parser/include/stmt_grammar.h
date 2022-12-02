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

        VAR_TYPE_WITH_BRACKETS =
                    (lit("int") >> '(') > qi::int_ > ')'          // int with size decl
                |   (VAR_BUILTIN_TYPE >> '[') > qi::int_ > ']'    // arrays decl
                ;

        VAR_TYPE =
                    VAR_TYPE_WITH_BRACKETS
                |   VAR_BUILTIN_TYPE
                ;

        INIT_VAR =
                    (NAME >> ':') > VAR_TYPE  // var decl with type
                >>  *('=' > EXPR)             [qi::_1]
                |   (NAME >> '=')             // var decl without type
                >   EXPR                      [qi::_1]
                ;

        STMT =
                    INIT_VAR > char_(';')
                |   EXPR > char_(';')
                ;

        STMTS = +STMT
                [qi::_1]
                ;

        BOOST_SPIRIT_DEBUG_NODES(
                (INIT_VAR)
                (VAR_TYPE)
                (VAR_TYPE_WITH_BRACKETS)
                (VAR_BUILTIN_TYPE)
                (NAME)
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

    qi::rule<Iterator, node_t(), Skipper> INIT_VAR, STMT, STMTS;
    qi::rule<Iterator, std::string(), Skipper> NAME, VAR_TYPE_WITH_BRACKETS, VAR_TYPE;
};

}  // namespace parasl


#endif //PARASL_STMT_GRAMMAR_H
