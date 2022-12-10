#pragma once

#include <iterator>
#include <limits>
#include <memory>
#include <algorithm>

#include "types.h"

namespace basic_syntax_nodes {
    class SyntaxNode {
    public:

        [[nodiscard]] virtual size_t GetChildsNum() const = 0;

        [[nodiscard]] virtual const SyntaxNode *GetChildAt(size_t idx) const = 0;

        [[nodiscard]] const SyntaxNode *GetParent() const {
            return parent_;
        }

        void SetParent(SyntaxNode *parent) {
            parent_ = parent;
        }

        [[nodiscard]] virtual syntax_node_t GetNodeType() const = 0;

        virtual ~SyntaxNode() = default;
    protected:
        SyntaxNode(SyntaxNode *parent = nullptr) : parent_(parent) {}

        SyntaxNode *parent_;
    };

    template<typename T>
    using Ref = std::unique_ptr<T>;

    class TypedSyntaxNode: public virtual SyntaxNode{
    public:
        TypedSyntaxNode(syntax_node_t syntax_type):
        syntax_node_type_(syntax_type){}

        [[nodiscard]] syntax_node_t GetNodeType() const {
            return syntax_node_type_;
        }
    private:
        syntax_node_t syntax_node_type_;
    };

    template <size_t nchild = std::numeric_limits<size_t>::max()>
    class ChildedSyntaxNode : virtual public SyntaxNode {
    public:

        [[nodiscard]] size_t GetChildsNum() const override {
            return children_.size();
        }

        [[nodiscard]] const SyntaxNode *GetChildAt(size_t idx) const override {
            return children_.at(idx).get();
        }



        ChildedSyntaxNode(ChildedSyntaxNode&& another) noexcept: children_(std::move(another.children_)){
            rebindParent();
        }

        ChildedSyntaxNode(ChildedSyntaxNode const& another) = delete;


        ChildedSyntaxNode& operator=(ChildedSyntaxNode&& another) = delete;

        ChildedSyntaxNode& operator=(ChildedSyntaxNode const& another) = delete;

    protected:

        template <class RandIt, 
        class = std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, 
                                            typename std::iterator_traits<RandIt>::iterator_category>::value>>
        ChildedSyntaxNode(RandIt begin, RandIt end) {
            std::copy(begin, end, children_.begin());
            rebindParent();
        }

        template<class ...Elt>
        ChildedSyntaxNode(Elt&&... elt) : children_{std::forward<Elt>(elt)...} {
            rebindParent();
        }

    private:

        void rebindParent(){
            for(auto &opnd : children_) {
                if(opnd)
                    opnd->SetParent(this);
            }
        }
        std::array<Ref<SyntaxNode>, nchild> children_;
    };

    template <>
    class ChildedSyntaxNode<std::numeric_limits<size_t>::max()> : virtual public SyntaxNode {
    public:
        size_t GetChildsNum() const override {
            return children_.size();
        }

        const SyntaxNode *GetChildAt(size_t idx) const override {
            return children_.at(idx).get();
        }

        ChildedSyntaxNode(ChildedSyntaxNode&& another) noexcept: children_(std::move(another.children_)){
            rebindParent();
        }

        ChildedSyntaxNode(ChildedSyntaxNode const& another) = delete;


        ChildedSyntaxNode& operator=(ChildedSyntaxNode&& another) = delete;

        ChildedSyntaxNode& operator=(ChildedSyntaxNode const& another) = delete;

    protected:
        void AddChild(std::unique_ptr<SyntaxNode> child) {
            children_.push_back(std::move(child));
        }

        template <class RandIt, 
        class = std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, 
                                            typename std::iterator_traits<RandIt>::iterator_category>::value>>
        ChildedSyntaxNode(RandIt begin, RandIt end) {
            std::transform(begin, end, std::back_inserter(children_),
                           [](auto& elem){
                return std::move(elem);
            });

            rebindParent();
        }

        template<class ...Elt>
        ChildedSyntaxNode(Elt&&... elt) : children_{std::forward<Elt>(elt)...} {
            rebindParent();
        }

    private:

        void rebindParent(){
            for(auto &opnd : children_) {
                if(opnd)
                    opnd->SetParent(this);
            }
        }

        std::vector<Ref<SyntaxNode>> children_;
    };

    template <>
    class ChildedSyntaxNode<0> : virtual public SyntaxNode {
    public:
        [[nodiscard]] size_t GetChildsNum() const override {
            return 0;
        }

        [[nodiscard]] const SyntaxNode *GetChildAt(size_t) const override {
            return nullptr;
        }

    protected:
        ChildedSyntaxNode() = default;
    };

    using LeafNode = ChildedSyntaxNode<0>;
}
