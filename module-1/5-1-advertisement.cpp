/*
 * В супермаркете решили оптимизировать показ рекламы. Известно расписание прихода и ухода покупателей (два целых
 * числа). Каждому покупателю необходимо показать минимум 2 рекламы. Рекламу можно транслировать только в целочисленные
 * моменты времени. Покупатель может видеть рекламу от момента прихода до момента ухода из магазина. В каждый момент
 * времени может показываться только одна реклама. Считается, что реклама показывается мгновенно. Если реклама
 * показывается в момент ухода или прихода, то считается, что посетитель успел её посмотреть.
 * Требуется определить минимальное число показов рекламы.
 *
 * Пример
 * Ввод
 * 5
 * 1 10
 * 10 12
 * 1 10
 * 1 10
 * 23 24
 * Вывод
 * 5
 */


#include <sstream>
#include <iostream>
#include <cassert>
#include <iterator>
#include <functional>
#include <algorithm>


const bool DEBUG = true;


template <typename ForwardIt, typename Comparator = std::less<>>
void InplaceMerge(ForwardIt first, ForwardIt middle, ForwardIt last, Comparator compare = Comparator{}) {
    using ValueType = typename std::iterator_traits<ForwardIt>::value_type;

    auto temp_storage = new ValueType[std::distance(first, last)];
    auto temp_storage_iter = temp_storage;

    auto left_iter = first;
    auto right_iter = middle;

    while (left_iter < middle && right_iter < last) {
        if (compare(*left_iter, *right_iter)) {
            *temp_storage_iter = std::move(*left_iter);
            left_iter = std::next(left_iter);
        } else {
            *temp_storage_iter = std::move(*right_iter);
            right_iter = std::next(right_iter);
        }
        temp_storage_iter = std::next(temp_storage_iter);
    }

    while (left_iter < middle) {
        *temp_storage_iter = std::move(*left_iter);
        left_iter = std::next(left_iter);
        temp_storage_iter = std::next(temp_storage_iter);
    }

    while (right_iter < last) {
        *temp_storage_iter = std::move(*right_iter);
        right_iter = std::next(right_iter);
        temp_storage_iter = std::next(temp_storage_iter);
    }

    std::copy(temp_storage, temp_storage_iter, first);

    delete[] temp_storage;
}

// recursive merge sort
// template <typename ForwardIt, typename Comparator = std::less<>>
// void MergeSort(ForwardIt first, ForwardIt last, Comparator compare = Comparator{}) {
//     const std::size_t size = std::distance(first, last);
//
//     if (size < 2) {
//         return;
//     }
//
//     const auto middle = std::next(first, size / 2);
//     MergeSort(first, middle, compare);
//     MergeSort(middle, last, compare);
//     InplaceMerge(first, middle, last, compare);
// }

// iterative merge sort
template <typename ForwardIt, typename Comparator = std::less<>>
void MergeSort(ForwardIt first, ForwardIt last, Comparator compare = Comparator{}) {
    const std::size_t size = std::distance(first, last);

    if (size < 2) {
        return;
    }

    for (std::size_t i = 1; i < size; i *= 2) {
        for (std::size_t j = 0; j < size - i; j += 2 * i) {
            InplaceMerge(first + j, first + j + i, first + std::min(j + 2 * i, size), compare);
        }
    }
}


struct TimeInterval {
    std::size_t begin, end;
};

struct CompareIntervalsEnd {
    bool operator () (const TimeInterval& lhs, const TimeInterval& rhs) const {
        if (lhs.end == rhs.end) {
            return lhs.begin < rhs.begin;
        }

        return lhs.end < rhs.end;
    }
};

std::istream& operator >> (std::istream& input, TimeInterval& interval) {
    input >> interval.begin >> interval.end;
    return input;
}

std::ostream& operator << (std::ostream& output, const TimeInterval& interval) {
    output << "TimeInterval(" << interval.begin << ", " << interval.end << ')';
    return output;
}

bool operator == (const TimeInterval& lhs, const TimeInterval& rhs) {
    return lhs.begin == rhs.begin && lhs.end == rhs.end;
}


void Run(std::istream& input, std::ostream& output) {
    std::size_t intervals_count;
    input >> intervals_count;

    auto intervals = new TimeInterval[intervals_count];
    for (std::size_t i = 0; i < intervals_count; ++i) {
        input >> intervals[i];
    }

    MergeSort(intervals, intervals + intervals_count, CompareIntervalsEnd{});

    std::size_t ads_count = 2;
    std::size_t first_ad_time = intervals[0].end - 1;
    std::size_t second_ad_time = intervals[0].end;
    for (std::size_t i = 1; i < intervals_count; ++i) {
        if (intervals[i].begin == second_ad_time) {
            ++ads_count;
            first_ad_time = second_ad_time;
            second_ad_time = intervals[i].end;
        } else if (intervals[i].begin > second_ad_time) {
            ads_count += 2;
            first_ad_time = intervals[i].end - 1;
            second_ad_time = intervals[i].end;
        } else if (intervals[i].begin > first_ad_time
                  && intervals[i].begin < second_ad_time) {
            ++ads_count;
            first_ad_time = intervals[i].end;
        } else if (intervals[i].begin > first_ad_time
                && intervals[i].end > second_ad_time) {
            ++ads_count;
            first_ad_time = second_ad_time;
            second_ad_time = intervals[i].end;
        }
    }

    output << ads_count << std::endl;

    delete[] intervals;
}

void TestRun() {
    {
        std::stringstream input, output;
        input << "5\n";
        input << "1 10\n";
        input << "10 12\n";
        input << "1 10\n";
        input << "1 10\n";
        input << "23 24\n";

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