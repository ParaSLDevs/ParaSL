#include <iomanip>

#include "parser.h"
#include "ast_printer.h"

namespace parasl {

    namespace ASTBuilder{
        ast::Builder* builderCtx = nullptr;
    }
bool Parser::Run() {
    ast::Builder builder;
    ASTBuilder::builderCtx = &builder;
    Skipper<StrIter> skipper;
    error_handler<StrIter> error_handler(parse_begin_, parse_end_);
    layers_grammar<StrIter, Skipper<StrIter>> grammar(error_handler);
    basic_syntax_nodes::Ref<basic_syntax_nodes::SyntaxNode> root;
    bool res = phrase_parse(parse_begin_, parse_end_, grammar, skipper, root);
    bool is_full_parsed = (parse_begin_ == parse_end_);
    if (!is_full_parsed) {
#if 0
        std::cout << "Unparseable: "
                  << std::quoted(std::string(parse_begin_, parse_end_)) << std::endl;
#endif
    } else{
        auto printer = ast::Printer();

        printer.print(root.get());
    }


    return res && is_full_parsed;
}

}  // namespace parasl