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

class Declaration : virtual public SyntaxNode, ChildSyntaxNode<0> {
public:
    std::string GetDeclName() const {
        return name_;
    }

    decl_type_t GetDeclType() const {
        return decl_type_;
    }

protected:
    Declaration(decl_type_t decl_type, const std::string &name) : SyntaxNode(syntax_node_t::DECL),
    ChildSyntaxNode<0>(), decl_type_(decl_type), name_(name) {}

    decl_type_t decl_type_;
    std::string name_;
};

class VarDecl : virtual public Declaration {
public:
    VarDecl(const std::string &var_name, const VarType *var_type, size_t bit_length) :
    Declaration(decl_type_t::VAR_DECL, var_name), var_type_(var_type), bit_length_(bit_length) {}

    const VarType *GetVarType() const {
        return var_type_;
    }

    size_t GetBitLength() const {
        return bit_length_;
    }

protected:
    const VarType *var_type_;
    size_t bit_length_;
};

class ArrayDecl : virtual public Declaration {
public:
    ArrayDecl(const std::string &array_name, const ArrayType *array_type) :
            Declaration(decl_type_t::ARRAY_DECL, array_name), array_type_(array_type) {}

    [[nodiscard]] const ArrayType *GetArrayType() const {
        return array_type_;
    }

protected:
    const ArrayType *array_type_
};

class StructDecl : virtual public Declaration {
public:
    StructDecl(const std::string &struct_name, const StructType *struct_type) :
            Declaration(decl_type_t::STRUCT_DECL, struct_name), struct_type_(struct_type) {}

    [[nodiscard]] const StructType *GetStructType() const {
        return struct_type_;
    }

protected:
    const StructType *struct_type_;
};

class FuncDecl : virtual public Declaration {
public:
    FuncDecl(const std::string &func_name, const FuncType *func_type)

protected:
    const FuncType *func_type_;
    std::vector<Statement> body_;
};

