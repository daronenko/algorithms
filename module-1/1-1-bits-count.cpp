/*
 * Подсчитать кол-во единичных бит в входном числе , стоящих на четных позициях.
 * Позиции битов нумеруются с 0.
 * Необходимо использование битовых операций.
 * Использование арифметических операций запрещено.
 *
 * Формат ввода
 * Входное число лежит в диапазоне 0..2^32-1 и вводится в десятичном виде.
 *
 * Формат вывода
 * Целое число - количество бит
 *
 * Пример 1
 * Ввод
 * 1
 * Вывод
 * 1
 *
 * Пример 2
 * Ввод
 * 5
 * Вывод
 * 2
 */


#include <iostream>
#include <cstdint>


std::uint16_t CountBits(std::uint32_t number) {
    std::uint16_t bits_count = 0;
    for (; number; number >>= 2) {
        if (number & 1) {
            ++bits_count;
        }
    }

    return bits_count;
}


void Run(std::istream& input, std::ostream& output) {
    std::uint32_t number;
    input >> number;

    output << CountBits(number) << std::endl;
}


int main() {
    Run(std::cin, std::cout);
    return 0;
}