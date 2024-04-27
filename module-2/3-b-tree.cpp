/*
 * Постройте B-дерево минимального порядка t и выведите его по слоям.
 * В качестве ключа используются числа, лежащие в диапазоне [0..2^32-1]
 * Требования:
 *   - B-дерево должно быть реализовано в виде шаблонного класса.
 *   - Решение должно поддерживать передачу функции сравнения снаружи.
 *
 * Формат ввода
 * Сначала вводится минимальный порядок дерева t.
 * Затем вводятся элементы дерева.
 *
 * Формат вывода
 * Программа должна вывести B-дерево по слоям.
 * Каждый слой на новой строке, элементы должны выводится в том порядке, в котором они лежат в узлах.
 *
 * Пример 1
 * Ввод
 * 2
 * 0 1 2 3 4 5 6 7 8 9
 * Вывод
 * 3
 * 1 5 7
 * 0 2 4 6 8 9
 *
 * Пример 2
 * Ввод
 * 4
 * 0 1 2 3 4 5 6 7 8 9
 * Вывод
 * 3
 * 0 1 2 4 5 6 7 8 9
 */


#include <iostream>
#include <vector>
#include <functional>
#include <sstream>
#include <queue>
#include <cassert>
#include <cstdint>


const bool DEBUG = false;


template <typename T, typename Compare = std::less<T>>
class BTree {
 public:
    explicit BTree(std::size_t min_degree): t_(min_degree), root_(nullptr) {
    }

    ~BTree() {
        if (root_) {
            delete root_;
        }
    }

    template <typename U>
    void Insert(U&& key) {
        if (!root_) {
            root_ = new Node(true);
        }

        // здесь и дальше: если при спуске по дереву находим
        // переполненный узел -- сначала разбиваем его, потом спускаемся
        if (isNodeFull(root_)) {
            Node* new_root = new Node(false);
            new_root->children.push_back(root_);
            root_ = new_root;
            splitChild(root_, 0);
        }

        // теперь корень точно не переполнен
        insertNonFull(root_, std::forward<T>(key));
    }

    void LevelOrderTraversal(
        const std::function<void()>& before_level,
        std::function<void(const T& key)> key_processor,
        const std::function<void()>& after_level
    ) {
        std::queue<Node*> queue;
        queue.push(root_);

        while (!queue.empty()) {
            std::size_t level_size = queue.size();

            before_level();

            for (std::size_t i = 0; i < level_size; ++i) {
                auto current_node = queue.front();
                queue.pop();

                for (const auto& key : current_node->keys) {
                    key_processor(key);
                }

                if (!current_node->leaf) {
                    for (auto child : current_node->children) {
                        queue.push(child);
                    }
                }
            }

            after_level();
        }
    }

 private:
    struct Node {
        explicit Node(bool leaf): leaf(leaf) {
        }

        ~Node() {
            for (Node* child : children) {
                delete child;
            }
        }

        bool leaf;
        std::vector<T> keys;
        std::vector<Node*> children;
    };

    bool isNodeFull(Node* node) {
        return node->keys.size() == 2 * t_ - 1;
    }

    // разбить переполненного потомка index узла node
    void splitChild(Node* node, std::size_t index) {
        auto full_child = node->children[index];

        auto new_child = new Node(full_child->leaf);
        node->children.insert(node->children.begin() + index + 1, new_child);

        node->keys.insert(node->keys.begin() + index, std::move(full_child->keys[t_ - 1]));

        for (std::size_t i = t_; i < 2 * t_ - 1; ++i) {
            new_child->keys.push_back(std::move(full_child->keys[i]));
        }
        full_child->keys.erase(full_child->keys.begin() + t_ - 1, full_child->keys.end());

        if (!full_child->leaf) {
            for (std::size_t i = t_; i < 2 * t_; ++i) {
                new_child->children.push_back(full_child->children[i]);
            }
            full_child->children.erase(full_child->children.begin() + t_, full_child->children.end());
        }
    }

    // вставить ключ key в гарантированно не переполненную ноду
    template <typename U>
    void insertNonFull(Node* node, U&& key) {
        int pos = node->keys.size() - 1;

        // гарантированно не перепеполненный лист -- запишем новый ключ в него
        if (node->leaf) {
            // расширили вектор ключей для вставки нового
            node->keys.resize(node->keys.size() + 1);
            while (pos >= 0 && compare_(key, node->keys[pos])) {
                // обходим ключи справа налево, сдвигая вправо на 1
                node->keys[pos + 1] = node->keys[pos];
                --pos;
            }
            // вставляем новый ключ на освобожденное в цикле место
            node->keys[pos + 1] = std::forward<T>(key);
        }
        else {  // не лист, значит есть потомки, пишем в один из них
            // ищем позицию потомка, в которого добавим ключ
            while (pos >= 0 && compare_(key, node->keys[pos])) {
                --pos;
            }

            // если потомок и так полон, надо его сначала разбить
            if (isNodeFull(node->children[pos + 1])) {
                splitChild(node, pos + 1);
                // после разбиения потомка в текущий узел из него поднялся ключ
                // надо сравниться и с ним
                if (compare_(node->keys[pos + 1], key))
                    ++pos;
            }
            insertNonFull(node->children[pos + 1], std::forward<T>(key));
        }
    }

    std::size_t t_;
    Node* root_;
    Compare compare_;
};


void Run(std::istream& input, std::ostream& output) {
    std::size_t t;
    input >> t;

    BTree<std::uint32_t> btree(t);

    std::uint32_t key;
    while (input >> key) {
        btree.Insert(key);
    }

    btree.LevelOrderTraversal(
        [](){},
        [&output](const auto& key){
            output << key << ' ';
        },
        [&output](){
            output << std::endl;
        }
    );
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "2\n";
        input << "0 1 2 3 4 5 6 7 8 9\n";

        Run(input, output);
        assert(output.str() == "3 \n1 5 7 \n0 2 4 6 8 9 \n");
    }
    {
        std::stringstream input, output;

        input << "4\n";
        input << "0 1 2 3 4 5 6 7 8 9\n";

        Run(input, output);
        assert(output.str() == "3 \n0 1 2 4 5 6 7 8 9 \n");
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