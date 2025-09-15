#include "cpu.hpp"
#include <iostream>

const std::array<CPU::Instruction, 256> CPU::table = makeTable();

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

    setFlag(Flag::Break);
    memory.write(0x0100 + SP--, P);

    setFlag(Flag::InterruptDisable);

    const uint8_t low = memory.read(0xFFFE);
    const uint8_t high = memory.read(0xFFFF);
    PC = high << 8 | low;
}

void CPU::CLC(uint16_t, uint8_t) noexcept {
    clearFlag(Flag::Carry);
}

void CPU::CLD(uint16_t, uint8_t) noexcept {
    clearFlag(Flag::Decimal);
}

void CPU::CLI(uint16_t, uint8_t) noexcept {
    clearFlag(Flag::InterruptDisable);
}

void CPU::CLV(uint16_t, uint8_t) noexcept {
    clearFlag(Flag::Overflow);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void CPU::DEC(const uint16_t addr, uint8_t value) noexcept {
    value--;
    memory.write(addr, value);
    setZN(value);
}

void CPU::DEX(uint16_t, uint8_t) noexcept {
    X--;
    setZN(X);
}

void CPU::DEY(uint16_t, uint8_t) noexcept {
    Y--;
    setZN(Y);
}

void CPU::INC(const uint16_t addr, uint8_t value) noexcept {
    value++;
    memory.write(addr, value);
    setZN(value);
}

void CPU::INX(uint16_t, uint8_t) noexcept {
    X++;
    setZN(X);
}

void CPU::INY(uint16_t, uint8_t) noexcept {
    Y++;
    setZN(Y);
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
    if (value == 0) setFlag(Flag::Zero);
    else clearFlag(Flag::Zero);

    if (value & 0x80) setFlag(Flag::Negative);
    else clearFlag(Flag::Negative);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void CPU::setFlag(Flag flag) noexcept {
    P |= static_cast<uint8_t>(flag);
}

// ReSharper disable once CppDFAUnreachableFunctionCall
void CPU::clearFlag(const Flag flag) noexcept {
    P &= ~static_cast<uint8_t>(flag);
}
