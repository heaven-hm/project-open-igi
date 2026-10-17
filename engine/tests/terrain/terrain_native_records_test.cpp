#include "igi/retail/terrain/terrain_native_records.hpp"

#include <array>
#include <cstring>
#include <iostream>
#include <stdexcept>

int main()
{
    try
    {
        using namespace igi::retail::terrain;
        std::array<std::byte, 32> source{};
        const std::uint32_t command_offset = 0x12345678U;
        std::memcpy(source.data() + 0x1C, &command_offset, sizeof(command_offset));
        TerrainContourRecord contour{};
        std::memcpy(&contour, source.data(), source.size());
        if (contour.commands.file_offset != command_offset)
        {
            throw std::runtime_error("CTR +0x1C source offset differs");
        }
        TerrainCommandHeader command{};
        command.triangle_byte_count = -4;
        if (command.triangle_byte_count != -4)
        {
            throw std::runtime_error("Native signed command field changed");
        }
        TerrainGridRecord grid{};
        grid.dimension = 8;
        if (reinterpret_cast<const std::byte*>(&grid.dimension) - reinterpret_cast<const std::byte*>(&grid) !=
            8)
        {
            throw std::runtime_error("Grid dimension offset changed");
        }
        std::cout
            << "Windows x86 terrain record offset/stride checks passed; renderer integration pending.\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << '\n';
        return 1;
    }
}
