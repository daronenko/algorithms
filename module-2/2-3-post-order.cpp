/*
 * Дано число N ≤ 10^4 и последовательность целых чисел из [-2^31..2^31] длиной N. Требуется построить бинарное дерево,
 * заданное наивным порядком вставки. Т.е., при добавлении очередного числа K в дерево с корнем root_, если root_→Key ≤ K,
 * то узел K добавляется в правое поддерево root_; иначе в левое поддерево root_. Выведите элементы в порядке post-order
 * (снизу вверх).
 *
 * Рекурсия запрещена.
 *
 * Пример 1
 * Ввод
 * 10
 * 7
 * 2
 * 10
 * 8
 * 5
 * 3
 * 6
 * 4
 * 1
 * 9
 * Вывод
 * 1 4 3 6 5 2 9 8 10 7
 *
 * Пример 2
 * Ввод
 * 10
 * 7
 * 10
 * 2
 * 6
 * 1
 * 9
 * 4
 * 5
 * 8
 * 3
 * Вывод
 * 1 3 5 4 6 2 8 9 10 7
 *
 * Пример 3
 * Ввод
 * 10
 * 4
 * 3
 * 9
 * 5
 * 6
 * 8
 * 7
 * 2
 * 10
 * 1
 * Вывод
 * 1 2 3 7 8 6 5 10 9 4
 */

#include <sstream>
#include <iostream>
#include <cassert>
#include <functional>
#include <stack>
#include <utility>
#include <cstdint>


const bool DEBUG = false;


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
    std::size_t size;
    input >> size;

    SearchTree<std::uint32_t> search_tree;
    for (std::size_t i = 0; i < size; ++i) {
        std::uint32_t value;
        input >> value;

        search_tree.Insert(value);
    }

    auto postorder_iter = search_tree.GetPostorderIterator();
    for (std::size_t i = 0; i < search_tree.Size(); ++i) {
        output << *postorder_iter << ' ';
        postorder_iter += 1;
    }
    output << std::endl;
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "10\n";
        input << "7\n";
        input << "2\n";
        input << "10\n";
        input << "8\n";
        input << "5\n";
        input << "3\n";
        input << "6\n";
        input << "4\n";
        input << "1\n";
        input << "9\n";

        Run(input, output);
        assert(output.str() == "1 4 3 6 5 2 9 8 10 7 \n");
    }
    {
        std::stringstream input, output;

        input << "10\n";
        input << "7\n";
        input << "10\n";
        input << "2\n";
        input << "6\n";
        input << "1\n";
        input << "9\n";
        input << "4\n";
        input << "5\n";
        input << "8\n";
        input << "3\n";

        Run(input, output);
        assert(output.str() == "1 3 5 4 6 2 8 9 10 7 \n");
    }
    {
        std::stringstream input, output;

        input << "10\n";
        input << "4\n";
        input << "3\n";
        input << "9\n";
        input << "5\n";
        input << "6\n";
        input << "8\n";
        input << "7\n";
        input << "2\n";
        input << "10\n";
        input << "1\n";

        Run(input, output);
        assert(output.str() == "1 2 3 7 8 6 5 10 9 4 \n");
    }
}


int main() {
    if (DEBUG) {
        TestRun();
    } else {
        Run(std::cin, std::cout);
    }

    return 0;
}