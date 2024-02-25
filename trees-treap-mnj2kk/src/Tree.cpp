#include "tree/Tree.hpp"

namespace detail {
Node::Node() : value(), priority(), left(), right() {}

Node::Node(int value, int priority) : value(value), priority(priority), left(), right() {}

Node::~Node() {
    delete left;
    delete right;
}
}  // namespace detail

Treap::Treap() : _size(), _root() {}

thread_local std::mt19937 Treap::_random = std::mt19937(std::random_device{}());

bool Treap::contains(const Treap::Node *const node, int value) const noexcept {
    if (node == nullptr) {
        return false;
    }
    if (value == node->value) {
        return true;
    }
    if (value < node->value) {
        return contains(node->left, value);
    } else {
        return contains(node->right, value);
    }
}

bool Treap::contains(int value) const noexcept {
    return contains(_root.left, value);
}

std::size_t Treap::size() const noexcept {
    return _size;
}

bool Treap::empty() const noexcept {
    return _size == 0;
}

void Treap::values(const Treap::Node *const node, std::vector<int> &result) const noexcept {
    if (node == nullptr) {
        return;
    }
    values(node->left, result);
    result.emplace_back(node->value);
    values(node->right, result);
}

std::vector<int> Treap::values() const noexcept {
    std::vector<int> result;
    result.reserve(_size);
    values(_root.left, result);
    return result;
}

bool Treap::insert(int value) {
    if (contains(value)) {
        return false;
    }
    const std::pair<Node *, Node *> split_treap = split(_root.left, value);
    try {
        _root.left = new Node(value, _random());
    } catch (...) {
        _root.left = merge(split_treap.first, split_treap.second);
        return false;
    }
    _root.left = merge(split_treap.first, _root.left);
    _root.left = merge(_root.left, split_treap.second);
    ++_size;
    return true;
}

Treap::Node *Treap::remove(Treap::Node *const node, int value) {
    if (node->value == value) {
        Node *const result = merge(node->left, node->right);
        node->left         = nullptr;
        node->right        = nullptr;
        delete node;
        return result;
    }
    if (value < node->value) {
        node->left = remove(node->left, value);
    } else {
        node->right = remove(node->right, value);
    }
    return node;
}

bool Treap::remove(int value) {
    if (!contains(value)) {
        return false;
    }
    _root.left = remove(_root.left, value);
    --_size;
    return true;
}

std::pair<Treap::Node *, Treap::Node *> Treap::split(Treap::Node *const node, int value) {
    if (node == nullptr) {
        return {nullptr, nullptr};
    }
    if (node->value < value) {
        std::pair<Treap::Node *, Treap::Node *> result = split(node->right, value);
        node->right                                    = result.first;
        return {node, result.second};
    } else {
        std::pair<Treap::Node *, Treap::Node *> result = split(node->left, value);
        node->left                                     = result.second;
        return {result.first, node};
    }
}

Treap::Node *Treap::merge(Treap::Node *const left, Treap::Node *const right) {
    if (right == nullptr) {
        return left;
    }
    if (left == nullptr) {
        return right;
    }
    if (right->priority < left->priority) {
        left->right = merge(left->right, right);
        return left;
    } else {
        right->left = merge(left, right->left);
        return right;
    }
}
