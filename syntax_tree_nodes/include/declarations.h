#pragma once

#include <memory>
#include <vector>
#include <string>

#include "types.h"
#include "syntax_node.h"


class FuncDecl;
class Statement;

class Declaration : virtual public SyntaxNode {
public:
    std::string GetDeclName() const {
        return name_;
    }

    decl_type_t GetDeclType() const {
        return decl_type_;
    }

    const Type *GetType() const {
        return type_;
    }

protected:
    Declaration(decl_type_t decl_type, const std::string &name, const Type *type) : SyntaxNode(syntax_node_t::DECL),
    type_(type), decl_type_(decl_type), name_(name) {}

    const Type *type_;
    decl_type_t decl_type_;
    std::string name_;
};

class FuncDecl : public Declaration, private ChildSyntaxNode<> {
public:
    template <class RandIt,
                class = std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, 
                                        typename std::iterator_traits<RandIt>::iterator_category>::value>>
    FuncDecl(const std::string &func_name, const FuncType *func_type, RandIt begin, RandIt end) : 
    Declaration(decl_type_t::FUNC_DECL, func_name, func_type), ChildSyntaxNode<>(begin, end) {}

    template <class ...Elt>
    FuncDecl(const std::string &func_name, const FuncType *func_type, Elt... elt) : 
    Declaration(decl_type_t::FUNC_DECL, func_name, func_type), 
    ChildSyntaxNode<>(std::move(elt)...) {}

    const SyntaxNode *GetBodyStmtAt(size_t idx) const {
        return GetChildAt(idx);
    }

    size_t GetBodyStmtNum() const {
        return GetChildsNum();
    }
};

