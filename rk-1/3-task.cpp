#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdint>
#include <functional>
#include <cmath>


const bool DEBUG = false;


struct Point {
    std::uint32_t x, y;
};

struct ComparePoints {
    bool operator () (const Point& lhs, const Point& rhs) const {
        return lhs.x < rhs.x || lhs.y < rhs.y;
    }
};

std::istream& operator >> (std::istream& input, Point& point) {
    input >> point.x >> point.y;
    return input;
}

std::ostream& operator << (std::ostream& output, const Point& point) {
    output << point.x << ' ' << point.y;
    return output;
}

template <typename ForwardIt, typename Comparator = std::less<>>
void InsertionSort(ForwardIt first, ForwardIt last, Comparator compare = Comparator{}) {
    for (auto iter = std::next(first); iter < last; iter = std::next(iter)) {
        auto temp_value = std::move(*iter);
        auto reversed_iter = iter;

        while (reversed_iter > first && compare(temp_value, *std::prev(reversed_iter))) {
            std::iter_swap(reversed_iter, std::prev(reversed_iter));
            reversed_iter = std::prev(reversed_iter);
        }

        *reversed_iter = std::move(temp_value);
    }
}


float Angle(const Point& first, const Point& second) {
    return atan2(first.x - second.x, first.y - second.y);
}


void Run(std::istream& input, std::ostream& output) {
    std::size_t size;
    input >> size;

    auto points = new Point[size];
    for (std::size_t i = 0; i < size; ++i) {
        input >> points[i];
    }

    InsertionSort(points, points + size, ComparePoints{});

    auto min_point = points[0];

    InsertionSort(points + 1, points + size, [&min_point](const Point& lhs, const Point& rhs){
        return Angle(lhs, min_point) <= Angle(rhs, min_point);
    });

    for (std::size_t i = 0; i < size; ++i) {
        output << points[i] << std::endl;
    }

    delete[] points;
}

void TestRun() {
    {
        std::stringstream input, output;

        input << "4\n";
        input << "0 0\n";
        input << "1 1\n";
        input << "1 0\n";
        input << "0 1\n";

        Run(input, output);
        assert(output.str() == "0 0\n0 1\n1 1\n1 0\n");
    }
}


int main() {
    if (DEBUG) {
        TestRun();
    } else {
        Run(std::cin, std::cout);
    }
}