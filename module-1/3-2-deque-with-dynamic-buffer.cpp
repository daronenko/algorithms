/*
 * Реализовать дек с динамическим зацикленным буфером.
 * Обрабатывать команды push * и pop *.
 *
 * Формат ввода
 * В первой строке количество команд n. n ≤ 1000000.
 * Каждая команда задаётся как 2 целых числа: a b.
 * a = 1 - push front
 * a = 2 - pop front
 * a = 3 - push back
 * a = 4 - pop back
 * Если дана команда pop *, то число b - ожидаемое значение. Если команда pop * вызвана для пустой структуры данных,
 * то ожидается “-1”.
 *
 * Пример 1
 * Ввод
 * 3
 * 1 44
 * 3 50
 * 2 44
 * Вывод
 * YES
 *
 * Пример 2
 * Ввод
 * 2
 * 2 -1
 * 1 10
 * Вывод
 * YES
 *
 * Пример 3
 * Ввод
 * 2
 * 3 44
 * 4 66
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
class Deque {
public:
    Deque(): buffer_(new T[capacity_]) {}

    Deque& operator = (const T& rhs) {
        if (this == rhs) {
            return *this;
        }

        delete[] buffer_;
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;
        head_ = rhs.head_;
        tail_ = rhs.tail_;
        buffer_ = new T[capacity_];
        memcpy(buffer_, rhs.buffer_, size_);

        return *this;
    }

    ~Deque() {
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

    void PushFront(const T& value) {
        if (size_ == capacity_) {
            Reserve(2 * capacity_);
        }

        head_ = (head_ + capacity_ - 1) % capacity_;
        buffer_[head_] = value;
        ++size_;
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
            throw std::length_error("cannot pop value from empty deque");
        }

        --size_;
        T temp = buffer_[head_++];
        head_ %= capacity_;

        return temp;
    }

    T PopBack() {
        if (Empty()) {
            throw std::length_error("cannot pop value from empty deque");
        }

        --size_;
        tail_ = (tail_ + capacity_ - 1) % capacity_;
        return buffer_[tail_];
    }

    T& Front() {
        if (Empty()) {
            throw std::length_error("cannot get front value from empty deque");
        }

        return buffer_[head_];
    }

    T& Back() {
        if (Empty()) {
            throw std::length_error("cannot get back value from empty deque");
        }

        return buffer_[(tail_ + capacity_ - 1) % capacity_];
    }

    [[nodiscard]] std::size_t Size() const noexcept {
        return size_;
    }

    [[nodiscard]] std::size_t Empty() const noexcept {
        return size_ == 0;
    }

private:
    std::size_t capacity_{2};
    std::size_t size_{0};
    T* buffer_{nullptr};

    std::size_t head_{0};
    std::size_t tail_{0};
};


void Run(std::istream& input, std::ostream& output) {
    std::uint32_t commands_count;
    input >> commands_count;

    Deque<std::int64_t> deque;

    bool is_correct = true;
    while (commands_count-- && is_correct) {
        std::uint16_t command;
        std::int64_t number;
        input >> command >> number;

        switch (command) {
            case 1:
                deque.PushFront(number);
                break;
            case 2:
                try {
                    is_correct = (deque.PopFront() == number);
                } catch (const std::length_error&) {
                    if (number != -1) {
                        is_correct = false;
                    }
                }
                break;
            case 3:
                deque.PushBack(number);
                break;
            case 4:
                try {
                    is_correct = (deque.PopBack() == number);
                } catch (const std::length_error&) {
                    if (number != -1) {
                        is_correct = false;
                    }
                }
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
        input << "1 44\n";
        input << "3 50\n";
        input << "2 44\n";

        Run(input, output);
        assert(output.str() == "YES\n");
    }
    {
        std::stringstream input, output;

        input << "2\n";
        input << "2 -1\n";
        input << "1 10\n";

        Run(input, output);
        assert(output.str() == "YES\n");
    }
    {
        std::stringstream input, output;

        input << "2\n";
        input << "3 44\n";
        input << "4 66\n";

        Run(input, output);
        assert(output.str() == "NO\n");
    }
    {
        std::stringstream input, output;

        input << "9\n";
        input << "3 22\n";
        input << "3 33\n";
        input << "3 44\n";
        input << "1 11\n";
        input << "4 44\n";
        input << "2 11\n";
        input << "2 22\n";
        input << "4 33\n";
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