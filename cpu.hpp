#pragma once

#include "memory.hpp"

class CPU {
public:
    uint8_t A{};   // аккумулятор
    uint8_t X{};
    uint8_t Y{};
    uint16_t PC{};
    uint8_t status{}; // P регистр

    Memory& memory;

    explicit CPU(Memory& mem) : memory(mem) {}

    void step();

private:
    void LDA(uint8_t value) noexcept;

    uint8_t fetchImmediate();
    uint8_t fetchZeroPage();
    uint8_t fetchZeroPageX();
    uint8_t fetchAbsolute();
    uint8_t fetchAbsoluteX(bool& pageCrossed);
    uint8_t fetchAbsoluteY(bool& pageCrossed);
    uint8_t fetchIndirectX();
    uint8_t fetchIndirectY(bool& pageCrossed);

    void setZN(uint8_t value) noexcept;
};
