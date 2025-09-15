#pragma once
#include <string_view>
#include "memory.hpp"

class CPU {
public:
    enum class AddrMode {
        Implied,
        Immediate,
        ZeroPage,
        ZeroPageX,
        ZeroPageY,
        Absolute,
        AbsoluteX,
        AbsoluteY,
        IndirectX,
        IndirectY
    };

    enum class OpType {
        Read,
        Write,
        ReadModifyWrite,
        Implied
    };

    enum class Flag : uint8_t {
        Carry = 0x01,
        Zero = 0x02,
        InterruptDisable = 0x04,
        Decimal = 0x08,
        Break = 0x10,
        Unused = 0x20,
        Overflow = 0x40,
        Negative = 0x80,
    };

    struct FetchResult {
        uint16_t addr;
        uint8_t value;
    };

    struct Instruction {
        std::string_view name;

        void (CPU::*operate)(uint16_t addr, uint8_t value);

        AddrMode mode;
        OpType type;
        int bytes;
        int cycles;
    };

    uint8_t A{}; // Accumulator
    uint8_t X{}; // X Index
    uint8_t Y{}; // Y Index
    uint16_t PC{}; // Program Counter
    uint8_t P{}; // Status Flag
    uint8_t SP{0xFD}; // Stack Pointer
    int cycles{};

    Memory &memory;

    explicit CPU(Memory &mem) : memory(mem) {
    }

