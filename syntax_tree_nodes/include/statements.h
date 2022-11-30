#pragma once

#include <iostream>

#include "syntax_node.h"
#include "declarations.h"


class Statement : virtual public SyntaxNode {
public:
    stmt_type_t GetStmtType() const {
        return stmt_type_;
    }

protected:
    Statement(stmt_type_t stmt_type) : SyntaxNode(syntax_node_t::STMT), stmt_type_(stmt_type) {}

    stmt_type_t stmt_type_;
};

class UnaryOperatorStmt : public Statement, UnaryOperatorExpr {
public:
    UnaryOperatorStmt(std::unique_ptr<Expression> opnd, bool is_postfix, operator_t op_type) : 
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::UNARY_OP), 
    UnaryOperatorExpr(std::move(opnd), is_postfix, op_type) {}    
};

class BinaryOperatorStmt : public Statement, BinaryOperatorExpr {
public:
    BinaryOperatorStmt(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, operator_t op_type) :
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::BINARY_OP), 
    BinaryOperatorExpr(std::move(left), std::move(right), op_type) {}    
};

class VarDeclStmt : public Statement, VarDecl {
public:
    VarDeclStmt(const std::string &var_name, const VarType *var_type, size_t bit_length) : 
    SyntaxNode(syntax_node_t::STMT), 
    Statement(stmt_type_t::DECL), VarDecl(var_name, var_type, bit_length) {}    
};

class ArrayDeclStmt : public Statement, ArrayDecl {
public:
    ArrayDeclStmt(const std::string &array_name, const ArrayType *array_type) :
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::DECL), 
    ArrayDecl(array_name, array_type) {}    
};

class StructDeclStmt : public Statement, StructDecl {
public:
    StructDeclStmt(const std::string &struct_name, const StructType *struct_type) :
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::DECL),
    StructDecl(struct_name, struct_type) {}    
};

class FuncDeclStmt : public  Statement, FuncDecl {
public:
   template <class RandIt,
                class = std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, 
                                        typename std::iterator_traits<RandIt>::iterator_category>::value>>
    FuncDeclStmt(const std::string &func_name, const FuncType *func_type, RandIt begin, RandIt end) : 
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::DECL), FuncDecl(func_name, func_type, begin, end) {}

    template <class ...Elt>
    FuncDeclStmt(const std::string &func_name, const FuncType *func_type, Elt... elt) : 
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::DECL), FuncDecl(func_name, func_type, elt...) {}    
};

class ConditionCompoundStmt : public Statement, protected ChildSyntaxNode<> {
public:
    template<class RandIt,
    class = std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, 
                            typename std::iterator_traits<RandIt>::iterator_category>::value>>
    ConditionCompoundStmt(std::unique_ptr<Expression> cond, RandIt begin, RandIt end) : 
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::CONDCOMP_STMT),
    ChildSyntaxNode<>(begin, end) {
        AddChild(std::move(cond));
    }   

    template<class ...Node>
    ConditionCompoundStmt(std::unique_ptr<Expression> cond, Node... node) : 
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::CONDCOMP_STMT), 
    ChildSyntaxNode<>(std::move(node)...) {
        AddChild(std::move(cond));
    }

    const SyntaxNode *GetCondition() const {
        return GetChildAt(GetChildsNum()-1);
    }

    const SyntaxNode *GetBodyStmtAt(size_t idx) const {
        if(idx == GetChildsNum() - 1) {
            std::cerr << "Trying to get condition with GetBodyStmtAt method\n";
        } else {
            return GetChildAt(idx);
        }
    }
};

class ForLoop : public Statement, protected ChildSyntaxNode<> {
public:
    template<class RandIt,
    class = std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, 
                            typename std::iterator_traits<RandIt>::iterator_category>::value>>
    ForLoop(std::unique_ptr<Statement> init, 
    std::unique_ptr<Expression> cond, std::unique_ptr<Expression> after, RandIt begin, RandIt end) : 
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::FOR_STMT),
    ChildSyntaxNode<>(begin, end) {
        AddChild(std::move(init));
        AddChild(std::move(cond));
        AddChild(std::move(after));
    }

    template<class ...Node>
    ForLoop(std::unique_ptr<Statement> init,
    std::unique_ptr<Expression> cond, std::unique_ptr<Statement> after, Node... node) : 
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::FOR_STMT), 
    ChildSyntaxNode<>(std::move(node)...) {
        AddChild(std::move(init));
        AddChild(std::move(cond));
        AddChild(std::move(after));
    }

    const SyntaxNode *GetInit() const {
        return GetChildAt(GetChildsNum() - 3);
    }

    const SyntaxNode *GetCond() const {
        return GetChildAt(GetChildsNum() - 2);
    }

    const SyntaxNode *GetAfterExpr() const {
        return GetChildAt(GetChildsNum() - 1);
    }

    const SyntaxNode *GetBodyStmtAt(size_t idx) {
        if(idx >= GetChildsNum() -3 && idx < GetChildsNum()) {
            std::cerr << "Trying to get init or condition or after stmt with GetBodyStmtAt method\n";
        } else {
            return GetChildAt(idx);
        }
    }
};

class RetStmt : public Statement, ChildSyntaxNode<1> {
public:
    RetStmt(std::unique_ptr<Expression> opnd) : 
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::RET_STMT),
    ChildSyntaxNode<1>(std::move(opnd)) {}

};

class OutputStmt : public Statement, ChildSyntaxNode<1> {
public:
    OutputStmt(std::unique_ptr<Expression> opnd) :
    SyntaxNode(syntax_node_t::STMT), Statement(stmt_type_t::OUTPUT_STMT),
    ChildSyntaxNode<1>(std::move(opnd)) {}    

};

