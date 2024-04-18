/*
 * Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы с открытой адресацией.
 * Хранимые строки непустые и состоят из строчных латинских букв. Хеш-функция строки должна быть реализована с помощью
 * вычисления значения многочлена методом Горнера. Начальный размер таблицы должен быть равным 8-ми. Перехеширование
 * выполняйте при добавлении элементов в случае, когда коэффициент заполнения таблицы достигает 3/4. Структура данных
 * должна поддерживать операции добавления строки в множество, удаления строки из множества и проверки принадлежности
 * данной строки множеству.
 * Для разрешения коллизий используйте квадратичное пробирование.
 * i-ая проба g(k, i) = g(k, i - 1) + i (mod m). m - степень двойки.
 *
 * Формат ввода
 * Каждая строка входных данных задает одну операцию над множеством. Запись операции состоит из типа операции
 * и следующей за ним через пробел строки, над которой проводится операция. Тип операции – один из трех символов:
 * + означает добавление данной строки в множество; - означает удаление строки из множества; ? означает проверку
 * принадлежности данной строки множеству. При добавлении элемента в множество НЕ ГАРАНТИРУЕТСЯ, что он отсутствует
 * в этом множестве. При удалении элемента из множества НЕ ГАРАНТИРУЕТСЯ, что он присутствует в этом множестве.
 *
 * Формат вывода
 * Программа должна вывести для каждой операции одну из двух строк OK или FAIL.
 * Для операции '?': OK, если элемент присутствует во множестве. FAIL иначе.
 * Для операции '+': FAIL, если добавляемый элемент уже присутствует во множестве и потому не может быть добавлен.
 * OK иначе.
 * Для операции '-': OK, если элемент присутствовал во множестве и успешно удален. FAIL иначе.
 *
 * Пример
 * Ввод
 * + hello
 * + bye
 * ? bye
 * + bye
 * - bye
 * ? bye
 * ? hello
 * Вывод
 * OK
 * OK
 * OK
 * FAIL
 * OK
 * FAIL
 * OK
 */


#include <sstream>
#include <iostream>
#include <cassert>
#include <functional>
#include <optional>


const bool DEBUG = false;


template <typename T, typename Hash = std::hash<T>>
class Set {
 public:
    explicit Set(std::size_t size = INIT_SIZE): size_(0), table_(size) {
    }

    template <typename U>
    bool Add(U&& key) {
        if (4 * size_ > 3 * table_.size()) {
            Reserve(2 * table_.size());
        }

        auto current_position = hash_(key) % table_.size();
        std::optional<std::size_t> deleted_position;
        for (std::size_t i = 0; i < table_.size(); ++i) {
            current_position = (current_position + i) % table_.size();
            auto& current_node = table_[current_position];

            if (current_node.type == Node::Type::KEY
                    && current_node.content.value() == key) {
                return false;

            } else if (current_node.type == Node::Type::DELETED) {
                deleted_position = current_position;

            } else if (current_node.type == Node::Type::NONE) {
                if (deleted_position.has_value()) {
                    auto& deleted_node = table_[deleted_position.value()];
                    deleted_node.Push(std::forward<U>(key));

                } else {
                    current_node.Push(std::forward<U>(key));
                }

                ++size_;
                return true;
            }
        }

        if (deleted_position.has_value()) {
            auto& deleted_node = table_[deleted_position.value()];
            deleted_node.Push(std::forward<U>(key));
            ++size_;
            return true;
        }

        return false;
    }

    bool Remove(const T& key) {
        auto current_position = hash_(key) % table_.size();
        for (std::size_t i = 0; i < table_.size(); ++i) {
            current_position = (current_position + i) % table_.size();
            auto& current_node = table_[current_position];

            if (current_node.type == Node::Type::KEY
                    && current_node.content.value() == key) {
                current_node.Pop();
                --size_;
                return true;

            } else if (table_[current_position].type == Node::Type::NONE) {
                return false;
            }
        }

        return false;
    }

