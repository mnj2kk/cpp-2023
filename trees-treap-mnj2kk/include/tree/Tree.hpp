#ifndef TREE_HPP
#define TREE_HPP

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

namespace detail {
struct Node {
    const int value;
    const int priority;
    Node* left;
    Node* right;

    Node();
    Node(int, int);
    ~Node();
};
}  // namespace detail

class Treap {
private:
    using Node = detail::Node;

    static thread_local std::mt19937 _random;
    std::size_t _size;
    Node _root;

    [[nodiscard]] bool contains(const Node* node, int value) const noexcept;
    void values(const Node* node, std::vector<int>& result) const noexcept;

    [[nodiscard]] Node* remove(Node* node, int value);

    [[nodiscard]] std::pair<Node*, Node*> split(Node* node, int value);
    [[nodiscard]] Node* merge(Node* left, Node* right);

public:
    [[nodiscard]] bool contains(int value) const noexcept;
    bool insert(int value);
    bool remove(int value);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] std::vector<int> values() const noexcept;

    Treap();
};

#endif
