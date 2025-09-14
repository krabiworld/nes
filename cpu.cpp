#include "cpu.hpp"
#include <iostream>

CPU::Instruction CPU::table[256];

void CPU::initTable() noexcept {
    for (auto &i: table) {
        i = {"???", nullptr, nullptr, nullptr, 1, 2};
    }

    table[0x0] = {"BRK", nullptr, &CPU::BRK, nullptr, 2, 7};

    table[0x09] = {"ORA", &CPU::ORA, nullptr, &CPU::fetchImmediate, 2, 2};
    table[0x05] = {"ORA", &CPU::ORA, nullptr, &CPU::fetchZeroPage, 2, 3};
    table[0x15] = {"ORA", &CPU::ORA, nullptr, &CPU::fetchZeroPageX, 2, 4};
    table[0x0D] = {"ORA", &CPU::ORA, nullptr, &CPU::fetchAbsolute, 3, 4};
    table[0x1D] = {"ORA", &CPU::ORA, nullptr, &CPU::fetchAbsoluteX, 3, 4};
    table[0x19] = {"ORA", &CPU::ORA, nullptr, &CPU::fetchAbsoluteY, 3, 4};
    table[0x01] = {"ORA", &CPU::ORA, nullptr, &CPU::fetchIndirectX, 2, 6};
    table[0x11] = {"ORA", &CPU::ORA, nullptr, &CPU::fetchIndirectY, 2, 5};

    table[0x29] = {"AND", &CPU::AND, nullptr, &CPU::fetchImmediate, 2, 2};
    table[0x25] = {"AND", &CPU::AND, nullptr, &CPU::fetchZeroPage, 2, 3};
    table[0x35] = {"AND", &CPU::AND, nullptr, &CPU::fetchZeroPageX, 2, 4};
    table[0x2D] = {"AND", &CPU::AND, nullptr, &CPU::fetchAbsolute, 3, 4};
    table[0x3D] = {"AND", &CPU::AND, nullptr, &CPU::fetchAbsoluteX, 3, 4};
    table[0x39] = {"AND", &CPU::AND, nullptr, &CPU::fetchAbsoluteY, 3, 4};
    table[0x21] = {"AND", &CPU::AND, nullptr, &CPU::fetchIndirectX, 2, 6};
    table[0x31] = {"AND", &CPU::AND, nullptr, &CPU::fetchIndirectY, 2, 5};

    table[0x85] = {"STA", &CPU::STA, nullptr, &CPU::fetchZeroPage, 2, 3};
    table[0x95] = {"STA", &CPU::STA, nullptr, &CPU::fetchZeroPageX, 2, 4};
    table[0x8D] = {"STA", &CPU::STA, nullptr, &CPU::fetchAbsolute, 3, 4};
    table[0x9D] = {"STA", &CPU::STA, nullptr, &CPU::fetchAbsoluteX, 3, 5};
    table[0x99] = {"STA", &CPU::STA, nullptr, &CPU::fetchAbsoluteY, 3, 5};
    table[0x81] = {"STA", &CPU::STA, nullptr, &CPU::fetchIndirectX, 2, 6};
    table[0x91] = {"STA", &CPU::STA, nullptr, &CPU::fetchIndirectY, 2, 6};

    table[0xA9] = {"LDA", &CPU::LDA, nullptr, &CPU::fetchImmediate, 2, 2};
    table[0xA5] = {"LDA", &CPU::LDA, nullptr, &CPU::fetchZeroPage, 2, 3};
    table[0xB5] = {"LDA", &CPU::LDA, nullptr, &CPU::fetchZeroPageX, 2, 4};
    table[0xAD] = {"LDA", &CPU::LDA, nullptr, &CPU::fetchAbsolute, 3, 4};
    table[0xBD] = {"LDA", &CPU::LDA, nullptr, &CPU::fetchAbsoluteX, 3, 4};
    table[0xB9] = {"LDA", &CPU::LDA, nullptr, &CPU::fetchAbsoluteY, 3, 4};
    table[0xA1] = {"LDA", &CPU::LDA, nullptr, &CPU::fetchIndirectX, 2, 6};
    table[0xB1] = {"LDA", &CPU::LDA, nullptr, &CPU::fetchIndirectY, 2, 5};

    table[0xA2] = {"LDX", &CPU::LDX, nullptr, &CPU::fetchImmediate, 2, 2};
    table[0xA6] = {"LDX", &CPU::LDX, nullptr, &CPU::fetchZeroPage, 2, 3};
    table[0xB6] = {"LDX", &CPU::LDX, nullptr, &CPU::fetchZeroPageY, 2, 4};
    table[0xAE] = {"LDX", &CPU::LDX, nullptr, &CPU::fetchAbsolute, 3, 4};
    table[0xBE] = {"LDX", &CPU::LDX, nullptr, &CPU::fetchAbsoluteY, 3, 4};

    table[0xA0] = {"LDY", &CPU::LDY, nullptr, &CPU::fetchImmediate, 2, 2};
    table[0xA4] = {"LDY", &CPU::LDY, nullptr, &CPU::fetchZeroPage, 2, 3};
    table[0xB4] = {"LDY", &CPU::LDY, nullptr, &CPU::fetchZeroPageX, 2, 4};
    table[0xAC] = {"LDY", &CPU::LDY, nullptr, &CPU::fetchAbsolute, 3, 4};
    table[0xBC] = {"LDY", &CPU::LDY, nullptr, &CPU::fetchAbsoluteX, 3, 4};

    table[0xE8] = {"INX", nullptr, &CPU::INX, nullptr, 1, 2};

    table[0xC8] = {"INY", nullptr, &CPU::INY, nullptr, 1, 2};
}

