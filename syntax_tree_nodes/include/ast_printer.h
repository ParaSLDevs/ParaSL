#pragma once

#include "syntax_node.h"

namespace parasl::ast{

    class Printer{
    public:

        void print(basic_syntax_nodes::SyntaxNode const* node){
            n_tabs = 0;
            print_impl(node);
        }

    private:

        void tabulate();
        void print_impl(basic_syntax_nodes::SyntaxNode const* node);
        unsigned n_tabs = 0;
    };

}