    void step();

private:
    static std::array<Instruction, 256> makeTable() {
        std::array<Instruction, 256> t{};
        for (auto &i: t) {
            i = {"Unimplemented", nullptr, AddrMode::Implied, OpType::Implied, 1, 2};
        }

        auto set = [&](const uint8_t opcode, const std::string_view name, auto fn, AddrMode mode, OpType type,
                       int bytes,
                       int cycles) {
            t[opcode] = {name, fn, mode, type, bytes, cycles};
        };

        // Access

        // LDA
        set(0xA9, "LDA", &CPU::LDA, AddrMode::Immediate, OpType::Read, 2, 2);
        set(0xA5, "LDA", &CPU::LDA, AddrMode::ZeroPage, OpType::Read, 2, 3);
        set(0xB5, "LDA", &CPU::LDA, AddrMode::ZeroPageX, OpType::Read, 2, 4);
        set(0xAD, "LDA", &CPU::LDA, AddrMode::Absolute, OpType::Read, 3, 4);
        set(0xBD, "LDA", &CPU::LDA, AddrMode::AbsoluteX, OpType::Read, 3, 4);
        set(0xB9, "LDA", &CPU::LDA, AddrMode::AbsoluteY, OpType::Read, 3, 4);
        set(0xA1, "LDA", &CPU::LDA, AddrMode::IndirectX, OpType::Read, 2, 6);
        set(0xB1, "LDA", &CPU::LDA, AddrMode::IndirectY, OpType::Read, 2, 5);

        // LDX
        set(0xA2, "LDX", &CPU::LDX, AddrMode::Immediate, OpType::Read, 2, 2);
        set(0xA6, "LDX", &CPU::LDX, AddrMode::ZeroPage, OpType::Read, 2, 3);
        set(0xB6, "LDX", &CPU::LDX, AddrMode::ZeroPageY, OpType::Read, 2, 4);
        set(0xAE, "LDX", &CPU::LDX, AddrMode::Absolute, OpType::Read, 3, 4);
        set(0xBE, "LDX", &CPU::LDX, AddrMode::AbsoluteY, OpType::Read, 3, 4);

        // LDY
        set(0xA0, "LDY", &CPU::LDY, AddrMode::Immediate, OpType::Read, 2, 2);
        set(0xA4, "LDY", &CPU::LDY, AddrMode::ZeroPage, OpType::Read, 2, 3);
        set(0xB4, "LDY", &CPU::LDY, AddrMode::ZeroPageX, OpType::Read, 2, 4);
        set(0xAC, "LDY", &CPU::LDY, AddrMode::Absolute, OpType::Read, 3, 4);
        set(0xBC, "LDY", &CPU::LDY, AddrMode::AbsoluteX, OpType::Read, 3, 4);

        // STA
        set(0x85, "STA", &CPU::STA, AddrMode::ZeroPage, OpType::Write, 2, 3);
        set(0x95, "STA", &CPU::STA, AddrMode::ZeroPageX, OpType::Write, 2, 4);
        set(0x8D, "STA", &CPU::STA, AddrMode::Absolute, OpType::Write, 3, 4);
        set(0x9D, "STA", &CPU::STA, AddrMode::AbsoluteX, OpType::Write, 3, 5);
        set(0x99, "STA", &CPU::STA, AddrMode::AbsoluteY, OpType::Write, 3, 5);
        set(0x81, "STA", &CPU::STA, AddrMode::IndirectX, OpType::Write, 2, 6);
        set(0x91, "STA", &CPU::STA, AddrMode::IndirectY, OpType::Write, 2, 6);

        // Arithmetic

        // DEC
        set(0xC6, "DEC", &CPU::DEC, AddrMode::ZeroPage, OpType::ReadModifyWrite, 2, 5);
        set(0xD6, "DEC", &CPU::DEC, AddrMode::ZeroPageX, OpType::ReadModifyWrite, 2, 6);
        set(0xCE, "DEC", &CPU::DEC, AddrMode::Absolute, OpType::ReadModifyWrite, 3, 6);
        set(0xDE, "DEC", &CPU::DEC, AddrMode::AbsoluteX, OpType::ReadModifyWrite, 3, 7);

        // DEX / DEY
        set(0xCA, "DEX", &CPU::DEX, AddrMode::Implied, OpType::Implied, 1, 2);
        set(0x88, "DEY", &CPU::DEY, AddrMode::Implied, OpType::Implied, 1, 2);

        // INC
        set(0xE6, "INC", &CPU::INC, AddrMode::ZeroPage, OpType::ReadModifyWrite, 2, 5);
        set(0xF6, "INC", &CPU::INC, AddrMode::ZeroPageX, OpType::ReadModifyWrite, 2, 6);
        set(0xEE, "INC", &CPU::INC, AddrMode::Absolute, OpType::ReadModifyWrite, 3, 6);
        set(0xFE, "INC", &CPU::INC, AddrMode::AbsoluteX, OpType::ReadModifyWrite, 3, 7);

        // INX / INY
        set(0xE8, "INX", &CPU::INX, AddrMode::Implied, OpType::Implied, 1, 2);
        set(0xC8, "INY", &CPU::INY, AddrMode::Implied, OpType::Implied, 1, 2);

        // Bitwise

        // AND
        set(0x29, "AND", &CPU::AND, AddrMode::Immediate, OpType::Read, 2, 2);
        set(0x25, "AND", &CPU::AND, AddrMode::ZeroPage, OpType::Read, 2, 3);
        set(0x35, "AND", &CPU::AND, AddrMode::ZeroPageX, OpType::Read, 2, 4);
        set(0x2D, "AND", &CPU::AND, AddrMode::Absolute, OpType::Read, 3, 4);
        set(0x3D, "AND", &CPU::AND, AddrMode::AbsoluteX, OpType::Read, 3, 4);
        set(0x39, "AND", &CPU::AND, AddrMode::AbsoluteY, OpType::Read, 3, 4);
        set(0x21, "AND", &CPU::AND, AddrMode::IndirectX, OpType::Read, 2, 6);
        set(0x31, "AND", &CPU::AND, AddrMode::IndirectY, OpType::Read, 2, 5);

        // ORA
        set(0x09, "ORA", &CPU::ORA, AddrMode::Immediate, OpType::Read, 2, 2);
        set(0x05, "ORA", &CPU::ORA, AddrMode::ZeroPage, OpType::Read, 2, 3);
        set(0x15, "ORA", &CPU::ORA, AddrMode::ZeroPageX, OpType::Read, 2, 4);
        set(0x0D, "ORA", &CPU::ORA, AddrMode::Absolute, OpType::Read, 3, 4);
        set(0x1D, "ORA", &CPU::ORA, AddrMode::AbsoluteX, OpType::Read, 3, 4);
        set(0x19, "ORA", &CPU::ORA, AddrMode::AbsoluteY, OpType::Read, 3, 4);
        set(0x01, "ORA", &CPU::ORA, AddrMode::IndirectX, OpType::Read, 2, 6);
        set(0x11, "ORA", &CPU::ORA, AddrMode::IndirectY, OpType::Read, 2, 5);

        // Flags

        // CLC / CLD / CLI / CLV
        set(0x18, "CLC", &CPU::CLC, AddrMode::Implied, OpType::Implied, 1, 2);
        set(0xD8, "CLD", &CPU::CLD, AddrMode::Implied, OpType::Implied, 1, 2);
        set(0x58, "CLI", &CPU::CLI, AddrMode::Implied, OpType::Implied, 1, 2);
        set(0xB8, "CLV", &CPU::CLV, AddrMode::Implied, OpType::Implied, 1, 2);

        // Jump

        // BRK
        set(0x00, "BRK", &CPU::BRK, AddrMode::Implied, OpType::Implied, 2, 7);

        // Transfer
        set(0xAA, "TAX", &CPU::TAX, AddrMode::Implied, OpType::Implied, 1, 2);
        set(0xA8, "TAY", &CPU::TAY, AddrMode::Implied, OpType::Implied, 1, 2);
        set(0xBA, "TSX", &CPU::TSX, AddrMode::Implied, OpType::Implied, 1, 2);
        set(0x8A, "TXA", &CPU::TXA, AddrMode::Implied, OpType::Implied, 1, 2);
        set(0x9A, "TXS", &CPU::TXS, AddrMode::Implied, OpType::Implied, 1, 2);
        set(0x98, "TYA", &CPU::TYA, AddrMode::Implied, OpType::Implied, 1, 2);

        return t;
    }

