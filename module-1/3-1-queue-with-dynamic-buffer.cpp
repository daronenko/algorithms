/*
 * Реализовать очередь с динамическим зацикленным буфером. Обрабатывать команды push back и pop front.
 *
 * Формат ввода
 * В первой строке количество команд n. n ≤ 1000000.
 * Каждая команда задаётся как 2 целых числа: a b.
 * a = 2 - pop front
 * a = 3 - push back
 * Если дана команда pop front, то число b - ожидаемое значение. Если команда pop front вызвана для пустой структуры
 * данных, то ожидается “-1”.
 *
 * Формат вывода
 * Требуется напечатать YES - если все ожидаемые значения совпали. Иначе, если хотя бы одно ожидание не оправдалось,
 * то напечатать NO.
 *
 * Пример 1
 * Ввод
 * 3
 * 3 44
 * 3 50
 * 2 44
 * Вывод
 * YES
 *
 * Пример 2
 * Ввод
 * 2
 * 2 -1
 * 3 10
 * Вывод
 * YES
 *
 * Пример 3
 * Ввод
 * 2
 * 3 44
 * 2 66
 * Вывод
 * NO
 */


#include <iostream>
#include <sstream>
#include <exception>
#include <cstdint>
#include <cassert>


constexpr bool DEBUG = true;


template <typename T>
class Queue {
 public:
    explicit Queue(): buffer_(new T[capacity_]) {}

    Queue& operator = (const T& rhs) {
        if (this == rhs) {
            return *this;
        }

        delete[] buffer_;
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;
        buffer_ = new T[capacity_];
        memcpy(buffer_, rhs.buffer_, size_);

        return *this;
    }

    ~Queue() {
        delete[] buffer_;
    }

    void Reserve(std::size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

        T* new_buffer = new T[new_capacity];
        for (std::size_t i = 0; i < Size(); ++i) {
            new_buffer[i] = buffer_[head_++];
            head_ %= capacity_;
        }

        delete[] buffer_;
        buffer_ = new_buffer;
        capacity_ = new_capacity;

        head_ = 0;
        tail_ = size_;
    }

    void PushBack(const T& value) {
        if (size_ == capacity_) {
            Reserve(2 * capacity_);
        }

        buffer_[tail_++] = value;
        tail_ %= capacity_;
        ++size_;
    }

    T PopFront() {
        if (Empty()) {
            throw std::length_error("cannot pop value from empty queue");
        }

        T temp = buffer_[head_++];
        head_ %= capacity_;
        --size_;

        return temp;
    }

    T& Front() {
        return buffer_[head_];
    }

    [[nodiscard]] std::size_t Size() const noexcept {
        return size_;
    }

    [[nodiscard]] std::size_t Empty() const noexcept {
        return size_ == 0;
    }

 private:
    std::size_t capacity_{4};
    std::size_t size_{0};
    T* buffer_{nullptr};

    std::size_t head_{0};
    std::size_t tail_{0};
};


void Run(std::istream& input, std::ostream& output) {
    std::uint32_t commands_count;
    input >> commands_count;

    Queue<std::int64_t> queue;

    bool is_correct = true;
    while (commands_count-- && is_correct) {
        std::uint16_t command;
        std::int64_t number;
        input >> command >> number;

        switch (command) {
            case 2:
                try {
                    is_correct = (queue.PopFront() == number);
                } catch (const std::length_error&) {
                    if (number != -1) {
                        is_correct = false;
                    }
                }
                break;
            case 3:
                queue.PushBack(number);
                break;
            default:
                throw std::invalid_argument("invalid command passed");
        }
    }

    output << (is_correct ? "YES" : "NO") << std::endl;
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "3\n";
        input << "3 44\n";
        input << "3 50\n";
        input << "2 44\n";

        Run(input, output);
        assert(output.str() == "YES\n");
    }
    {
        std::stringstream input, output;

        input << "2\n";
        input << "2 -1\n";
        input << "3 10\n";

        Run(input, output);
        assert(output.str() == "YES\n");
    }
    {
        std::stringstream input, output;

        input << "2\n";
        input << "3 44\n";
        input << "2 66\n";

        Run(input, output);
        assert(output.str() == "NO\n");
    }
    {
        std::stringstream input, output;

        input << "13\n";
        input << "3 11\n";
        input << "3 22\n";
        input << "2 11\n";
        input << "3 33\n";
        input << "3 44\n";
        input << "3 55\n";
        input << "3 66\n";
        input << "2 22\n";
        input << "2 33\n";
        input << "2 44\n";
        input << "2 55\n";
        input << "2 66\n";
        input << "2 -1\n";

        Run(input, output);
        assert(output.str() == "YES\n");
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