#include <iomanip>

#include "parser.h"

namespace parasl {

bool Parser::Run() {
    Skipper<StrIter> skipper;
    error_handler<StrIter> error_handler(parse_begin_, parse_end_);
    layers_grammar<StrIter, Skipper<StrIter>> grammar(error_handler);
    ChildSyntaxNode<> *root;
    bool res = phrase_parse(parse_begin_, parse_end_, grammar, skipper, root);
    bool is_full_parsed = (parse_begin_ == parse_end_);
    if (!is_full_parsed) {
        std::cout << "Unparseable: "
                  << std::quoted(std::string(parse_begin_, parse_end_)) << std::endl;
    }
    return res && is_full_parsed;
}

}  // namespace parasl