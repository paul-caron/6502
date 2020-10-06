#include "cpu6502.hpp"
#include <cstdint>
#include <iostream>

int main(){
    cpu6502 cpu = cpu6502();
    uint16_t y =1;
    int8_t x = 0b10000000;
    std::cout << y + x;
    return 0;
}
