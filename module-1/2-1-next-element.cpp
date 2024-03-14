/*
 * Дан отсортированный массив целых чисел A[0..n-1] и массив целых чисел B[0..m-1]. Для каждого элемента массива B[i]
 * найдите минимальный индекс k минимального элемента массива A, равного или превосходящего B[i]: A[k] >= B[i]. Если
 * такого элемента нет, выведите n. Время работы поиска k для каждого элемента B[i]: O(log(k)). n, m ≤ 10000.
 *
 * Формат ввода
 * В первой строчке записаны числа n и m. Во второй и третьей массивы A и B соответственно.
 *
 * Пример 1
 * Ввод
 * 2 1
 * 1 2
 * 2
 * Вывод
 * 1
 *
 * Пример 2
 * Ввод
 * 4 3
 * 2 4 5 7
 * 4 6 1
 * Вывод
 * 1 3 0
 */


#include <iostream>
#include <utility>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <cstdint>


constexpr bool DEBUG = false;


template <typename InputIt, typename T>
std::pair<InputIt, InputIt> GetBounds(InputIt first, InputIt last, const T& target) {
    assert(std::distance(first, last) > 0);
    assert(std::is_sorted(first, last));

    auto right = std::next(first);
    while (right < last && *right <= target) {
        std::size_t dist = std::distance(first, right);
        first = right;
        if (std::next(right, dist) <= last) {
            std::advance(right, dist);
        } else {
            right = last;
        }
    }

    return {first, right};
}

void TestGetBounds() {
    int numbers[] = {2, 4, 5, 7};
    auto [left, right] = GetBounds(std::begin(numbers), std::end(numbers), 6);
    assert(*left == 5 && *right == 7);
}


template <typename ForwardIt, typename T>
ForwardIt LowerBound(ForwardIt first, ForwardIt last, const T& target) {
    assert(std::distance(first, last) > 0);
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

void TestLowerBound() {
    int numbers[] = {2, 4, 5, 7};
    auto bound = LowerBound(std::begin(numbers), std::end(numbers), 3);
    assert(*bound == 4);
}


void Run(std::istream& input, std::ostream& output) {
    std::uint16_t n, m;
    input >> n >> m;

    int a[n];
    for (std::size_t i = 0; i < n; ++i) {
        input >> a[i];
    }

    int b[m];
    for (std::size_t i = 0; i < m; ++i) {
        input >> b[i];
    }

    for (auto number : b) {
        auto [left, right] = GetBounds(a, a + n, number);
        output << LowerBound(left, right, number) - a << ' ';
    }
    output << std::endl;
}

void TestRun() {
    {
        std::stringstream in, out;

        in << "2 1\n";
        in << "1 2\n";
        in << "2\n";

        Run(in, out);
        assert(out.str() == "1 \n");
    }
    {
        std::stringstream in, out;

        in << "4 3\n";
        in << "2 4 5 7\n";
        in << "4 6 1\n";

        Run(in, out);
        assert(out.str() == "1 3 0 \n");
    }
}


int main() {
    if (DEBUG) {
        TestGetBounds();
        TestLowerBound();
        TestRun();
    } else {
        Run(std::cin, std::cout);
    }

    return 0;
}