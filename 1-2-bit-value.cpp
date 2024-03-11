/*
 * Вернуть значение бита в числе N по его номеру K.
 * Позиции битов нумеруются с 0.
 * Необходимо использование битовых операций.
 * Использование арифметических операций запрещено.
 *
 * Формат ввода
 * Сначала вводится число N.
 * Оно лежит в диапазоне 0..232-1 и вводится в десятичном виде.
 * Далее вводится номер бита K.
 * Лежит в диапазоне 0..31 и вводится в десятичном виде.
 *
 * Формат вывода
 * Целое число - значение бита
 *
 * Пример 1
 * Ввод
 * 1 0
 * Вывод
 * 1
 *
 * Пример 2
 * Ввод
 * 1 1
 * Вывод
 * 0
 */


#include <iostream>
#include <cstdint>


std::uint16_t GetKthBit(std::uint32_t number, std::uint16_t k) {
    return (number >> k) & 1;
}


void run(std::istream& input, std::ostream& output) {
    std::uint32_t number;
    std::uint16_t k;
    input >> number >> k;

    output << GetKthBit(number, k) << std::endl;
}


int main() {
    run(std::cin, std::cout);
    return 0;
}