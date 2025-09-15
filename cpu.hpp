#pragma once
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

    struct FetchResult {
        uint16_t addr;
        uint8_t value;
    };

    struct Instruction {
        const char* name;
        void (CPU::*operate)(uint16_t addr, uint8_t value);
        AddrMode mode;
        OpType type;
        int bytes;
        int cycles;
    };

    uint8_t A{};
    uint8_t X{};
    uint8_t Y{};
    uint16_t PC{};
    uint8_t P{};
    uint8_t SP{0xFD};
    int cycles{};

    Memory& memory;

    explicit CPU(Memory& mem) : memory(mem) {
        initTable();
    }

    void step();

private:
    static Instruction table[256];

    bool pageCrossed{};

    void BRK(uint16_t, uint8_t) noexcept;
    void ORA(uint16_t, uint8_t value) noexcept;
    void AND(uint16_t, uint8_t value) noexcept;

    void STA(uint16_t addr, uint8_t) noexcept;
    void STX(uint16_t addr, uint8_t) noexcept;
    void STY(uint16_t addr, uint8_t) noexcept;

    void LDA(uint16_t, uint8_t value) noexcept;
    void LDX(uint16_t, uint8_t value) noexcept;
    void LDY(uint16_t, uint8_t value) noexcept;

    void INX(uint16_t, uint8_t) noexcept;
    void INY(uint16_t, uint8_t) noexcept;

    void TAX(uint16_t, uint8_t) noexcept;
    void TAY(uint16_t, uint8_t) noexcept;
    void TSX(uint16_t, uint8_t) noexcept;
    void TXA(uint16_t, uint8_t) noexcept;
    void TXS(uint16_t, uint8_t) noexcept;
    void TYA(uint16_t, uint8_t) noexcept;

    FetchResult fetch(AddrMode mode);

    void setZN(uint8_t value) noexcept;

    static void initTable() noexcept;
};
