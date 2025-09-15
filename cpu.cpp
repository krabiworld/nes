#include "cpu.hpp"
#include <iostream>

CPU::Instruction CPU::table[256];

void CPU::initTable() noexcept {
    for (auto &i: table) {
        i = {"???", nullptr, AddrMode::Implied, OpType::Implied, 1, 2};
    }

    table[0x00] = {"BRK", &CPU::BRK, AddrMode::Implied, OpType::Implied, 2, 7};

    table[0x09] = {"ORA", &CPU::ORA, AddrMode::Immediate, OpType::Read, 2, 2};
    table[0x05] = {"ORA", &CPU::ORA, AddrMode::ZeroPage, OpType::Read, 2, 3};
    table[0x15] = {"ORA", &CPU::ORA, AddrMode::ZeroPageX, OpType::Read, 2, 4};
    table[0x0D] = {"ORA", &CPU::ORA, AddrMode::Absolute, OpType::Read, 3, 4};
    table[0x1D] = {"ORA", &CPU::ORA, AddrMode::AbsoluteX, OpType::Read, 3, 4};
    table[0x19] = {"ORA", &CPU::ORA, AddrMode::AbsoluteY, OpType::Read, 3, 4};
    table[0x01] = {"ORA", &CPU::ORA, AddrMode::IndirectX, OpType::Read, 2, 6};
    table[0x11] = {"ORA", &CPU::ORA, AddrMode::IndirectY, OpType::Read, 2, 5};

    table[0x29] = {"AND", &CPU::AND, AddrMode::Immediate, OpType::Read, 2, 2};
    table[0x25] = {"AND", &CPU::AND, AddrMode::ZeroPage, OpType::Read, 2, 3};
    table[0x35] = {"AND", &CPU::AND, AddrMode::ZeroPageX, OpType::Read, 2, 4};
    table[0x2D] = {"AND", &CPU::AND, AddrMode::Absolute, OpType::Read, 3, 4};
    table[0x3D] = {"AND", &CPU::AND, AddrMode::AbsoluteX, OpType::Read, 3, 4};
    table[0x39] = {"AND", &CPU::AND, AddrMode::AbsoluteY, OpType::Read, 3, 4};
    table[0x21] = {"AND", &CPU::AND, AddrMode::IndirectX, OpType::Read, 2, 6};
    table[0x31] = {"AND", &CPU::AND, AddrMode::IndirectY, OpType::Read, 2, 5};

    table[0x85] = {"STA", &CPU::STA, AddrMode::ZeroPage, OpType::Write, 2, 3};
    table[0x95] = {"STA", &CPU::STA, AddrMode::ZeroPageX, OpType::Write, 2, 4};
    table[0x8D] = {"STA", &CPU::STA, AddrMode::Absolute, OpType::Write, 3, 4};
    table[0x9D] = {"STA", &CPU::STA, AddrMode::AbsoluteX, OpType::Write, 3, 5};
    table[0x99] = {"STA", &CPU::STA, AddrMode::AbsoluteY, OpType::Write, 3, 5};
    table[0x81] = {"STA", &CPU::STA, AddrMode::IndirectX, OpType::Write, 2, 6};
    table[0x91] = {"STA", &CPU::STA, AddrMode::IndirectY, OpType::Write, 2, 6};

    table[0xA9] = {"LDA", &CPU::LDA, AddrMode::Immediate, OpType::Read, 2, 2};
    table[0xA5] = {"LDA", &CPU::LDA, AddrMode::ZeroPage, OpType::Read, 2, 3};
    table[0xB5] = {"LDA", &CPU::LDA, AddrMode::ZeroPageX, OpType::Read, 2, 4};
    table[0xAD] = {"LDA", &CPU::LDA, AddrMode::Absolute, OpType::Read, 3, 4};
    table[0xBD] = {"LDA", &CPU::LDA, AddrMode::AbsoluteX, OpType::Read, 3, 4};
    table[0xB9] = {"LDA", &CPU::LDA, AddrMode::AbsoluteY, OpType::Read, 3, 4};
    table[0xA1] = {"LDA", &CPU::LDA, AddrMode::IndirectX, OpType::Read, 2, 6};
    table[0xB1] = {"LDA", &CPU::LDA, AddrMode::IndirectY, OpType::Read, 2, 5};

    table[0xA2] = {"LDX", &CPU::LDX, AddrMode::Immediate, OpType::Read, 2, 2};
    table[0xA6] = {"LDX", &CPU::LDX, AddrMode::ZeroPage, OpType::Read, 2, 3};
    table[0xB6] = {"LDX", &CPU::LDX, AddrMode::ZeroPageY, OpType::Read, 2, 4};
    table[0xAE] = {"LDX", &CPU::LDX, AddrMode::Absolute, OpType::Read, 3, 4};
    table[0xBE] = {"LDX", &CPU::LDX, AddrMode::AbsoluteY, OpType::Read, 3, 4};

    table[0xA0] = {"LDY", &CPU::LDY, AddrMode::Immediate, OpType::Read, 2, 2};
    table[0xA4] = {"LDY", &CPU::LDY, AddrMode::ZeroPage, OpType::Read, 2, 3};
    table[0xB4] = {"LDY", &CPU::LDY, AddrMode::ZeroPageX, OpType::Read, 2, 4};
    table[0xAC] = {"LDY", &CPU::LDY, AddrMode::Absolute, OpType::Read, 3, 4};
    table[0xBC] = {"LDY", &CPU::LDY, AddrMode::AbsoluteX, OpType::Read, 3, 4};

    table[0xE8] = {"INX", &CPU::INX, AddrMode::Implied, OpType::Implied, 1, 2};
    table[0xC8] = {"INY", &CPU::INY, AddrMode::Implied, OpType::Implied, 1, 2};

    table[0xAA] = {"TAX", &CPU::TAX, AddrMode::Implied, OpType::Implied, 1, 2};
    table[0xA8] = {"TAY", &CPU::TAY, AddrMode::Implied, OpType::Implied, 1, 2};
    table[0xBA] = {"TSX", &CPU::TSX, AddrMode::Implied, OpType::Implied, 1, 2};
    table[0x8A] = {"TXA", &CPU::TXA, AddrMode::Implied, OpType::Implied, 1, 2};
    table[0x9A] = {"TXS", &CPU::TXS, AddrMode::Implied, OpType::Implied, 1, 2};
    table[0x98] = {"TYA", &CPU::TYA, AddrMode::Implied, OpType::Implied, 1, 2};
}