    [[nodiscard]] bool Contains(const T& key) const {
        auto current_position = hash_(key) % table_.size();
        for (std::size_t i = 0; i < table_.size(); ++i) {
            current_position = (current_position + i) % table_.size();
            auto& current_node = table_[current_position];

            if (current_node.type == Node::Type::KEY
                    && current_node.content.value() == key) {
                return true;

            } else if (current_node.type == Node::Type::NONE) {
                return false;
            }
        }

        return false;
    }

 private:
    void Reserve(std::size_t new_size) {
        std::vector<Node> new_table(new_size);

        for (std::size_t i = 0; i < table_.size(); ++i) {
            if (table_[i].type == Node::Type::KEY) {
                auto new_position = hash_(table_[i].content.value()) % new_table.size();
                for (std::size_t j = 0; j < new_table.size(); ++j) {
                    new_position = (new_position + j) % new_table.size();
                    auto& new_node = new_table[new_position];

                    if (new_node.type == Node::Type::NONE) {
                        new_node = std::move(table_[i]);
                        break;
                    }
                }
            }
        }

        table_ = std::move(new_table);
    }

    struct Node {
        enum class Type {
            KEY,
            NONE,
            DELETED
        } type;

        std::optional<T> content;

        explicit Node(const T& key): type(Type::KEY), content(key) {
        }

        explicit Node(T&& key) noexcept: type(Type::KEY), content(std::move(key)) {
        }

        Node& operator = (const Node& other)  {
            if (this == &other) {
                return *this;
            }

            type = other.type;
            content = other.content;

            return *this;
        }

        Node& operator = (Node&& other) noexcept {
            if (this == &other) {
                return *this;
            }

            type = other.type;
            content = std::move(other.content);

            return *this;
        }

        Node(): type(Type::NONE) {
        }

        void Push(const T& new_value) {
            type = Type::KEY;
            content = new_value;
        }

        void Push(T&& new_value) {
            type = Type::KEY;
            content = std::move(new_value);
        }

        void Pop() {
            type = Type::DELETED;
            content.reset();
        }
    };

    static const std::size_t INIT_SIZE = 8;

    Hash hash_;
    std::size_t size_;
    std::vector<Node> table_;
};


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


void Run(std::istream& input, std::ostream& output) {
    Set<std::string, StringHash> hash_table;

    char action;
    std::string key;

    while (input >> action >> key) {
        switch (action) {
            case '+':
                output << (hash_table.Add(std::move(key)) ? "OK" : "FAIL") << std::endl;
                break;
            case '-':
                output << (hash_table.Remove(key) ? "OK" : "FAIL") << std::endl;
                break;
            case '?':
                output << (hash_table.Contains(key) ? "OK" : "FAIL") << std::endl;
                break;
            default:
                return;
        }
    }
}


void TestRun() {
    {
        std::stringstream input, output;

        input << "+ hello\n";
        input << "+ bye\n";
        input << "? bye\n";
        input << "+ bye\n";
        input << "- bye\n";
        input << "? bye\n";
        input << "? hello\n";

        Run(input, output);
        assert(output.str() == "OK\nOK\nOK\nFAIL\nOK\nFAIL\nOK\n");
    }
    {
        std::stringstream input, output;

        input << "+ a\n";
        input << "+ b\n";
        input << "? c\n";
        input << "- a\n";
        input << "? a\n";
        input << "+ a\n";
        input << "? a\n";

        Run(input, output);
        assert(output.str() == "OK\nOK\nFAIL\nOK\nFAIL\nOK\nOK\n");
    }
    {
        std::stringstream input, output;

        input << "+ a\n";
        input << "+ b\n";
        input << "+ c\n";
        input << "? a\n";
        input << "? b\n";
        input << "? c\n";

        Run(input, output);
        assert(output.str() == "OK\nOK\nOK\nOK\nOK\nOK\n");
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
