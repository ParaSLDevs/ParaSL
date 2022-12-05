#ifndef PARASL_LAYER0_GRAMMAR_H
#define PARASL_LAYER0_GRAMMAR_H

#include "common.h"
#include "skipper.h"

namespace parasl {

template<typename Iterator, typename Skipper>
struct layer0_grammar : qi::grammar<Iterator, node_t(), Skipper> {
    layer0_grammar(error_handler<Iterator>& error_handler);

private:
    keywords_list keywords_t;
    prim_types_list VAR_BUILTIN_TYPES;

    qi::symbols<char, UnaryOp> UNARY_OP;
    qi::symbols<char, MultOp> MULT_OP;
    qi::symbols<char, AddOp> ADD_OP;
    qi::symbols<char, RelOp> RELATION_OP;
    qi::symbols<char, EqOp> EQUALITY_OP;

    //Common rules
    qi::rule<Iterator, std::string(), Skipper> NAME;
    qi::rule<Iterator, node_t(), Skipper> TYPE;

    // Expression rules
    qi::rule<Iterator, node_t(), Skipper>
            FUNC_CALL, TERM, DOT_EXPR, SQUARE_BRAKET_EXPR, UNARY_EXPR, MULT, ADD_OR_MINUS_EXPR,
            LESS_OR_GREATER_EXPR, EQUALITY_EXPR, AND_EXPR, OR_EXPR, EXPR, DECL_EXPR;

    // Entity expression rules
    qi::rule<Iterator, node_t(), Skipper>
            INPUT_DEF, ARR_DEF_WITH_TYPE, ARR_DEF_WITH_INPUT, ARR_DEF_WITH_REPEAT, ARR_ENTITY_EXPR,
            STRUCT_TYPE, FUNC_TYPE, FUNC_DEF, BIND_EXPR, GLUE_ARG, STRUCT_DEF, ENTITY_EXPR;

    // Statement rules
    qi::rule<Iterator, node_t(), Skipper>
            VAR_BUILTIN_TYPE, ARR_TYPE, OUTPUT_STMT, LOOP_STMT, LOOP_HEADER, LOOP_IF_BODY, IF_STMT, RANGE,
            VAR_TYPE_WITH_BRACKETS, VAR_TYPE, ASSIGNMENT, STMT, STMTS;

    // Layer0
    qi::rule<Iterator, node_t(), Skipper>
            LAYER0;
};

}  // namespace parasl

#endif //PARASL_LAYER0_GRAMMAR_H
