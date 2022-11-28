#ifndef PARASL_PARSER_H
#define PARASL_PARSER_H

#include <string>

#include "layers_grammar.h"

namespace parasl {

using StrIter = std::string::iterator;

class Parser final {
public:
    Parser(StrIter begin, StrIter end) : parse_begin_(begin), parse_end_(end) {}

    bool Run();

private:
    StrIter parse_begin_;
    StrIter parse_end_;
};

}  // namespace parasl

#endif //PARASL_PARSER_H