    static const std::array<Instruction, 256> table;

    bool pageCrossed{};

    void BRK(uint16_t, uint8_t) noexcept;

    void CLC(uint16_t, uint8_t) noexcept;

    void CLD(uint16_t, uint8_t) noexcept;

    void CLI(uint16_t, uint8_t) noexcept;

    void CLV(uint16_t, uint8_t) noexcept;

    void DEC(uint16_t addr, uint8_t value) noexcept;

    void DEX(uint16_t, uint8_t) noexcept;

    void DEY(uint16_t, uint8_t) noexcept;

    void INC(uint16_t addr, uint8_t value) noexcept;

    void INX(uint16_t, uint8_t) noexcept;

    void INY(uint16_t, uint8_t) noexcept;

    void ORA(uint16_t, uint8_t value) noexcept;

    void AND(uint16_t, uint8_t value) noexcept;

    void STA(uint16_t addr, uint8_t) noexcept;

    void STX(uint16_t addr, uint8_t) noexcept;

    void STY(uint16_t addr, uint8_t) noexcept;

    void LDA(uint16_t, uint8_t value) noexcept;

    void LDX(uint16_t, uint8_t value) noexcept;

    void LDY(uint16_t, uint8_t value) noexcept;

    void TAX(uint16_t, uint8_t) noexcept;

    void TAY(uint16_t, uint8_t) noexcept;

    void TSX(uint16_t, uint8_t) noexcept;

    void TXA(uint16_t, uint8_t) noexcept;

    void TXS(uint16_t, uint8_t) noexcept;

    void TYA(uint16_t, uint8_t) noexcept;

    FetchResult fetch(AddrMode mode);

    void setZN(uint8_t value) noexcept;

    void setFlag(Flag flag) noexcept;

    void clearFlag(Flag flag) noexcept;
};
