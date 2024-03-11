/*
 * Инвертируйте значение бита в числе N по его номеру K.
 * Необходимо использование битовых операций.
 * Использование арифметических операций запрещено.
 *
 * Формат ввода
 * Сначала вводится число N.
 * Оно лежит в диапазоне 0..2^32-1 и вводится в десятичном виде.
 * Далее вводится номер бита K.
 * Лежит в диапазоне 0..31 и вводится в десятичном виде.
 *
 * Формат вывода
 * Выходное число выводится в десятичном виде.
 *
 * Пример
 * Ввод
 * 1 0
 * Вывод
 * 0
 */


#include <iostream>
#include <cstdint>


std::uint32_t ToggleKthBit(std::uint32_t number, std::uint16_t k) {
    return number ^ (1 << k);
}


void Run(std::istream& input, std::ostream& output) {
    std::uint32_t number;
    std::uint16_t k;
    input >> number >> k;

    output << ToggleKthBit(number, k) << std::endl;
}


int main() {
    Run(std::cin, std::cout);
    return 0;
}