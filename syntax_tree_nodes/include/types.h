#pragma once

#include <cstddef>

enum class syntax_node_t {STMT, EXPR};

enum class stmt_type_t {ASSIGNMENT, FOR_STMT, RET_STMT,
                        OUTPUT_STMT, COMPOUND_STMT, IF_STMT, DECL};
enum class decl_type_t {VAR_DECL, ARRAY_DECL, STRUCT_DECL, FUNC_DECL};
enum class expr_type_t {OPERATOR, LITERAL, SYMBOL, INPUT, MEMBER_ACCESS, REFERENCE, INIT_LIST, REPEAT, GLUE, BIND};
enum class entity_type_t {VAR, ARRAY, VECTOR, STRUCT, FUNC};

enum class operator_t {ASSIGN, PLUS, MINUS, MULT, DIV, AND, OR, LT, GT, LE, GE, NOT, DOT, SQUARE_BR, PAREN, EQ, NE};
enum class prim_type_t {CHAR, INT, DOUBLE, FLOAT};

inline std::ostream& operator<<(std::ostream& stream, prim_type_t type){
    switch (type) {
        case prim_type_t::CHAR:   stream << "char"; break;
        case prim_type_t::INT:    stream << "int"; break;
        case prim_type_t::DOUBLE: stream << "double"; break;
        case prim_type_t::FLOAT:  stream << "float"; break;
    }
    return stream;
}
namespace types {
    class Type {
    public:
        entity_type_t GetEntityType() const{
            return entity_type_;
        }

        virtual void dump(std::ostream& ostream) const = 0;
        virtual ~Type() = default;

    protected:
        explicit Type(entity_type_t type) : entity_type_(type) {}

        entity_type_t entity_type_;
    };

    class VarType : public Type {
    public:
        explicit VarType(prim_type_t prim_type, size_t bit_length) : Type(entity_type_t::VAR), 
        prim_type_(prim_type), bit_length_(bit_length) {}

        void dump(std::ostream& ostream) const override{
            ostream << prim_type_;
            if(prim_type_ == prim_type_t::INT)
                ostream << "(" << bit_length_ << ")";
        }

        prim_type_t primType() const{
            return prim_type_;
        }

        size_t bitlength() const{
            return bit_length_;
        }
    protected:
        /*
        * idea: may be we don't need prim_type_t field
        */
        prim_type_t prim_type_;
        size_t bit_length_;

    };

    // TODO: combine array and vector types under same interface (compound type or smth)

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

        void dump(std::ostream& ostream) const override{
            elt_type_->dump(ostream);
            ostream << "[" << size_ << "]";
        }

    protected:
        const Type *elt_type_;
        size_t size_;
    };

    class VectorType: public Type{
    public:
        VectorType(const Type *elt_type, size_t size) : Type(entity_type_t::VECTOR),
                                                       elt_type_(elt_type), size_(size) {}

        [[nodiscard]] size_t GetSize() const {
            return size_;
        }

        [[nodiscard]] const Type *GetEltType() const {
            return elt_type_;
        }

        void dump(std::ostream& ostream) const override{
            elt_type_->dump(ostream);
            ostream << "[" << size_ << "]";
        }

    protected:
        const Type *elt_type_;
        size_t size_;
    };

    class FuncType : public Type {
    public:
        template <class ...ArgTypes>
        FuncType(const Type *ret_type, ArgTypes&& ...arg_type) :
        Type(entity_type_t::FUNC), ret_type_(ret_type), arg_list_{std::forward<ArgTypes>(arg_type)...} {}

        const Type *GetArgAt(size_t idx) {
            return arg_list_.at(idx).second;
        }

        const Type *GetRetType() {
            return ret_type_;
        }

        using ArgContainer = std::vector<std::pair<std::string, const Type *>>;

        ArgContainer::iterator begin(){
            return arg_list_.begin();
        }

        ArgContainer::iterator end(){
            return arg_list_.end();
        }

        ArgContainer::const_iterator begin() const{
            return arg_list_.begin();
        }

        ArgContainer::const_iterator end() const{
            return arg_list_.end();
        }

        void dump(std::ostream& ostream) const override{
            // TODO
            ostream << "<func type>";
        }
    protected:
        const Type *ret_type_;
        ArgContainer arg_list_;
    };

    class StructType : public Type {
    public:
        template <typename ...FieldType>
        StructType(FieldType&& ...field) : Type(entity_type_t::STRUCT),
        fields_{std::forward<FieldType>(field)...} {}

        template <typename Iter>
        StructType(Iter begin , Iter end) :
        Type(entity_type_t::STRUCT){
            fields_.resize(end - begin);
            std::copy(begin, end, fields_.begin());
        }

        using FieldContainer = std::vector<std::pair<std::string, const Type *>>;

        FieldContainer::iterator begin(){
            return fields_.begin();
        }

        FieldContainer::iterator end(){
            return fields_.end();
        }

        FieldContainer::const_iterator begin() const{
            return fields_.begin();
        }

        FieldContainer::const_iterator end() const{
            return fields_.end();
        }

        bool operator==(StructType const& another) const{
            return std::mismatch(fields_.begin(), fields_.end(), another.begin()).first != fields_.end();
        }

        void dump(std::ostream& ostream) const override{
            ostream << "{ ";
            for(auto& member: fields_) {
                ostream << member.first << " : ";
                member.second->dump(ostream);
                ostream << ", ";
            }
            ostream << " }";
        }
    protected:
        FieldContainer fields_;
    };
}
