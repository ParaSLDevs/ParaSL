//
// Created by denist on 11/14/22.
//

#include <iterator>
#include <limits>
#include <memory>

#include "types.h"

class SyntaxNode {
public:

    // TODO: remove parent field and remain only interface methods

    [[nodiscard]] virtual size_t GetChildsNum() const = 0;

    [[nodiscard]] virtual const SyntaxNode *GetChildAt(size_t idx) const = 0;

    [[nodiscard]] const SyntaxNode *GetParent() const {
        return parent_;
    }

    void SetParent(SyntaxNode *parent) {
        parent_ = parent;
    }

    [[nodiscard]] syntax_node_t GetSyntaxType() const {
        return syntax_node_type_;
    }

    virtual ~SyntaxNode() = default;
protected:
    SyntaxNode(syntax_node_t syntax_type = syntax_node_t::NONE, SyntaxNode *parent = nullptr) : parent_(parent), syntax_node_type_(syntax_type) {}

    SyntaxNode *parent_;
    syntax_node_t syntax_node_type_;
};

template <size_t num_opnds = std::numeric_limits<size_t>::max()>
class ChildSyntaxNode : virtual public SyntaxNode {
public:

    [[nodiscard]] size_t GetChildsNum() const override {
        return opnds_.size();
    }

    [[nodiscard]] const SyntaxNode *GetChildAt(size_t idx) const override {
        return opnds_.at(idx).get();
    }

protected:

    template<class ...Elt>
    ChildSyntaxNode(Elt... elt) : opnds_{std::move(elt)...} {
        for(auto &opnd : opnds_) {
            opnd->SetParent(this);
        }
    }

private:
    std::array<std::unique_ptr<SyntaxNode>, num_opnds> opnds_;
};

template <>
class ChildSyntaxNode<std::numeric_limits<size_t>::max()> : virtual public SyntaxNode {
public:
    size_t GetChildsNum() const override {
        return opnds_.size();
    }

    const SyntaxNode *GetChildAt(size_t idx) const override {
        return opnds_.at(idx).get();
    }

protected:

    template<class ...Elt>
    ChildSyntaxNode(Elt... elt) : opnds_{elt...} {
        for(auto &opnd : opnds_) {
            opnd->SetParent(this);
        }
    }

private:
    std::vector<std::unique_ptr<SyntaxNode>> opnds_;
};

template <>
class ChildSyntaxNode<0> : virtual public SyntaxNode {
public:
    [[nodiscard]] size_t GetChildsNum() const override {
        return 0;
    }

    [[nodiscard]] const SyntaxNode *GetChildAt(size_t) const override {
        return nullptr;
    }

protected:
    ChildSyntaxNode() = default;
};