void CPU::step() {
    const uint8_t opcode = memory.read(PC++);
    const Instruction &instr = table[opcode];

    std::println("Opcode: {}", instr.name);

    FetchResult fr{0, 0};
    if (instr.mode != AddrMode::Implied) {
        fr = fetch(instr.mode);
    }

    switch (instr.type) {
        case OpType::Read:
            (this->*instr.operate)(0, fr.value);
            break;
        case OpType::Write:
            (this->*instr.operate)(fr.addr, 0);
            break;
        case OpType::ReadModifyWrite:
            (this->*instr.operate)(fr.addr, fr.value);
            break;
        case OpType::Implied:
            (this->*instr.operate)(0, 0);
            break;
    }

    if (pageCrossed) {
        cycles++;
        pageCrossed = false;
    }
}

void CPU::BRK(uint16_t, uint8_t) noexcept {
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

void CPU::ORA(uint16_t, const uint8_t value) noexcept {
    A = A | value;
    setZN(A);
}

void CPU::AND(uint16_t, const uint8_t value) noexcept {
    A = A & value;
    setZN(A);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void CPU::STA(const uint16_t addr, uint8_t) noexcept {
    memory.write(addr, A);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void CPU::STX(const uint16_t addr, uint8_t) noexcept {
    memory.write(addr, X);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void CPU::STY(const uint16_t addr, uint8_t) noexcept {
    memory.write(addr, Y);
}

void CPU::LDA(uint16_t, const uint8_t value) noexcept {
    A = value;
    setZN(A);
}

void CPU::LDX(uint16_t, const uint8_t value) noexcept {
    X = value;
    setZN(X);
}

void CPU::LDY(uint16_t, const uint8_t value) noexcept {
    Y = value;
    setZN(Y);
}

void CPU::INX(uint16_t, uint8_t) noexcept {
    X++;
    setZN(X);
}

void CPU::INY(uint16_t, uint8_t) noexcept {
    Y++;
    setZN(Y);
}

void CPU::TAX(uint16_t, uint8_t) noexcept {
    X = A;
    setZN(X);
}

void CPU::TAY(uint16_t, uint8_t) noexcept {
    Y = A;
    setZN(Y);
}

void CPU::TSX(uint16_t, uint8_t) noexcept {
    X = SP;
    setZN(X);
}

void CPU::TXA(uint16_t, uint8_t) noexcept {
    A = X;
    setZN(A);
}

void CPU::TXS(uint16_t, uint8_t) noexcept {
    SP = A;
    setZN(SP);
}

void CPU::TYA(uint16_t, uint8_t) noexcept {
    A = Y;
    setZN(A);
}

CPU::FetchResult CPU::fetch(const AddrMode mode) {
    FetchResult res{0, 0};

    switch (mode) {
        case AddrMode::Immediate:
            res.addr = PC++;
            res.value = memory.read(res.addr);
            break;

        case AddrMode::ZeroPage: {
            const uint8_t addr = memory.read(PC++);
            res.addr = addr;
            res.value = memory.read(addr);
            break;
        }

        case AddrMode::ZeroPageX: {
            const uint8_t base = memory.read(PC++);
            const uint8_t addr = base + X;
            res.addr = addr;
            res.value = memory.read(addr);
            break;
        }

        case AddrMode::ZeroPageY: {
            const uint8_t base = memory.read(PC++);
            const uint8_t addr = base + Y;
            res.addr = addr;
            res.value = memory.read(addr);
            break;
        }

        case AddrMode::Absolute: {
            const uint8_t lo = memory.read(PC++);
            const uint8_t hi = memory.read(PC++);
            res.addr = hi << 8 | lo;
            res.value = memory.read(res.addr);
            break;
        }

        case AddrMode::AbsoluteX: {
            const uint8_t lo = memory.read(PC++);
            const uint8_t hi = memory.read(PC++);
            const uint16_t base = hi << 8 | lo;
            res.addr = base + X;
            pageCrossed = (base & 0xFF00) != (res.addr & 0xFF00);
            res.value = memory.read(res.addr);
            break;
        }

        case AddrMode::AbsoluteY: {
            const uint8_t lo = memory.read(PC++);
            const uint8_t hi = memory.read(PC++);
            const uint16_t base = hi << 8 | lo;
            res.addr = base + Y;
            pageCrossed = (base & 0xFF00) != (res.addr & 0xFF00);
            res.value = memory.read(res.addr);
            break;
        }

        case AddrMode::IndirectX: {
            const uint8_t base = memory.read(PC++);
            const uint8_t ptr = base + X;
            res.addr = memory.read(ptr) | memory.read(ptr + 1) << 8;
            res.value = memory.read(res.addr);
            break;
        }

        case AddrMode::IndirectY: {
            const uint8_t base = memory.read(PC++);
            const uint16_t addrBase = memory.read(base) | memory.read(base + 1) << 8;
            res.addr = addrBase + Y;
            pageCrossed = (addrBase & 0xFF00) != (res.addr & 0xFF00);
            res.value = memory.read(res.addr);
            break;
        }

        case AddrMode::Implied:
        default:
            res.addr = 0;
            res.value = 0;
            break;
    }

    return res;
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void CPU::setZN(const uint8_t value) noexcept {
    if (value == 0) P |= 0x02;
    else P &= ~0x02; // Z flag
    if (value & 0x80) P |= 0x80;
    else P &= ~0x80; // N flag
}
