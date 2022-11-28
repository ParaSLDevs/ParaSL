#include "parser.h"

namespace parasl {

bool Parser::Run() {
    Skipper<StrIter> skipper;
    layers_grammar<StrIter> grammar;
    bool res = phrase_parse(parse_begin_, parse_end_, grammar, skipper);
    bool is_full_parsed = (parse_begin_ == parse_end_);
    return res && is_full_parsed;
}

}  // namespace parasl