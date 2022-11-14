//
// Created by denist on 11/13/22.
//

#pragma once

#include <cstddef>

enum class stmt_type_t {UNARY_OP, BINARY_OP, ASSIGN_WITH_DECL, DECL, IF_STMT, WHILE_STMT, FOR_STMT, RET_STMT,
                        OUTPUT_STMT};
enum class decl_type_t {VAR_DECL, ARRAY_DECL, STRUCT_DECL, FUNC_DECL};
enum class operator_t {ASSIGN, PLUS, MINUS, MULT, DIV, AND, OR, LT, GT, LE, GE, NOT, DOT, SQUARE_BR, PAREN};
enum class entity_class_t {VAR, ARRAY, STRUCT, FUNC};
enum class prim_type_t {CHAR, INT, DOUBLE, FLOAT};

class ArrayType {
    prim_type_t type;
    size_t size;
};


