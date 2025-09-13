#pragma once

#include <cstdint>

enum class Opcode : uint8_t {
    LDA_Immediate = 0xA9,
    LDA_ZeroPage = 0xA5,
    LDA_ZeroPageX = 0xB5,
    LDA_Absolute = 0xAD,
    LDA_AbsoluteX = 0xBD,
    LDA_AbsoluteY = 0xB9,
    LDA_IndirectX = 0xA1,
    LDA_IndirectY = 0xB1,
};
