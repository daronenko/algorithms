#include <iostream>
#include <cassert>
#include <iterator>
#include <string>
#include <functional>


template <typename ForwardIt, typename T>
ForwardIt LowerBound(ForwardIt first, ForwardIt last, const T& target) {
    while (first < last) {
        auto middle = std::next(first, std::distance(first, last) / 2);
        if (*middle < target) {
            first = std::next(middle);
        } else {
            last = middle;
        }
    }

    return first;
}


template <typename ForwardIt>
ForwardIt FindMinValue(ForwardIt first, ForwardIt last) {
    auto min_iter = last;
    auto left = first;
    auto right = std::prev(last);
    while (left <= right) {
        auto middle = std::next(left, std::distance(left, right) / 2);

        if (*middle <= *right) {
            if (min_iter == last || *middle < *min_iter) {
                min_iter = middle;
            }
            right = std::prev(middle);
        } else if (*middle >= *left) {
            if (min_iter == last || *left < *min_iter) {
                min_iter = left;
            }
            left = std::next(middle);
        }
    }

    return min_iter;
}


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

template <typename ForwardIt, typename Comparator = std::less<>>
void InsertionSort(ForwardIt first, ForwardIt last, Comparator compare = Comparator{}) {
    for (auto iter = std::next(first); iter < last; iter = std::next(iter)) {
        auto temp_value = std::move(*iter);
        auto reversed_iter = iter;

        for (; reversed_iter > first && compare(temp_value, *std::prev(reversed_iter)); reversed_iter = std::prev(reversed_iter)) {
            std::iter_swap(reversed_iter, std::prev(reversed_iter));
        }

        *reversed_iter = std::move(temp_value);
    }
}


void TestRun() {
    {
        std::vector<int> v{1, 3, 5, 7, 9};
        auto result = LowerBound(v.begin(), v.end(), 4) - v.begin();
        assert(result == 2);
    }
    {
        std::vector<int> v{3, 4, 5, 1, 2};
        auto result = FindMinValue(v.begin(), v.end()) - v.begin();
        assert(result == 3);
    }
    {
        std::string str = "(()(()))";
        auto result = IsValidParenthesisSequence(str);
        assert(result == true);
    }
    {
        std::vector<int> v{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        InsertionSort(v.begin(), v.end());
        for (const auto value : v) {
            std::cout << value << ' ';
        }
        std::cout << std::endl;
    }
}


int main() {
    TestRun();
}