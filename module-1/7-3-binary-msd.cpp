/*
 * Дан массив неотрицательных целых 64-разрядных чисел. Количество чисел не больше 1000000. Отсортировать массив методом
 * MSD по битам (бинарный QuickSort).
 *
 * Пример
 * Ввод
 * 3
 * 4 1000000 7
 * Вывод
 * 4 7 1000000
 */


#include <iostream>
#include <sstream>
#include <cassert>
#include <iterator>
#include <functional>
#include <cstdint>


const bool DEBUG = false;


bool GetBit(std::uint64_t number, std::uint8_t position) {
    return (number >> position) & 1;
}

template <typename ForwardIt, typename Comparator = std::less<>>
void BinaryMSD(ForwardIt first, ForwardIt last, Comparator compare = Comparator{}, std::int8_t bit = 63) {
    if (first >= last || bit < 0) {
        return;
    }

    auto left = first;
    auto right = std::prev(last);

    while (left < right) {
        if (!compare(GetBit(*left, bit), GetBit(*right, bit))) {
            std::iter_swap(left, right);
        }

        if (GetBit(*left, bit) == !compare(0, 1)) {
            left = std::next(left);
        }

        if (GetBit(*right, bit) == compare(0, 1)) {
            right = std::prev(right);
        }
    }

    if (GetBit(*left, bit) == !compare(0, 1)) {
        left = std::next(left);
    }

    BinaryMSD(first, left, compare, bit - 1);
    BinaryMSD(left, last, compare, bit - 1);
}


void Run(std::istream& input, std::ostream& output) {
    std::size_t size;
    input >> size;

    auto array = new std::uint64_t[size];
    for (std::size_t i = 0; i < size; ++i) {
        input >> array[i];
    }

    BinaryMSD(array, array + size);

    for (std::size_t i = 0; i < size; ++i) {
        output << array[i] << ' ';
    }
    output << std::endl;

    delete[] array;
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "3\n";
        input << "4 1000000 7\n";

        Run(input, output);
        assert(output.str() == "4 7 1000000 \n");
    }
    {
        std::stringstream input, output;

        input << "5\n";
        input << "5 4 3 2 1\n";

        Run(input, output);
        assert(output.str() == "1 2 3 4 5 \n");
    }
}


int main() {
    if (DEBUG) {
        TestRun();
    } else {
        Run(std::cin, std::cout);
    }
}