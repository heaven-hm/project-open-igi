#include "igi/qvm_initializer.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace
{

igi::assets::FileBytes integer_argument(std::uint8_t opcode, std::uint32_t operand, std::size_t width)
{
    igi::assets::FileBytes file(60);
    const auto write_integer = [&](std::size_t offset, std::uint32_t value)
    {
        for (std::size_t byte_index = 0; byte_index < 4; ++byte_index)
        {
            file[offset + byte_index] = static_cast<std::uint8_t>(value >> (byte_index * 8));
        }
    };
    file[0] = 'L';
    file[1] = 'O';
    file[2] = 'O';
    file[3] = 'P';
    write_integer(4, 8);
    write_integer(8, 5);
    write_integer(16, 60);
    write_integer(24, 6);
    write_integer(32, 66);
    write_integer(44, 66);
    write_integer(48, static_cast<std::uint32_t>(14 + width));
    const std::array<std::uint8_t, 6> identifiers{'V', 'a', 'l', 'u', 'e', 0};
    file.insert(file.end(), identifiers.begin(), identifiers.end());
    const std::array<std::uint8_t, 12> caller{13, 0, 24, 1, 0, 0, 0, 12, 0, 0, 0, 0};
    file.insert(file.end(), caller.begin(), caller.end());
    file.push_back(opcode);
    for (std::size_t byte_index = 0; byte_index < width; ++byte_index)
    {
        file.push_back(static_cast<std::uint8_t>(operand >> (byte_index * 8)));
    }
    file.push_back(0);
    return file;
}

void verify_operand(std::uint8_t opcode, std::uint32_t operand, std::size_t width, double expected,
                    std::ostream* trace)
{
    const auto file = integer_argument(opcode, operand, width);
    const igi::assets::QvmInitializer initializer{igi::assets::BinaryView(file)};
    if (initializer.calls().size() != 1 || initializer.calls()[0].arguments.size() != 1 ||
        initializer.calls()[0].arguments[0].number != expected)
    {
        throw std::runtime_error("Incorrect signed QVM operand: opcode=" + std::to_string(opcode) +
                                 ", encoded=" + std::to_string(operand) +
                                 ", expected=" + std::to_string(expected));
    }
    if (trace != nullptr)
    {
        *trace << static_cast<unsigned>(opcode) << ',' << operand << ','
               << static_cast<std::int64_t>(initializer.calls()[0].arguments[0].number) << '\n';
        if (!*trace)
        {
            throw std::runtime_error("Cannot write C++ operand trace");
        }
    }
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        if (argument_count > 2)
        {
            throw std::runtime_error("Usage: qvm_integer_operands_probe [output.csv]");
        }
        std::ofstream trace;
        if (argument_count == 2)
        {
            trace.open(arguments[1], std::ios::binary);
            if (!trace)
            {
                throw std::runtime_error("Cannot create C++ operand trace");
            }
        }
        auto* output = trace.is_open() ? &trace : nullptr;
        for (std::uint32_t operand = 0; operand < 256; ++operand)
        {
            const double expected = operand < 128 ? operand : static_cast<double>(operand) - 256;
            verify_operand(3, operand, 1, expected, output);
        }
        for (std::uint32_t operand = 0; operand < 65536; ++operand)
        {
            const double expected = operand < 32768 ? operand : static_cast<double>(operand) - 65536;
            verify_operand(4, operand, 2, expected, output);
        }
        for (const std::uint32_t operand :
             {0u, 1u, 127u, 128u, 32767u, 32768u, 65535u, 0x7fffffffu, 0x80000000u, 0xffffffffu})
        {
            const double expected = operand < 0x80000000u ? operand
                                                        : static_cast<double>(operand) - 4294967296.0;
            verify_operand(2, operand, 4, expected, output);
        }
        if (trace.is_open())
        {
            trace.close();
            if (!trace)
            {
                throw std::runtime_error("Cannot flush C++ operand trace");
            }
        }
        std::cout << "QVM integer operands passed: all 256 byte encodings, all 65536 word encodings, "
                     "and 10 doubleword boundary cases\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "QVM integer operands failed: " << exception.what() << '\n';
        return 1;
    }
}
