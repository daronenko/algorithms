/*
 * Дано число N и N строк. Каждая строка содержащит команду добавления или удаления натуральных чисел, а также запрос
 * на получение k-ой порядковой статистики. Команда добавления числа A задается положительным числом A, команда удаления
 * числа A задается отрицательным числом “-A”. Запрос на получение k-ой порядковой статистики задается числом k.
 * Требуемая скорость выполнения запроса - O(log n).
 *
 * Пример 1
 * Ввод
 * 3
 * 1 0
 * 2 0
 * -1 0
 * Вывод
 * 1 1 2
 *
 * Пример 2
 * Ввод
 * 5
 * 40 0
 * 10 1
 * 4 1
 * -10 0
 * 50 2
 * Вывод
 * 40
 * 40
 * 10
 * 4
 * 50
 */


#include <sstream>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <cassert>
#include <cstdint>
#include <cmath>


const bool DEBUG = true;


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

    [[nodiscard]] const T& KthMean(std::size_t k) const {
        Node* kth_mean_node = kthMeanInternal(root_, k);
        if (kth_mean_node) {
            return kth_mean_node->value;
        }

        throw std::out_of_range("invalid value of `k`");
    }

 private:
    Node* kthMeanInternal(Node* node, std::size_t k) const {
        std::size_t left_nodes_count = getNodesCount(node->left);
        if (left_nodes_count == k) {
            return node;
        }

        if (left_nodes_count > k) {
            return kthMeanInternal(node->left, k);
        } else {
            return kthMeanInternal(node->right, k - left_nodes_count - 1);
        }
    }

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
            if (left->height < right->height) {
                Node* right_node = extractMin(right, extracted_node);
                extracted_node->right = right_node;
                extracted_node->left = left;
            } else {
                Node* left_node = extractMax(left, extracted_node);
                extracted_node->left = left_node;
                extracted_node->right = right;
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
    std::size_t queries_count;
    input >> queries_count;

    AVLTree<std::int32_t> avl_tree;
    for (std::size_t i = 0; i < queries_count; ++i) {
        std::int32_t value;
        std::size_t k;
        input >> value >> k;

        if (value > 0) {
            avl_tree.Add(value);
        } else {
            avl_tree.Delete(std::abs(value));
        }

        output << avl_tree.KthMean(k) << std::endl;
    }
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "3\n";
        input << "1 0\n";
        input << "2 0\n";
        input << "-1 0\n";

        Run(input, output);
        assert(output.str() == "1\n1\n2\n");
    }
    {
        std::stringstream input, output;

        input << "5\n";
        input << "40 0\n";
        input << "10 1\n";
        input << "4 1\n";
        input << "-10 0\n";
        input << "50 2\n";

        Run(input, output);
        assert(output.str() == "40\n40\n10\n4\n50\n");
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