void CPU::step() {
    const uint8_t opcode = memory.read(PC++);

    if (const Instruction &instr = table[opcode]; instr.fetch && instr.operate) {
        std::println("Opcode: {}", instr.name);
        const uint8_t value = (this->*instr.fetch)();
        if (pageCrossed) {
            cycles++;
            pageCrossed = !pageCrossed;
        }
        (this->*instr.operate)(value);
    } else if (instr.operateNoArg) {
        std::println("Opcode (no arg): {}", instr.name);
        (this->*instr.operateNoArg)();
    } else {
        std::println("Unknown opcode: {}", opcode);
    }
}

void CPU::BRK() noexcept {
    PC++;

    memory.write(0x0100 + SP--, PC >> 8 & 0xFF);
    memory.write(0x0100 + SP--, PC & 0xFF);

    const uint8_t flags = P | 0x10;
    memory.write(0x0100 + SP--, flags);

    P |= 0x04;

    const uint8_t low = memory.read(0xFFFE);
    const uint8_t high = memory.read(0xFFFF);
    PC = high << 8 | low;
}

void CPU::ORA(const uint8_t value) noexcept {
    A = A | value;
    setZN(A);
}

void CPU::AND(const uint8_t value) noexcept {
    A = A & value;
    setZN(A);
}

void CPU::STA(const uint8_t addr) noexcept {
    memory.write(addr, A);
}

void CPU::LDA(const uint8_t value) noexcept {
    A = value;
    setZN(A);
}

void CPU::LDX(const uint8_t value) noexcept {
    X = value;
    setZN(X);
}

void CPU::LDY(const uint8_t value) noexcept {
    Y = value;
    setZN(Y);
}

void CPU::INX() noexcept {
    X++;
    setZN(X);
}

void CPU::INY() noexcept {
    Y++;
    setZN(Y);
}

uint8_t CPU::fetchImmediate() {
    return memory.read(PC++);
}

uint8_t CPU::fetchZeroPage() {
    const uint8_t addr = memory.read(PC++);
    return memory.read(addr);
}

uint8_t CPU::fetchZeroPageX() {
    const uint8_t base = memory.read(PC++);
    const uint8_t addr = base + X;
    return memory.read(addr);
}

uint8_t CPU::fetchZeroPageY() {
    const uint8_t base = memory.read(PC++);
    const uint8_t addr = base + Y;
    return memory.read(addr);
}

uint8_t CPU::fetchAbsolute() {
    const uint8_t low = memory.read(PC++);
    const uint8_t high = memory.read(PC++);
    const uint16_t addr = high << 8 | low;
    return memory.read(addr);
}

uint8_t CPU::fetchAbsoluteX() {
    const uint8_t low = memory.read(PC++);
    const uint8_t high = memory.read(PC++);
    const uint16_t base = high << 8 | low;
    const uint16_t addr = base + X;
    pageCrossed = (base & 0xFF00) != (addr & 0xFF00);
    return memory.read(addr);
}

uint8_t CPU::fetchAbsoluteY() {
    const uint8_t low = memory.read(PC++);
    const uint8_t high = memory.read(PC++);
    const uint16_t base = high << 8 | low;
    const uint16_t addr = base + Y;
    pageCrossed = (base & 0xFF00) != (addr & 0xFF00);
    return memory.read(addr);
}

uint8_t CPU::fetchIndirectX() {
    const uint8_t base = memory.read(PC++);
    const uint8_t ptr = base + X;
    const uint16_t addr = memory.read(ptr) | memory.read(ptr + 1) << 8;
    return memory.read(addr);
}

uint8_t CPU::fetchIndirectY() {
    const uint8_t base = memory.read(PC++);
    const uint16_t addrBase = memory.read(base) | memory.read(base + 1) << 8;
    const uint16_t addr = addrBase + Y;
    pageCrossed = (addrBase & 0xFF00) != (addr & 0xFF00);
    return memory.read(addr);
}

void CPU::setZN(const uint8_t value) noexcept {
    if (value == 0) P |= 0x02;
    else P &= ~0x02; // Z flag
    if (value & 0x80) P |= 0x80;
    else P &= ~0x80; // N flag
}
