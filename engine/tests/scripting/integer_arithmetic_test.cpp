#include "igi/scripting/script_interpreter.hpp"

#include <array>
#include <bit>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{

std::int32_t evaluate(std::uint8_t opcode, std::int32_t left_operand,
                      std::int32_t right_operand, bool unary = false)
{
    igi::QvmNativeRegistry registry;
    igi::QvmProgram program;
    program.uses_loop_85_instruction_set = true;
    const auto append = [&](std::uint8_t instruction_opcode, std::int32_t operand = 0)
    {
        igi::QvmInstruction instruction{};
        instruction.opcode = instruction_opcode;
        instruction.operand_i32 = operand;
        program.instructions.push_back(instruction);
    };
    append(2, left_operand);
    if (!unary)
    {
        append(2, right_operand);
    }
    append(opcode);
    append(0);
    igi::QvmExecutionContext context(program, registry);
    if (!context.Run() || context.StackSize() != 1)
    {
        throw std::runtime_error("Integer script execution failed: " + context.GetLastError());
    }
    const auto result = context.Pop();
    if (result.type != igi::QvmValueType::Int)
    {
        throw std::runtime_error("Integer script returned a non-integer value");
    }
    return result.int_val;
}

void require_equal(std::uint8_t opcode, std::int32_t left_operand,
                   std::int32_t right_operand, std::int32_t expected, bool unary = false)
{
    if (evaluate(opcode, left_operand, right_operand, unary) != expected)
    {
        throw std::runtime_error("Incorrect 32-bit script arithmetic: opcode=" +
                                 std::to_string(opcode) + ", left=" + std::to_string(left_operand) +
                                 ", right=" + std::to_string(right_operand));
    }
}

} // namespace

int main()
{
    try
    {
        constexpr std::array<std::int32_t, 11> operands{
            0, 1, -1, 2, -2, 31, 32, 65535, 65536,
            std::numeric_limits<std::int32_t>::min(),
            std::numeric_limits<std::int32_t>::max()};
        std::size_t checked_operations = 0;
        for (const auto left_operand : operands)
        {
            const auto left_bits = std::bit_cast<std::uint32_t>(left_operand);
            require_equal(0x2c, left_operand, 0,
                          std::bit_cast<std::int32_t>(0u - left_bits), true);
            ++checked_operations;
            for (const auto right_operand : operands)
            {
                const auto right_bits = std::bit_cast<std::uint32_t>(right_operand);
                require_equal(0x19, left_operand, right_operand,
                              std::bit_cast<std::int32_t>(left_bits + right_bits));
                require_equal(0x1a, left_operand, right_operand,
                              std::bit_cast<std::int32_t>(left_bits - right_bits));
                require_equal(0x1b, left_operand, right_operand,
                              std::bit_cast<std::int32_t>(left_bits * right_bits));
                require_equal(0x1d, left_operand, right_operand,
                              std::bit_cast<std::int32_t>(left_bits << (right_bits & 31u)));
                const std::int64_t divisor = std::int64_t{1} << (right_bits & 31u);
                const auto shifted = left_operand >= 0
                    ? static_cast<std::int64_t>(left_operand) / divisor
                    : -((-static_cast<std::int64_t>(left_operand) + divisor - 1) / divisor);
                require_equal(0x1e, left_operand, right_operand, static_cast<std::int32_t>(shifted));
                checked_operations += 5;
            }
        }
        std::cout << "Verified " << checked_operations << " boundary arithmetic operations\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Script integer arithmetic failed: " << exception.what() << '\n';
        return 1;
    }
}
