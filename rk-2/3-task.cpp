/*
 * Нужно кастомизировать std::unordered_set для операций со структурой данных
 *
 * struct Node {
 *     std::string product_name;
 *     int color;
 *     int size;
 * };
 *
 * Формат ввода
 * Каждая строка входных данных задает одну операцию над множеством.
 * Запись операции состоит из типа операции и следующей за ним через пробел тройки значений структуры, над которой
 * проводится операция.
 * Тип операции – один из трех символов:
 * + означает добавление данной строки в множество;
 * - означает удаление строки из множества;
 * ? означает проверку принадлежности данной строки множеству.
 * При добавлении элемента в множество НЕ ГАРАНТИРУЕТСЯ, что он отсутствует в этом множестве.
 * При удалении элемента из множества НЕ ГАРАНТИРУЕТСЯ, что он присутствует в этом множестве.
 *
 * Формат вывода
 * Программа должна вывести для каждой операции одну из двух строк OK или FAIL, в зависимости от того, встречается
 * ли данные в нашем множестве.
 */

#include <sstream>
#include <iostream>
#include <string>
#include <unordered_set>

// #define DEBUG


struct Node {
    std::string product_name;
    int color;
    int size;
};


void HashCombine(std::size_t& seed, std::size_t hash) {
    seed ^= hash + (seed << 3);
}


class StringHash {
 public:
    explicit StringHash(std::size_t prime = PRIME): prime_(prime) {
    }

    std::size_t operator () (const std::string& str) const {
        std::size_t hash = 0;
        for (auto symbol : str) {
            hash = prime_ * hash + symbol;
        }

        return hash;
    }

 private:
    static const std::size_t PRIME = 1610612741;
    std::size_t prime_;
};


template<>
struct std::hash<Node> {
    std::size_t operator()(const Node& node) const noexcept {
        std::size_t hash_1 = StringHash{}(node.product_name);
        std::size_t hash_2 = std::hash<int>{}(node.color);
        std::size_t hash_3 = std::hash<int>{}(node.size);

        std::size_t seed = 0;
        HashCombine(seed, hash_1);
        HashCombine(seed, hash_2);
        HashCombine(seed, hash_3);

        return seed;
    }
};

bool operator == (const Node& lhs, const Node& rhs) {
    return std::hash<Node>{}(lhs) == std::hash<Node>{}(rhs);
}


void Run(std::istream& input, std::ostream& output) {
    char operation;
    Node node;

    std::unordered_set<Node> set;
    while (input >> operation >> node.product_name >> node.color >> node.size) {
        switch (operation) {
            case '+':
                output << (set.insert(node).second ? "OK" : "FAIL") << std::endl;
                break;
            case '-':
                output << (set.erase(node) ? "OK" : "FAIL") << std::endl;
                break;
            case '?':
                output << (set.contains(node) ? "OK" : "FAIL") << std::endl;
                break;
            default:
                return;
        }
    }
}


#ifdef DEBUG

#include <cassert>

void TestRun() {
    {
        std::stringstream input, output;

        input << "+ a 1 2\n+ a 1 1\n+ a 2 1\n? a 1 1\n? b 2 1\n? a 1 3\n? b 1 1\n";

        Run(input, output);
        assert(output.str() == "OK\nOK\nOK\nOK\nFAIL\nFAIL\nFAIL\n");
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
