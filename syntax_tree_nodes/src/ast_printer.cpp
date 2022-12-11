#include "ast_printer.h"
#include "statements.h"
#include "expressions.h"
#include <cassert>

namespace parasl::ast{
    namespace {

        std::ostream &operator<<(std::ostream& stream, operator_t op){
            switch (op) {
                case operator_t::ASSIGN: stream << '='; break;
                case operator_t::PLUS:   stream << '+'; break;
                case operator_t::MINUS:  stream << '-'; break;
                case operator_t::MULT:   stream << '*'; break;
                case operator_t::DIV:    stream << '\\'; break;
                case operator_t::AND:    stream << '+'; break;
                case operator_t::OR:     stream << "||"; break;
                case operator_t::LT:     stream << "<"; break;
                case operator_t::GT:     stream << ">"; break;
                case operator_t::LE:     stream << "<="; break;
                case operator_t::GE:     stream << ">="; break;
                case operator_t::NOT:    stream << "!"; break;
                case operator_t::DOT:    stream << "."; break;
                case operator_t::SQUARE_BR: stream << "[]"; break;
                case operator_t::PAREN:  stream << "()"; break;
                case operator_t::EQ: stream << "=="; break;
                case operator_t::NE:  stream << "!="; break;
            }
            return stream;
        }
        void expression_preamble(){
            std::cout << "EXPR(";
        }

        void expression_epilogue(expressions::Expression const* expr){
            std::cout << "): <type>=";
            if(expr->GetType())
                expr->GetType()->dump(std::cout);
            else
                std::cout << "<null>";
        }

        void statement_preamble(){
            std::cout << "STMT(";
        }

        void statement_epilogue(){
            std::cout << ")";
        }

    }


    void Printer::operator()(basic_syntax_nodes::SyntaxNode const*){

        std::cout << "<unknown node>";

    }

    void Printer::operator()(expressions::OperatorExpression const* node){
        expression_preamble();
        std::cout << "operator " << node->GetOperatorType();
        expression_epilogue(node);
    }

    void Printer::operator()(expressions::MemberAccess const* node){
        expression_preamble();
        std::cout << "Member access: ." << node->member();
        expression_epilogue(node);
    }

    void Printer::operator()(expressions::Identifier const* node){
        expression_preamble();
        std::cout << "ID: \"" << node->GetSymbolName() << "\"";
        expression_epilogue(node);
    }

    void Printer::operator()(expressions::Literal const* node){
        expression_preamble();
        std::cout << "literal: " << node->GetLiteralValue<unsigned int>();
        expression_epilogue(node);
    }

    void Printer::operator()(expressions::Reference const* node){
        expression_preamble();
        std::cout << "reference of: " << node->identifier()->GetSymbolName();
        expression_epilogue(node);
    }

    void Printer::operator()(expressions::Expression const* node){
        expression_preamble();
        std::cout << "unknown";
        expression_epilogue(node);
    }

    void Printer::operator()(statements::AssignmentStatement const*){
        statement_preamble();
        std::cout << "ASSIGNMENT";
        statement_epilogue();
    }

    void Printer::operator()(statements::CompoundStatement const*){
        statement_preamble();
        std::cout << "COMPOUND";
        statement_epilogue();
    }

    void Printer::operator()(statements::IfStatement const*){
        statement_preamble();
        std::cout << "IF";
        statement_epilogue();
    }

    void Printer::operator()(statements::DeclarationStatement const* node){
        statement_preamble();
        std::cout << "DECLARATION<id = " << node->identifier()->GetSymbolName()
                  << "; type = ";
        node->identifier()->GetType()->dump(std::cout);
        std::cout << ">";
        statement_epilogue();
    }

    void Printer::operator()(statements::Statement const*){
        statement_preamble();
        std::cout << "<unknown>";
        statement_epilogue();
    }

    void Printer::tabulate() {
        for(unsigned i = 0; i < n_tabs; ++i)
            std::cout << '\t';
    }

    void Printer::operator()(std::nullptr_t) {
        std::cout << "<null>" << std::endl;
    }

    void Printer::PreAction(basic_syntax_nodes::SyntaxNode const*node) {
        tabulate();
        visit_impl(node);
        std::cout << std::endl;
        n_tabs++;
    }

    void Printer::operator()(const statements::ForLoop *) {
        statement_preamble();
        std::cout << "FOR";
        statement_epilogue();
    }

    void Printer::operator()(const statements::WhileLoop *) {
        statement_preamble();
        std::cout << "WHILE";
        statement_epilogue();
    }

    void Printer::operator()(const statements::ForHeader *) {
        statement_preamble();
        std::cout << "FOR HEADER";
        statement_epilogue();
    }

    void Printer::operator()(const expressions::ArrayRange *node) {
        expression_preamble();
        std::cout << "Range over array";
        expression_epilogue(node);
    }

    void Printer::operator()(const expressions::IndexedRange *node) {
        expression_preamble();
        std::cout << "indexed range: from " << node->begin() << " to " << node->end() << " with step " << node->step();
        expression_epilogue(node);
    }
}