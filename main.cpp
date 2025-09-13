#include <fstream>
#include <vector>
#include <iostream>

#include "cpu.hpp"
#include "memory.hpp"

bool loadNES(const std::string &path, std::vector<uint8_t> &prgRom) {
    std::ifstream file{path, std::ios::binary};
    if (!file) {
        std::cerr << "Cannot open ROM: " << path << "\n";
        return false;
    }

    uint8_t header[16];
    file.read(reinterpret_cast<char *>(header), 16);
    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A) {
        std::cerr << "Not a valid NES ROM\n";
        return false;
    }

    const int prgSize = header[4] * 16 * 1024; // PRG-ROM size in bytes
    prgRom.resize(prgSize);
    file.read(reinterpret_cast<char *>(prgRom.data()), prgSize);

    std::cout << "Loaded PRG-ROM, size: " << prgSize << " bytes\n";
    return true;
}

int main(const int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_rom.nes>\n";
        return 1;
    }

    const std::string romPath = argv[1];

    Memory memory;
    CPU cpu(memory);

    std::cout << "Loading ROM: " << romPath << "\n";

    std::vector<uint8_t> prgRom;
    if (!loadNES(romPath, prgRom)) {
        return 1;
    }

    uint16_t start = 0x8000;
    for (size_t i = 0; i < prgRom.size(); ++i) {
        memory.write(start + i, prgRom[i]);
    }

    std::cout << "Starting CPU at PC: " << std::hex << cpu.PC << "\n";

    for (int i = 0; i < 10; ++i) {
        cpu.step();
        std::cout << "Step " << i + 1
                << " | A: " << std::hex << static_cast<int>(cpu.A)
                << " X: " << static_cast<int>(cpu.X)
                << " Y: " << static_cast<int>(cpu.Y)
                << " P: " << static_cast<int>(cpu.P)
                << " PC: " << cpu.PC
                << "\n";
    }

    return 0;
}
