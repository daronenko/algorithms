/*
 * Реализуйте AVL-дерево. Решения с использованием других структур засчитываться не будут.
 * Входной файл содержит описание операций с деревом. Операций не больше 10^5.
 *
 * Формат ввода
 * В каждой строке находится одна из следующих операций:
 * insert x — добавить в дерево ключ x. Если ключ x есть в дереве, то ничего делать не надо.
 * delete x — удалить из дерева ключ x. Если ключа x в дереве нет, то ничего делать не надо.
 * exists x — если ключ x есть в дереве, вывести «true», иначе «false»
 * next x — минимальный элемент в дереве, больший x, или «none», если такого нет.
 * prev x — максимальный элемент в дереве, меньший x, или «none», если такого нет.
 * Все числа во входном файле целые и по модулю не превышают 10^9.
 *
 * Формат вывода
 * Выведите последовательно результат выполнения всех операций exists, next, prev. Следуйте формату выходного файла
 * из примера.
 */

#include <sstream>
#include <iostream>
#include <functional>
#include <cstdint>
#include <optional>

// #define DEBUG


template <typename T, typename Compare = std::less<T>>
class AVLTree {
 private:
    struct Node {
        template <typename U>
        explicit Node(U&& data): value(std::forward<U>(data)), left(nullptr), right(nullptr), height(1), nodes_count(1) {
        }

        T value;
        Node* left;
        Node* right;
        std::size_t height;
        std::size_t nodes_count;
    };

 public:
    AVLTree(): root_(nullptr) {
    }

    ~AVLTree() {
        destroyTree(root_);
    }

    template <typename U>
    void Add(U&& value) {
        root_ = addInternal(root_, std::forward<U>(value));
    }

    bool Has(const T& value) const {
        Node* tmp = root_;
        while (tmp) {
            if (tmp->value == value) {
                return true;
            } else if (compare_(tmp->value, value)) {
                tmp = tmp->right;
            } else {
                tmp = tmp->left;
            }
        }

        return false;
    }

    void Delete(const T& value) {
        root_ = deleteInternal(root_, value);
    }

    std::optional<T> Next(const T& value) const {
        std::optional<T> result;

        Node* current_node = root_;
        while (current_node) {
            if (compare_(value, current_node->value)) {
                result = current_node->value;
                current_node = current_node->left;
            } else {
                current_node = current_node->right;
            }
        }

        if (result == value) {
            result.reset();
        }

        return result;
    }

    std::optional<T> Prev(const T& value) const {
        std::optional<T> result;

        Node* current_node = root_;
        while (current_node) {
            if (compare_(current_node->value, value)) {
                result = current_node->value;
                current_node = current_node->right;
            } else {
                current_node = current_node->left;
            }
        }

        if (result == value) {
            result.reset();
        }

        return result;
    }

 private:
    void destroyTree(Node* node) {
        if (!node) {
            return;
        }

        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    Node* deleteInternal(Node* node, const T& value) {
        if (!node) {
            return nullptr;
        }

        if (node->value == value) {
            Node* left = node->left;
            Node* right = node->right;

            delete node;

            if (!left) {
                return right;
            }

            if (!right) {
                return left;
            }

            Node* extracted_node;
            if (left->height > right->height) {
                Node* left_node = extractMax(left, extracted_node);
                extracted_node->left = left_node;
                extracted_node->right = right;
            } else {
                Node* right_node = extractMin(right, extracted_node);
                extracted_node->right = right_node;
                extracted_node->left = left;
            }

            return doBalance(extracted_node);

        } else if (compare_(node->value, value)) {
            node->right = deleteInternal(node->right, value);

        } else {
            node->left = deleteInternal(node->left, value);
        }

        return doBalance(node);
    }

    Node* extractMin(Node* node, Node*& min_node) {
        if (!node->left) {
            min_node = node;
            return node->right;
        }

        node->left = extractMin(node->left, min_node);

        return doBalance(node);
    }

    Node* extractMax(Node* node, Node*& max_node) {
        if (!node->right) {
            max_node = node;
            return node->left;
        }

        node->right = extractMax(node->right, max_node);

        return doBalance(node);
    }

    template <typename U>
    Node* addInternal(Node* node, U&& value) {
        if (!node) {
            return new Node(std::forward<U>(value));
        }

        if (compare_(value, node->value)) {
            node->left = addInternal(node->left, std::forward<U>(value));
        } else {
            node->right = addInternal(node->right, std::forward<U>(value));
        }

        return doBalance(node);
    }

    std::size_t getHeight(Node* node) const {
        return node ? node->height : 0;
    }

    void fixHeight(Node* node) {
        node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;
    }

    std::size_t getNodesCount(Node* node) const {
        return node ? node->nodes_count : 0;
    }

    void fixNodesCount(Node* node) {
        node->nodes_count = getNodesCount(node->left) + getNodesCount(node->right) + 1;
    }

    std::int32_t getBalance(Node* node) const {
        return getHeight(node->right) - getHeight(node->left);
    }

    Node* rotateLeft(Node* node) {
        Node *tmp = node->right;
        node->right = tmp->left;
        tmp->left = node;

        fixHeight(node);
        fixHeight(tmp);

        fixNodesCount(node);
        fixNodesCount(tmp);

        return tmp;
    }

    Node* rotateRight(Node* node) {
        Node *tmp = node->left;
        node->left = tmp->right;
        tmp->right = node;

        fixHeight(node);
        fixHeight(tmp);

        fixNodesCount(node);
        fixNodesCount(tmp);

        return tmp;
    }

    Node* doBalance(Node* node) {
        fixHeight(node);
        fixNodesCount(node);

        switch (getBalance(node)) {
            case 2: {
                if (getBalance(node->right) < 0) {
                    node->right = rotateRight(node->right);
                }
                return rotateLeft(node);
            }
            case -2: {
                if (getBalance(node->left) > 0) {
                    node->left = rotateLeft(node->left);
                }
                return rotateRight(node);
            }
            default:
                return node;
        }
    }

    Node* root_;
    Compare compare_;
};


void Run(std::istream& input, std::ostream& output) {
    std::string operation;

    AVLTree<std::int64_t> avl_tree;
    while (input >> operation) {
        std::int64_t value;
        input >> value;

        if (operation == "insert") {
            if (!avl_tree.Has(value)) {
                avl_tree.Add(value);
            }
        } else if (operation == "exists") {
            output << (avl_tree.Has(value) ? "true" : "false") << std::endl;
        } else if (operation == "delete") {
            if (avl_tree.Has(value)) {
                avl_tree.Delete(value);
            }
        } else if (operation == "next") {
            auto next = avl_tree.Next(value);
            if (next.has_value()) {
                output << next.value() << std::endl;
            } else {
                output << "none" << std::endl;
            }
        } else if (operation == "prev") {
            auto prev = avl_tree.Prev(value);
            if (prev.has_value()) {
                output << prev.value() << std::endl;
            } else {
                output << "none" << std::endl;
            }
        } else {
            return;
        }
    }
}


#ifdef DEBUG

#include <cassert>

void TestRun() {
    {
        std::stringstream input, output;

        input << "insert 2\ninsert 5\ninsert 3\nexists 2\nexists 4\nnext 4\nprev 4\ndelete 5\nnext 4\nprev 4\n";

        Run(input, output);
        assert(output.str() == "true\nfalse\n5\n3\nnone\n3\n");
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
