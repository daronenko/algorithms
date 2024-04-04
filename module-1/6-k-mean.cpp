/*
 * Дано множество целых чисел из [0..10^9] размера n.
 * Используя алгоритм поиска k-ой порядковой статистики, требуется найти следующие параметры множества:
 * 10% перцентиль
 * медиана
 * 90% перцентиль
 *
 * Требования:
 * К дополнительной памяти: O(n).
 * Среднее время работы: O(n)
 * Должна быть отдельно выделенная функция partition.
 * Рекурсия запрещена.
 * Решение должно поддерживать передачу функции сравнения снаружи.
 *
 * Формат ввода
 * Сначала вводится кол-во элементов массива n. После сам массив.
 *
 * Формат вывода
 * Параметры множества в порядке:
 * 10% перцентиль
 * медиана
 * 90% перцентиль
 *
 * Пример
 * Ввод
 * 10
 * 1 2 3 4 5 6 7 8 9 10
 * Вывод
 * 2
 * 6
 * 10
 */


#include <iostream>
#include <sstream>
#include <cassert>
#include <functional>
#include <iterator>
#include <random>


const bool DEBUG = true;


template <typename BidirIt, typename Comparator = std::less<>>
BidirIt Partition(BidirIt first, BidirIt last, Comparator compare = Comparator{}) {
    if (last - first < 1) {
        throw std::logic_error("passed invalid values of iterators");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, std::distance(first, last) - 1);

    std::size_t pivot_idx = distrib(gen);
    auto pivot_value = *std::next(first, pivot_idx);
    std::iter_swap(first, std::next(first, pivot_idx));

    auto left_iter = std::prev(last);
    auto right_iter = std::prev(last);
    while (left_iter > first) {
        if (compare(*left_iter, pivot_value)) {
            std::advance(left_iter, -1);
        } else {
            std::iter_swap(left_iter, right_iter);
            std::advance(left_iter, -1);
            std::advance(right_iter, -1);
        }
    }

    std::iter_swap(first, right_iter);
    return right_iter;
}


template <typename BidirIt, typename Comparator = std::less<>>
BidirIt QuickSelect(BidirIt first, BidirIt last, std::size_t k, Comparator compare = Comparator{}) {
    auto left = first;
    auto right = last;
    while (left < right) {
        auto pivot = Partition(left, right, compare);
        std::size_t pivot_idx = pivot - first;

        if (pivot_idx == k) {
            return pivot;
        }

        if (pivot_idx < k) {
            left = pivot + 1;
        } else {
            right = pivot;
        }
    }

    return last;
}


void Run(std::istream& input, std::ostream& output) {
    std::size_t size;
    input >> size;

    auto array = new std::uint32_t[size]();
    for (std::size_t i = 0; i < size; ++i) {
        input >> array[i];
    }

    output << *QuickSelect(array, array + size, size * 10 / 100) << std::endl;
    output << *QuickSelect(array, array + size, size * 50 / 100) << std::endl;
    output << *QuickSelect(array, array + size, size * 90 / 100) << std::endl;

    delete[] array;
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "10\n";
        input << "1 2 3 4 5 6 7 8 9 10\n";

        Run(input, output);
        assert(output.str() == "2\n6\n10\n");
    }
    {
        std::stringstream input, output;

        input << "10\n";
        input << "10 9 8 7 6 5 4 3 2 1\n";

        Run(input, output);
        assert(output.str() == "2\n6\n10\n");
    }
}


int main() {
    if (DEBUG) {
        TestRun();
    } else {
        Run(std::cin, std::cout);
    }
}