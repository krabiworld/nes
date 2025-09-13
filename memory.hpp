#pragma once

#include <array>

class Memory {
public:
    Memory() {
        data.fill(0x00);
    }

    [[nodiscard]] constexpr uint8_t read(const uint16_t addr) const noexcept {
        return data[addr];
    }

    constexpr void write(const uint16_t addr, const uint8_t value) noexcept {
        data[addr] = value;
    }

private:
    std::array<uint8_t, 0x10000> data{};
};
