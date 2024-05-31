#include <iostream>
#include <sstream>
#include <cassert>
#include <string>


const bool DEBUG = false;


template <typename T>
class Stack {
 public:
    Stack(): buffer_(new T[capacity_]) {}

    Stack& operator = (const T& rhs) {
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

    ~Stack() {
        delete[] buffer_;
    }

    void Reserve(std::size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

        T* new_buffer = new T[new_capacity];
        for (std::size_t i = 0; i < Size(); ++i) {
            new_buffer[i] = buffer_[i];
        }

        delete[] buffer_;
        buffer_ = new_buffer;
        capacity_ = new_capacity;
    }

    void PushBack(const T& value) {
        if (size_ == capacity_) {
            Reserve(2 * capacity_);
        }

        buffer_[size_++] = value;
    }

    T PopBack() {
        if (Empty()) {
            throw std::length_error("cannot pop value from empty stack");
        }

        return buffer_[--size_];
    }

    T& Top() {
        if (Empty()) {
            throw std::length_error("cannot get top value from empty stack");
        }

        return buffer_[size_ - 1];
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

bool IsValidParenthesisSequence(std::string_view brackets) {
    Stack<char> stack;
    for (const auto bracket : brackets) {
        if (bracket == '(') {
            stack.PushBack(bracket);
        } else if (!stack.Empty() && stack.Top() == '(') {
            stack.PopBack();
        } else {
            return false;
        }
    }

    return stack.Empty();
}


void Run(std::istream& input, std::ostream& output) {
    std::string brackets;
    input >> brackets;

    output << (IsValidParenthesisSequence(brackets) ? "YES" : "NO") << std::endl;
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "()\n";

        Run(input, output);
        assert(output.str() == "YES\n");
    }
    {
        std::stringstream input, output;

        input << ")(\n";

        Run(input, output);
        assert(output.str() == "NO\n");
    }
}


int main() {
    if (DEBUG) {
        TestRun();
    } else {
        Run(std::cin, std::cout);
    }
}