/*
 * Напишите программу, которая использует кучу для слияния K отсортированных массивов суммарной длиной N.
 *
 * Требования:
 * Время работы O(N * logK)
 * Куча должна быть реализована в виде шаблонного класса.
 * Решение должно поддерживать передачу функции сравнения снаружи.
 * Куча должна быть динамической.
 *
 * Формат ввода
 * Сначала вводится количество массивов K.
 * Затем по очереди размер каждого массива и элементы массива.
 * Каждый массив упорядочен по возрастанию.
 *
 * Формат вывода
 * Итоговый отсортированный массив.
 *
 * Пример
 * Ввод
 * 3
 * 1
 * 6
 * 2
 * 50 90
 * 3
 * 1 10 70
 * Вывод
 * 1 6 10 50 70 90
 */


#include <sstream>
#include <iostream>
#include <cassert>
#include <functional>
#include <algorithm>


const bool DEBUG = true;


template <typename T, typename Comparator = std::less<T>>
class Heap {
 public:
    Heap(Comparator compare = Comparator{})
        : buffer_(new T[capacity_]), compare_(compare) {
    }

    explicit Heap(std::size_t capacity, Comparator compare = Comparator{})
        : capacity_(capacity), buffer_(new T[capacity]), compare_(compare) {
    }

    Heap& operator = (const Heap& rhs) {
        if (this == rhs) {
            return *this;
        }

        delete[] buffer_;
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;
        buffer_ = new T[capacity_];
        std::copy(rhs.buffer_, rhs.buffer_ + size_, buffer_);

        return *this;
    }

    ~Heap() {
        delete[] buffer_;
    }

    [[nodiscard]] std::size_t Size() const noexcept {
        return size_;
    }

    [[nodiscard]] bool Empty() const noexcept {
        return !size_;
    }

    [[nodiscard]] const T& Top() const {
        return buffer_[0];
    }

    T& Top() {
        return buffer_[0];
    }

    void Pop() {
        if (Empty()) {
            throw std::length_error("cannot pop value from empty heap");
        }

        std::swap(buffer_[0], buffer_[--size_]);
        (buffer_ + size_)->~T();
        SiftDown_(0);
    }

    void Push(const T& value) {
        if (size_ == capacity_) {
            Reserve_(2 * capacity_);
        }

        buffer_[size_++] = value;
        SiftUp_(size_ - 1);
    }

    void Push(T&& value) {
        if (size_ == capacity_) {
            Reserve_(2 * capacity_);
        }

        buffer_[size_++] = std::move(value);
        SiftUp_(size_ - 1);
    }

 private:
    void Reserve_(std::size_t new_capacity) {
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

    void SiftUp_(std::size_t current_node) {
        std::size_t parent_node = (current_node - 1) / 2;
        while (current_node > 0 && compare_(buffer_[current_node], buffer_[parent_node])) {
            std::swap(buffer_[current_node], buffer_[parent_node]);
            current_node = parent_node;
            parent_node = (current_node - 1) / 2;
        }
    }

    void SiftDown_(std::size_t current_node) {
        while (2 * current_node + 1 < size_) {
            std::size_t left_child = 2 * current_node + 1;
            std::size_t right_child = 2 * current_node + 2;

            std::size_t target_node = left_child;
            if (right_child < size_ && compare_(buffer_[right_child], buffer_[left_child])) {
                target_node = right_child;
            }

            if (compare_(buffer_[current_node], buffer_[target_node])) {
                break;
            }

            std::swap(buffer_[current_node], buffer_[target_node]);
            current_node = target_node;
        }
    }

    std::size_t capacity_{4};
    std::size_t size_{0};
    T* buffer_{nullptr};

    Comparator compare_;
};


template <typename T>
class Array {
    friend std::istream& operator >> (std::istream& input, Array<T>& array) {
        for (std::size_t i = 0; i < array.size_; ++i) {
            input >> array.content_[i];
        }

        return input;
    }

