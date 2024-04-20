#include <iostream>
#include <sstream>
#include <cassert>


const bool DEBUG = false;


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


void Run(std::istream& input, std::ostream& output) {
    std::size_t size;
    input >> size;

    auto array = new int[size];
    for (std::size_t i = 0; i < size; ++i) {
        input >> array[i];
    }

    auto start = FindMinValue(array, array + size);
    output << std::distance(array, start) << std::endl;

    delete[] array;
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "8\n";
        input << "12 14 16 17 1 3 5 7\n";

        Run(input, output);
        assert(output.str() == "4\n");
    }
    {
        std::stringstream input, output;

        input << "6\n";
        input << "5 5 5 5 5 1\n";

        Run(input, output);
        assert(output.str() == "5\n");
    }
}


int main() {
    if (DEBUG) {
        TestRun();
    } else {
        Run(std::cin, std::cout);
    }
}