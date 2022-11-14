//
// Created by denist on 11/14/22.
//

#pragma once

#include <memory>
#include <vector>
#include <string>

#include "types.h"
#include "syntax_node.h"


class FuncDecl;
class Statement;

class Declaration : public SyntaxNode {
    std::unique_ptr<Type> type_;
    decl_type_t decl_type_;
    std::string name_;
};

class VarDecl : virtual public Declaration {
    size_t bit_lenght_;
};

class ArrayDecl : virtual public Declaration {
    ArrayType type;
};

class StructDecl : virtual public Declaration {
    std::vector<Declaration> fields_;
    std::vector<FuncDecl> methods_;
};

class FuncDecl : virtual public Declaration {
    std::vector<Declaration> args_;
    Declaration ret_type_;
    std::vector<Statement> body_;
    /*
     * idea:
     * We could use not just vector of Statements, but create another
     * class Scope, which can also contains, for example, SymbolTable
     */
};

