#include "cpu.hpp"
#include <iostream>
#include "opcode.hpp"

void CPU::step() {
    const uint8_t raw = memory.read(PC++);

    switch (const auto op = static_cast<Opcode>(raw)) {
        case Opcode::LDA_Immediate:
            LDA(fetchImmediate());
            break;
        case Opcode::LDA_ZeroPage:
            LDA(fetchZeroPage());
            break;
        case Opcode::LDA_ZeroPageX:
            LDA(fetchZeroPageX());
            break;
        case Opcode::LDA_Absolute:
            LDA(fetchAbsolute());
            break;
        case Opcode::LDA_AbsoluteX: {
            bool pageCrossed = false;
            LDA(fetchAbsoluteX(pageCrossed));
            break;
        }
        case Opcode::LDA_AbsoluteY: {
            bool pageCrossed = false;
            LDA(fetchAbsoluteY(pageCrossed));
            break;
        }
        case Opcode::LDA_IndirectX:
            LDA(fetchIndirectX());
            break;
        case Opcode::LDA_IndirectY: {
            bool pageCrossed = false;
            LDA(fetchIndirectY(pageCrossed));
            break;
        }
        default:
            std::cerr << "Unknown opcode: " << static_cast<int>(op) << "\n";
            break;
    }
}

// --- Логика инструкции ---
void CPU::LDA(const uint8_t value) noexcept {
    A = value;
    setZN(A);
}

void CPU::setZN(const uint8_t value) noexcept {
    if (value == 0) status |= 0x02; else status &= ~0x02; // Z flag
    if (value & 0x80) status |= 0x80; else status &= ~0x80; // N flag
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

uint8_t CPU::fetchAbsolute() {
    const uint8_t low = memory.read(PC++);
    const uint8_t high = memory.read(PC++);
    const uint16_t addr = high << 8 | low;
    return memory.read(addr);
}

uint8_t CPU::fetchAbsoluteX(bool& pageCrossed) {
    const uint8_t low = memory.read(PC++);
    const uint8_t high = memory.read(PC++);
    const uint16_t base = high << 8 | low;
    const uint16_t addr = base + X;
    pageCrossed = (base & 0xFF00) != (addr & 0xFF00);
    return memory.read(addr);
}

uint8_t CPU::fetchAbsoluteY(bool& pageCrossed) {
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

uint8_t CPU::fetchIndirectY(bool& pageCrossed) {
    const uint8_t base = memory.read(PC++);
    const uint16_t addrBase = memory.read(base) | memory.read(base + 1) << 8;
    const uint16_t addr = addrBase + Y;
    pageCrossed = (addrBase & 0xFF00) != (addr & 0xFF00);
    return memory.read(addr);
}
