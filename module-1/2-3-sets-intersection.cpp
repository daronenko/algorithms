/*
 * Даны два массива неповторяющихся целых чисел, упорядоченные по возрастанию. A[0..n-1] и B[0..m-1]. n » m. Найдите их
 * пересечение. Требуемое время работы: O(m * log k), где k - позиция элементта B[m-1] в массиве A. В процессе поиска
 * очередного элемента B[i] в массиве A пользуйтесь результатом поиска элемента B[i-1]. n, k ≤ 10000.
 *
 * Пример
 * Ввод
 * 5
 * 3
 * 1 2 3 4 5
 * 1 3 5
 * Вывод
 * 1 3 5
 */


#include <iostream>
#include <utility>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <cstdint>


constexpr bool DEBUG = false;


template <typename ForwardIt, typename T>
std::pair<ForwardIt, ForwardIt> GetBounds(ForwardIt first, ForwardIt last, const T& target) {
    assert(std::distance(first, last) >= 0);
    assert(std::is_sorted(first, last));

    auto right = std::next(first);
    while (right < last && *right <= target) {
        std::size_t dist = std::distance(first, right);
        first = right;
        std::advance(right, 2 * dist);
    }

    if (right > last) {
        right = last;
    }

    return {first, right};
}


template <typename ForwardIt, typename T>
ForwardIt LowerBound(ForwardIt first, ForwardIt last, const T& target) {
    assert(std::distance(first, last) >= 0);
    assert(std::is_sorted(first, last));

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


void Run(std::istream& input, std::ostream& output) {
    std::uint16_t a_length, b_length;
    input >> a_length >> b_length;

    int a[a_length];
    for (std::size_t i = 0; i < a_length; ++i) {
        input >> a[i];
    }

    int b[b_length];
    for (std::size_t i = 0; i < b_length; ++i) {
        input >> b[i];
    }

    auto left = a;
    auto right = a + a_length;
    for (std::size_t i = 0; i < b_length; ++i) {
        auto [new_left, new_right] = GetBounds(left, right, b[i]);
        auto result = LowerBound(new_left, new_right, b[i]);
        if (*result == b[i]) {
            output << *result << ' ';
        }
        left = result;
    }
    output << std::endl;
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "5\n";
        input << "3\n";
        input << "1 2 3 4 5\n";
        input << "1 3 5\n";

        Run(input, output);
        assert(output.str() == "1 3 5 \n");
    }
    {
        std::stringstream input, output;

        input << "1\n";
        input << "5\n";
        input << "1\n";
        input << "1 2 3 4 5\n";

        Run(input, output);
        assert(output.str() == "1 \n");
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