 public:
    Array() = default;

    explicit Array(std::size_t size): content_(new T[size]), size_(size) {
    }

    Array(const Array& other) noexcept {
        size_ = other.size_;
        current_iter_ = other.current_iter_;
        content_ = new T[size_];
        std::copy(other.content_, other.content_ + size_, content_);
    }

    Array& operator = (const Array& rhs) {
        if (this == &rhs) {
            return *this;
        }

        delete[] content_;
        size_ = rhs.size_;
        current_iter_ = rhs.current_iter_;
        content_ = new T[size_];
        std::copy(rhs.content_, rhs.content_ + size_, content_);

        return *this;
    }

    Array(Array&& other) noexcept
            : content_(other.content_), current_iter_(other.current_iter_), size_(other.size_) {
        other.content_ = nullptr;
        other.size_ = 0;
        other.current_iter_ = 0;
    }

    Array& operator = (Array&& rhs) noexcept {
        delete[] content_;
        content_ = rhs.content_;
        size_ = rhs.size_;
        current_iter_ = rhs.current_iter_;
        rhs.content_ = nullptr;
        rhs.size_ = 0;
        rhs.current_iter_ = 0;
        return *this;
    }

    ~Array() {
        delete[] content_;
    }

    [[nodiscard]] const T& Head() const {
        if (current_iter_ == size_) {
            throw std::length_error("values in array are over");
        }

        return content_[current_iter_];
    }

    T& Head() {
        if (current_iter_ == size_) {
            throw std::length_error("values in array are over");
        }

        return content_[current_iter_];
    }

    void Next() {
        if (current_iter_ == size_) {
            throw std::length_error("values in array are over");
        }

        ++current_iter_;
    }

    [[nodiscard]] std::size_t Size() const noexcept {
        return size_ - current_iter_;
    }

    [[nodiscard]] std::size_t Empty() const noexcept {
        return current_iter_ == size_;
    }

 private:
    T* content_{nullptr};
    std::size_t current_iter_{0};
    std::size_t size_{0};
};


template <typename T>
struct CompareCurrent {
    bool operator () (const Array<T>& lhs, const Array<T>& rhs) const {
        return lhs.Head() < rhs.Head();
    }
};


void Run(std::istream& input, std::ostream& output) {
    std::size_t arrays_count;
    input >> arrays_count;

    std::size_t result_size = 0;
    auto heap = Heap<Array<int>, CompareCurrent<int>>(arrays_count);
    for (std::size_t i = 0; i < arrays_count; ++i) {
        std::size_t array_size;
        input >> array_size;
        result_size += array_size;

        auto array = Array<int>(array_size);
        input >> array;
        heap.Push(std::move(array));
    }

    auto result = new int[result_size];
    for (std::size_t i = 0; i < result_size; ++i) {
        auto extracted_array = std::move(heap.Top());
        heap.Pop();
        result[i] = extracted_array.Head();
        extracted_array.Next();
        if (!extracted_array.Empty()) {
            heap.Push(std::move(extracted_array));
        }
    }

    for (std::size_t i = 0; i < result_size; ++i) {
        output << result[i] << ' ';
    }
    output << std::endl;

    delete[] result;
}

void TestRun() {
    {
        std::stringstream input, output;
        input << "3\n";
        input << "1\n";
        input << "6\n";
        input << "2\n";
        input << "50 90\n";
        input << "3\n";
        input << "1 10 70\n";

        Run(input, output);
        assert(output.str() == "1 6 10 50 70 90 \n");
    }
    {
        std::stringstream input, output;
        input << "3\n";
        input << "3\n";
        input << "0 3 6\n";
        input << "3\n";
        input << "1 4 7\n";
        input << "3\n";
        input << "2 5 8\n";

        Run(input, output);
        assert(output.str() == "0 1 2 3 4 5 6 7 8 \n");
    }
}


int main() {
    if (DEBUG) {
        TestRun();
    } else {
        Run(std::cin, std::cout);
    }
}