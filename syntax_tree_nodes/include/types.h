//
// Created by denist on 11/13/22.
//

#pragma once

#include <cstddef>

enum class syntax_node_t {STMT, DECL, EXPR};

enum class stmt_type_t {UNARY_OP, BINARY_OP, ASSIGN_WITH_DECL, DECL, IF_STMT, WHILE_STMT, FOR_STMT, RET_STMT,
                        OUTPUT_STMT};
enum class decl_type_t {VAR_DECL, ARRAY_DECL, STRUCT_DECL, FUNC_DECL};
enum class expr_type_t {OPERATOR, LITERAL, SYMBOL, INPUT};
enum class type_type_t {VAR, ARRAY, STRUCT, FUNC};

enum class operator_t {ASSIGN, PLUS, MINUS, MULT, DIV, AND, OR, LT, GT, LE, GE, NOT, DOT, SQUARE_BR, PAREN};
enum class prim_type_t {CHAR, INT, DOUBLE, FLOAT};


class Type {
    type_type_t type_type_;
};

class VarType : public Type {
    prim_type_t var_type_;
};

class ArrayType : public Type {
    prim_type_t elt_type_;
    size_t size;
};

class FuncType : Type {

};

class StructType : Type {
    std::vector<std::unique_ptr<Type>> field_types_;
    std::vector<std::string> field_names_;
    std::vector<FuncType> methods_;
    std::vector<std::string> method_names_;
};
