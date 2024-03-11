/*
 * Если в числе содержится только один бит со значением 1, записать в выходной поток OK.
 * Иначе записать FAIL.
 * Необходимо использование битовых операций.
 * Использование арифметических операций запрещено.
 *
 * Формат ввода
 * Входное число лежит в диапазоне 0..2^32-1 и вводится в десятичном виде.
 *
 * Формат вывода
 * Строка OK или FAIL
 *
 * Пример 1
 * Ввод
 * 0
 * Вывод
 * FAIL
 *
 * Пример 2
 * Ввод
 * 1
 * Вывод
 * OK
 */


#include <iostream>
#include <cstdint>


bool ContainsOneBit(std::uint32_t number) {
    std::uint8_t bits_count = 0;
    for (; number; number >>= 1) {
        bits_count += (number & 1);
    }

    return bits_count == 1;
}


void Run(std::istream& input, std::ostream& output) {
    std::uint32_t number;
    input >> number;

    output << (ContainsOneBit(number) ? "OK" : "FAIL") << std::endl;
}


int main() {
    Run(std::cin, std::cout);
    return 0;
}