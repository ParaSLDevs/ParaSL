//
// Created by denist on 11/13/22.
//

#pragma once

#include <cstddef>

enum class syntax_node_t {NONE, STMT, DECL, EXPR, OPND};

enum class stmt_type_t {UNARY_OP, BINARY_OP, ASSIGN_WITH_DECL, DECL, IF_STMT, WHILE_STMT, FOR_STMT, RET_STMT,
                        OUTPUT_STMT};
enum class decl_type_t {VAR_DECL, ARRAY_DECL, STRUCT_DECL, FUNC_DECL};
enum class expr_type_t {OPERATOR, LITERAL, SYMBOL, INPUT};
enum class entity_type_t {VAR, ARRAY, STRUCT, FUNC};

enum class operator_t {ASSIGN, PLUS, MINUS, MULT, DIV, AND, OR, LT, GT, LE, GE, NOT, DOT, SQUARE_BR, PAREN};
enum class prim_type_t {CHAR, INT, DOUBLE, FLOAT};

class Type {
public:
    entity_type_t GetEntityType() {
        return entity_type_;
    }

protected:
    explicit Type(entity_type_t type) : entity_type_(type) {}

    entity_type_t entity_type_;
};

class VarType : public Type {
public:
    explicit VarType(prim_type_t prim_type, size_t bit_lenght) : Type(entity_type_t::VAR), var_type_(prim_type) {}

    [[nodiscard]] prim_type_t GetVarType() const {
        return var_type_;
    }

protected:
    /*
     * idea: may be we don't need prim_type_t field
     */
    prim_type_t var_type_;

};

class ArrayType : public Type {
public:
    ArrayType(const Type *elt_type, size_t size) : Type(entity_type_t::ARRAY),
    elt_type_(elt_type), size_(size) {}

    [[nodiscard]] size_t GetSize() const {
        return size_;
    }

    [[nodiscard]] const Type *GetEltType() const {
        return elt_type_;
    }

protected:
    const Type *elt_type_;
    size_t size_;
};

class FuncType : Type {
public:
    template <class ...ArgTypes>
    FuncType(const Type *ret_type, const ArgTypes * ...arg_type) :
    Type(entity_type_t::FUNC), ret_type_(ret_type), arg_types_{arg_type...} {}

    const Type *GetArgAt(size_t idx) {
        return arg_types_.at(idx);
    }

    const Type *GetRetType() {
        return ret_type_;
    }

protected:
    const Type *ret_type_;
    std::vector<const Type *> arg_types_;
};

class StructType : Type {
public:
    template <typename ...FieldType>
    StructType(std::pair<std::string, const FieldType *> ...field) : Type(entity_type_t::STRUCT),
    field_types_{field.first...}, field_names_{field.second...} {}

protected:
    std::vector<const Type *> field_types_;
    std::vector<std::string> field_names_;
};
