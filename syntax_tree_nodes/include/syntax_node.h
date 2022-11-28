//
// Created by denist on 11/14/22.
//

#include <iterator>

#include "types.h"

class SyntaxNode {
public:

    virtual size_t GetChildsNum() const = 0;

    virtual const SyntaxNode *GetChildAt(size_t idx) const = 0;

    const SyntaxNode *GetParent() const {
        return parent_;
    }

    void SetParent(SyntaxNode *parent) {
        parent_ = parent;
    }

    syntax_node_t GetSyntaxType() const {
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

    size_t GetChildsNum() const override {
        return opnds_.size();
    }

    const SyntaxNode *GetChildAt(size_t idx) const override {
        return opnds_.at(idx).get();
    }

protected:
    template<class InputIt>
    ChildSyntaxNode(InputIt begin, InputIt end) {
        std::copy(begin, end, opnds_.begin());

        for(auto &elt: opnds_) {
            elt->SetParent(this);
        }
    }

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
    // TODO: The same as in generate template
    template<class InputIt>
    ChildSyntaxNode(InputIt begin, InputIt end) : opnds_{begin, end} {}

    template<class ...Elt>
    ChildSyntaxNode(Elt... elt) : opnds_{elt...} {
        for(auto &opnd : opnds_) {
            opnd->SetParent(this);
        }
    }

private:
    std::vector<std::unique_ptr<SyntaxNode>> opnds_;
};

