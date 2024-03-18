/*
 * Дан отсортированный массив различных целых чисел A[0..n-1] и массив целых чисел B[0..m-1]. Для каждого элемента
 * массива B[i] найдите минимальный индекс элемента массива A[k], ближайшего по значению к B[i]. n ≤ 110000, m ≤ 1000.
 * Время работы поиска для каждого элемента B[i]: O(log(k)).
 *
 * Пример 1
 * Ввод
 * 3
 * 10 20 30
 * 3
 * 9 15 35
 * Вывод
 * 0 0 2
 *
 * Пример 2
 * Ввод
 * 3
 * 10 20 30
 * 4
 * 8 9 10 32
 * Вывод
 * 0 0 0 2
 */


#include <iostream>
#include <utility>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <cstdint>


constexpr bool DEBUG = true;


template <typename ForwardIt, typename T>
std::pair<ForwardIt, ForwardIt> GetBounds(ForwardIt first, ForwardIt last, const T& target) {
     // assert(std::distance(first, last) >= 0);
     // assert(std::is_sorted(first, last));

    auto right = std::next(first);
    while (right < last && *right <= target) {
        std::size_t dist = std::distance(first, right);
        first = right;
        std::advance(right, 2 * dist);
    }

    right = std::next(right);
    if (right > last) {
        right = last;
    }

    return {first, right};
}


template <typename BidirIt, typename T>
BidirIt ClosestValue(BidirIt first, BidirIt last, const T& target) {
     // assert(std::distance(first, last) >= 0);
     // assert(std::is_sorted(first, last));

    auto left = first;
    auto right = last;
    while (left < right) {
        auto middle = std::next(left, std::distance(left, right) / 2);

        if (*middle < target) {
            left = std::next(middle);
        } else {
            right = middle;
        }
    }

    if ((std::prev(left) >= first && target - *std::prev(left) <= *left - target) ||
            (left == last)) {
        return std::prev(left);
    } else {
        return left;
    }
}


void Run(std::istream& input, std::ostream& output) {
    std::uint32_t a_length;
    input >> a_length;

    int a[a_length];
    for (std::size_t i = 0; i < a_length; ++i) {
        input >> a[i];
    }

    std::uint16_t  b_length;
    input >> b_length;

    int b[b_length];
    for (std::size_t i = 0; i < b_length; ++i) {
        input >> b[i];
    }

    for (std::size_t i = 0; i < b_length; ++i) {
        auto [left, right] = GetBounds(a, a + a_length, b[i]);
        auto result = ClosestValue(left, right, b[i]);
        output << result - a << ' ';
    }
    output << std::endl;
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "3\n";
        input << "10 20 30\n";
        input << "3\n";
        input << "9 15 35\n";

        Run(input, output);
        assert(output.str() == "0 0 2 \n");
    }
    {
        std::stringstream input, output;

        input << "3\n";
        input << "10 20 30\n";
        input << "4\n";
        input << "8 9 10 32\n";

        Run(input, output);
        assert(output.str() == "0 0 0 2 \n");
    }
    {
        std::stringstream input, output;

        input << "7\n";
        input << "1 2 4 5 6 8 9\n";
        input << "1\n";
        input << "11\n";

        Run(input, output);
        assert(output.str() == "6 \n");
    }
    {
        std::stringstream input, output;

        input << "6\n";
        input << "2 5 6 7 8 9\n";
        input << "1\n";
        input << "4\n";

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