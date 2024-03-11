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
 * Ввод	Вывод
 * 1
 * 1
 *
 * Пример 2
 * Ввод	Вывод
 * 5
 * 2
 */


#include <iostream>
#include <cstdint>


std::uint8_t CountBits(std::uint32_t number) {
    std::uint8_t bits_count = 0;
    for (; number; number >>= 2) {
        if (number & 1) {
            ++bits_count;
        }
    }

    return bits_count;
}


void run(std::istream& input, std::ostream& output) {
    std::uint32_t number;
    input >> number;

    // `+` is needed to print number instead of character
    output << +CountBits(number) << std::endl;
}


int main() {
    run(std::cin, std::cout);
    return 0;
}