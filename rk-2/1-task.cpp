/*
 * Напишите функцию, проверяющую, что во всех узлах бинарного дерева поиска хранится одинаковое значение.
 * Значения - int.
 *
 * Формат ввода
 * По очереди вводятся узлы бинарного дерева в порядке вставки.
 *
 * Формат вывода
 * 0 - если все значения в узлах бинарного дерева разные.
 * 1 - если все значения в узлах бинарного дерева одинаковые.
 */

#include <sstream>
#include <iostream>
#include <functional>
#include <stack>
#include <utility>

// #define DEBUG


template <typename T, typename Compare = std::less<T>>
class SearchTree {
 public:
    SearchTree(): root_(nullptr), size_(0) {
    }

    ~SearchTree() {
        auto postorder_iter = GetPostorderIterator();
        for (std::size_t i = 0; i < Size(); ++i) {
            delete postorder_iter.ptr_;
            postorder_iter += 1;
        }
    }

    template <typename U>
    void Insert(U&& value) {
        ++size_;

        if (!root_) {
            root_ = new Node(std::forward<T>(value));
            return;
        }

        auto current_node_ptr = root_;
        while (true) {
            if (compare(value, current_node_ptr->value)) {
                if (!current_node_ptr->left) {
                    current_node_ptr->left = new Node(std::forward<T>(value));
                    return;
                }
                current_node_ptr = current_node_ptr->left;

            } else {
                if (!current_node_ptr->right) {
                    current_node_ptr->right = new Node(std::forward<T>(value));
                    return;
                }
                current_node_ptr = current_node_ptr->right;
            }
        }
    }

    const T& Top() const {
        return root_->value;
    }

    [[nodiscard]] std::size_t Size() const {
        return size_;
    }

 private:
    struct Node {
        T value;
        Node* left;
        Node* right;

        template <typename U>
        explicit Node(U&& value): value(std::forward<T>(value)), left(nullptr), right(nullptr) {
        }

        template <typename U>
        Node(U&& value, Node* left, Node* right): value(std::forward<T>(value)), left(left), right(right) {
        }
    };

    class postorder_iterator;

    Node* root_;
    std::size_t size_;

    Compare compare;

 public:
    postorder_iterator GetPostorderIterator() {
        return postorder_iterator(root_);
    }

    bool IsSameNodes() {
        auto value = Top();
        auto postorder_iter = GetPostorderIterator();
        for (std::size_t i = 0; i < Size(); ++i) {
            if (*postorder_iter != value) {
                return false;
            }
            postorder_iter += 1;
        }

        return true;
    }
};

template <typename T, typename Compare>
class SearchTree<T, Compare>::postorder_iterator {
    friend SearchTree::~SearchTree();

 public:
    explicit postorder_iterator(Node* root): ptr_(root) {
        if (root) {
            stack_.push({root, false});
            *this += 1;
        }
    }

    T& operator * () {
        return ptr_->value;
    }

    postorder_iterator& operator += (std::size_t steps) {
        for (std::size_t i = 0; i < steps && !stack_.empty(); ++i) {
            while (!stack_.top().second) {
                stack_.top().second = true;

                auto node_ptr = stack_.top().first;
                if (node_ptr->right) {
                    stack_.push({node_ptr->right, false});
                }
                if (node_ptr->left) {
                    stack_.push({node_ptr->left, false});
                }
            }

            ptr_ = stack_.top().first;
            stack_.pop();
        }

        return *this;
    }

 private:
    Node* ptr_;
    std::stack<std::pair<Node*, bool>> stack_;
};


void Run(std::istream& input, std::ostream& output) {
    SearchTree<int> search_tree;

    int value;
    while (input >> value) {
        search_tree.Insert(value);
    }

    output << search_tree.IsSameNodes() << std::endl;
}


#ifdef DEBUG

#include <cassert>

void TestRun() {
    {
        std::stringstream input, output;

        input << "2 3 1\n";

        Run(input, output);
        assert(output.str() == "0\n");
    }
    {
        std::stringstream input, output;

        input << "1 1 1\n";

        Run(input, output);
        assert(output.str() == "1\n");
    }
}

#endif


int main() {
    #ifdef DEBUG
        TestRun();
    #else
        Run(std::cin, std::cout);
    #endif

    return 0;
}
