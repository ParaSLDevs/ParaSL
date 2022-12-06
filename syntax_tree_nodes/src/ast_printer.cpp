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
    }
    void Printer::print_impl(const basic_syntax_nodes::SyntaxNode *node) {
        if(!node) {
            tabulate();
            std::cout << "<null>" << std::endl;
            return;
        }
        tabulate();
        auto node_type = node->GetNodeType();
        switch (node_type) {
            case syntax_node_t::EXPR: {
                auto* expr = dynamic_cast<const expressions::Expression*>(node);
                assert(expr && "expected expression");
                auto expr_type = expr->GetExprCategory();

                std::cout << "EXPR(";
                switch (expr_type) {
                    case expr_type_t::OPERATOR:{
                        auto* op = dynamic_cast<const expressions::OperatorExpression*>(expr);
                        assert(op && "expected operator expression");
                        std::cout << "operator " << op->GetOperatorType();
                        break;
                    }
                    case expr_type_t::SYMBOL: {
                        auto* op = dynamic_cast<const expressions::Identifier*>(expr);
                        assert(op && "expected identifier expression");
                        std::cout << "ID: \"" << op->GetSymbolName() << "\"";
                        break;
                    }
                    case expr_type_t::LITERAL:{
                        auto* literal = dynamic_cast<const expressions::Literal*>(expr);
                        assert(literal && "expected literal expression");
                        std::cout << "literal: " << literal->GetLiteralValue<unsigned int>();
                        break;
                    }
                    case expr_type_t::REFERENCE:{
                        auto* reference = dynamic_cast<const expressions::Reference*>(expr);
                        assert(reference && "expected reference expression");
                        std::cout << "reference of: " << reference->identifier()->GetSymbolName();
                        break;
                    }
                    case expr_type_t::MEMBER_ACCESS:{
                        auto* memberAccess = dynamic_cast<const expressions::MemberAccess*>(expr);
                        assert(memberAccess && "expected literal expression");
                        std::cout << "Member access: ." << memberAccess->member();
                        break;
                    }
                    default:
                        std::cout << "<unknown>";
                }

                std::cout << "): <type>=";
                if(expr->GetType())
                    expr->GetType()->dump(std::cout);
                else
                    std::cout << "<null>";
                break;
            }
            case syntax_node_t::STMT:{
                auto* stmt = dynamic_cast<const statements::Statement*>(node);
                assert(stmt && "expected statement");
                auto stmt_type = stmt->GetStmtType();

                std::cout << "STMT(";
                switch (stmt_type) {
                    case stmt_type_t::IF_STMT:{
                        std::cout << "IF";
                        break;
                    }
                    case stmt_type_t::ASSIGNMENT:{
                        std::cout << "ASSIGNMENT";
                        break;
                    }
                    case stmt_type_t::COMPOUND_STMT:{
                        std::cout << "COMPOUND";
                        break;
                    }
                    case stmt_type_t::DECL:{
                        auto* decl = dynamic_cast<const statements::DeclarationStatement*>(node);
                        std::cout << "DECLARATION<id = " << decl->identifier()->GetSymbolName()
                        << "; type = ";
                        decl->identifier()->GetType()->dump(std::cout);
                        std::cout << ">";
                        break;
                    }
                    default:
                        std::cout << "<unknown>";
                }
                std::cout << ")";
                break;
            }
            default:
                std::cout << "<unknown node>";
        }
        std::cout << std::endl;
        n_tabs++;
        for(unsigned i = 0; i < node->GetChildsNum(); i++){
            print_impl(node->GetChildAt(i));
        }
        n_tabs--;
    }

    void Printer::tabulate() {
        for(unsigned i = 0; i < n_tabs; ++i)
            std::cout << '\t';
    }
}