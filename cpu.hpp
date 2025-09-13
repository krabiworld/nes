#pragma once
#include "memory.hpp"

class CPU {
public:
    struct Instruction {
        const char* name;
        void (CPU::*operate)(uint8_t);
        void (CPU::*operateNoArg)();
        uint8_t (CPU::*fetch)();
        int bytes;
        int cycles;
    };

    uint8_t A{};
    uint8_t X{};
    uint8_t Y{};
    uint16_t PC{};
    uint8_t P{};
    int cycles{};

    Memory& memory;

    explicit CPU(Memory& mem) : memory(mem) {}

    void step();

private:
    static Instruction table[256];

    bool pageCrossed{};

    void AND(uint8_t value) noexcept;
    void LDA(uint8_t value) noexcept;

    uint8_t fetchImmediate();
    uint8_t fetchZeroPage();
    uint8_t fetchZeroPageX();
    uint8_t fetchAbsolute();
    uint8_t fetchAbsoluteX();
    uint8_t fetchAbsoluteY();
    uint8_t fetchIndirectX();
    uint8_t fetchIndirectY();

    void setZN(uint8_t value) noexcept;

    static void initTable() noexcept;
};
