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

class Declaration : virtual public SyntaxNode {
    decl_type_t decl_type_;
    std::string name_;
};

class VarDecl : virtual public Declaration {
    VarType var_type_;
    size_t bit_lenght_;
};

class ArrayDecl : virtual public Declaration {
    ArrayType array_type_
};

class StructDecl : virtual public Declaration {
    StructType struct_type_;
};

class FuncDecl : virtual public Declaration {
    FuncType func_type_;
    std::vector<Statement> body_;
    /*
     * idea:
     * We could use not just vector of Statements, but create another
     * class Scope, which can also contains, for example, SymbolTable
     */
};

