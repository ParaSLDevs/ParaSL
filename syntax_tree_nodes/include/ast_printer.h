#pragma once

#include "ast_visitor.h"

namespace parasl::ast{

    class Printer: public depth_first_visitor<Printer>, public ast_visitor<Printer>{
    public:

        void operator()(basic_syntax_nodes::SyntaxNode const* node);

        void operator()(expressions::OperatorExpression const* node);

        void operator()(expressions::MemberAccess const* node);

        void operator()(expressions::Identifier const* node);

        void operator()(expressions::Literal const* node);

        void operator()(expressions::Reference const* node);

        void operator()(expressions::Expression const* node);

        void operator()(statements::AssignmentStatement const* node);

        void operator()(statements::CompoundStatement const* node);

        void operator()(statements::IfStatement const* node);

        void operator()(statements::DeclarationStatement const* node);

        void operator()(statements::Statement const* node);

        void operator()(std::nullptr_t);

        void PreAction(basic_syntax_nodes::SyntaxNode const* node);

        void PostAction(basic_syntax_nodes::SyntaxNode const*){
            n_tabs--;
        }
    private:
        void tabulate();
        unsigned n_tabs = 0;
    };

}