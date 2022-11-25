#ifndef PARASL_EXPRESSION_H
#define PARASL_EXPRESSION_H

namespace parasl {

template<typename Iterator>
struct expression_grammar : qi::grammar<Iterator, DeclStmt(), Skipper<Iterator>> {
    expression_grammar() : expression_grammar::base_type{EXPR} {
        EXPR =
                EXPR >> '+'  >> EXPR
                | EXPR >> '-' >> EXPR
                | MULT
                ;
        
        MULT =
                MULT >> '*' >> MULT
                | MULT >> '\\' >> MULT
                | MULT1
                ;
        
        MULT1 =
                MULT1 >> '>' >> MULT1
                | MULT1 >> '<' >> MULT1
                | MULT1 >> ">=" >> MULT1
                | MULT1 >> "<=" >> MULT1
                | MULT2
                ;
        
        MULT2 =
                MULT2 >> "&&" >> MULT2
                | MULT3
                ;
        
        MULT3 =
                MULT3 >> "||" >> MULT3
                | TERM
                ;
    }
    
private:
    
};

}  // namespace parasl

#endif //PARASL_EXPRESSION_H
