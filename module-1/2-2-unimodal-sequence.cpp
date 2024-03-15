/*
 * Дан массив целых чисел А[0..n-1]. Известно, что на интервале [0, m] значения массива строго возрастают,
 * а на интервале [m, n-1] строго убывают. Найти m за O( log m ). 2 ≤ n ≤ 10000.
 *
 * Пример
 * Ввод
 * 10
 * 1 2 3 4 5 6 7 6 5 4
 * Вывод
 * 6
 */


#include <iostream>
#include <utility>
#include <iterator>
#include <sstream>
#include <cassert>
#include <cstdint>


constexpr bool DEBUG = true;


template <typename InputIt>
std::pair<InputIt, InputIt> GetBounds(InputIt first, InputIt last) {
    assert(std::distance(first, last) > 0);

    if (std::distance(first, last) == 1) {
        return {first, last};
    }

    auto right = std::next(first);
    while (right < last) {
        std::size_t dist = std::distance(first, right);
        if (*std::prev(right) > *right) {
            break;
        }

        first = right;
        std::advance(right, 2 * dist);
    }

    if (right > last) {
        right = last;
    }

    return {first, right};
}

void TestGetBounds() {
    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 6, 5, 4};
    auto [left, right] = GetBounds(std::begin(numbers), std::end(numbers));
    assert(*left == 4);
    assert(std::distance(left, right) == 4);
}


template <typename ForwardIt>
ForwardIt FindPeak(ForwardIt first, ForwardIt last) {
    assert(std::distance(first, last) > 0);

    if (std::distance(first, last) == 1) {
        return first;
    }

    while (first < last) {
        auto middle = std::next(first, std::distance(first, last) / 2);
        if (std::next(middle) < last && *middle < *std::next(middle)) {
            first = std::next(middle);
        } else if (std::prev(middle) >= first && *std::prev(middle) < *middle) {
            first = middle;
        } else {
            last = middle;
        }
    }

    return first;
}

void TestFindPeak() {
    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 6, 5, 4};
    auto peak = FindPeak(std::begin(numbers), std::end(numbers));
    assert(*peak == 7);
}


void Run(std::istream& input, std::ostream& output) {
    std::uint16_t length;
    input >> length;

    int sequence[length];
    for (std::size_t i = 0; i < length; ++i) {
        input >> sequence[i];
    }

    auto [left, right] = GetBounds(sequence, sequence + length);
    output << FindPeak(left, right) - sequence << std::endl;
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "10\n";
        input << "1 2 3 4 5 6 7 6 5 4\n";

        Run(input, output);
        assert(output.str() == "6\n");
    }
    {
        std::stringstream input, output;

        input << "7\n";
        input << "1 2 5 4 3 2 1\n";

        Run(input, output);
        assert(output.str() == "2\n");
    }
    {
        std::stringstream input, output;

        input << "3\n";
        input << "1 2 1\n";

        Run(input, output);
        assert(output.str() == "1\n");
    }
    {
        std::stringstream input, output;

        input << "3\n";
        input << "1 2 3\n";

        Run(input, output);
        assert(output.str() == "2\n");
    }
}


int main() {
    if (DEBUG) {
        TestGetBounds();
        TestFindPeak();
        TestRun();
    } else {
        Run(std::cin, std::cout);
    }

    return 0;
}