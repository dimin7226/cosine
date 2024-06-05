#include "fun.h"

void print_bits(char number) {
    union type_char bits;
    bits.number = number;
    printf("Двоичное представление младших 4 битов: %d%d%d%d\n", bits.bits.bit4, bits.bits.bit3, bits.bits.bit2, bits.bits.bit1);
}

void task1() {
    char num;
    printf("Введите число: ");
    scanf_s("%hhd", &num);
    print_bits(num);
}

