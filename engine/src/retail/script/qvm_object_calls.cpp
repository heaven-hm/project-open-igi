#include "igi/retail/script/qvm_object_calls.hpp"

#include <bit>
#include <cmath>
#include <limits>
#include <set>

namespace igi::retail::script
{
namespace
{

class ObjectCallReader
{
  public:
    explicit ObjectCallReader(const QvmCompiledProgram &compiled_program) : program(compiled_program)
    {
    }

    std::vector<std::shared_ptr<QvmObjectCall>> read()
    {
        static_cast<void>(evaluate(0U, 0U));
        return calls;
    }

  private:
    const QvmCompiledProgram &program;
    std::vector<std::shared_ptr<QvmObjectCall>> calls;
    std::set<std::size_t> active_offsets;
    std::size_t instruction_budget = 2000000U;

    std::uint32_t read_integer(std::size_t &offset, std::size_t byte_count)
    {
        if (offset > program.code.size() || byte_count > program.code.size() - offset)
        {
            throw QvmFormatError("QVM object instruction operand is truncated.");
        }
        std::uint32_t value = 0;
        for (std::size_t byte_index = 0; byte_index < byte_count; ++byte_index)
        {
            value |= std::to_integer<std::uint32_t>(program.code[offset++]) << (byte_index * 8U);
        }
        return value;
    }

    static std::string get_string(const std::optional<QvmIndexedStringTable> &table, std::uint32_t index)
    {
        if (table.has_value())
        {
            const auto value = table->try_get(index);
            if (value.has_value())
            {
                return std::string(*value);
            }
        }
        throw QvmFormatError("QVM object instruction references an invalid string index.");
    }

    QvmObjectValue evaluate(std::size_t offset, std::size_t depth)
    {
        if (depth > 128U || !active_offsets.insert(offset).second)
        {
            throw QvmFormatError("QVM object expression has cyclic or excessive recursion.");
        }
        const std::size_t starting_offset = offset;
        std::vector<QvmObjectValue> stack;
        const auto pop = [&stack]()
        {
            if (stack.empty())
            {
                throw QvmFormatError("QVM object expression stack underflow.");
            }
            QvmObjectValue value = std::move(stack.back());
            stack.pop_back();
            return value;
        };
        while (offset < program.code.size())
        {
            if (instruction_budget-- == 0U)
            {
                throw QvmFormatError("QVM object instruction budget exhausted.");
            }
            const std::size_t instruction_offset = offset;
            const std::uint32_t opcode = read_integer(offset, 1U);
            if (opcode == 0U || opcode == 1U)
            {
                active_offsets.erase(starting_offset);
                return stack.empty() ? QvmObjectValue(std::int32_t{0}) : pop();
            }
            switch (opcode)
            {
            case 2U:
                stack.emplace_back(std::bit_cast<std::int32_t>(read_integer(offset, 4U)));
                break;
            case 3U:
            case 4U:
                stack.emplace_back(static_cast<std::int32_t>(read_integer(offset, opcode == 3U ? 1U : 2U)));
                break;
            case 5U:
            {
                const float value = std::bit_cast<float>(read_integer(offset, 4U));
                if (!std::isfinite(value))
                {
                    throw QvmFormatError("QVM object literal is not finite.");
                }
                stack.emplace_back(value);
                break;
            }
            case 8U:
            case 9U:
            case 10U:
            {
                const std::size_t width = opcode == 8U ? 4U : opcode == 9U ? 1U : 2U;
                stack.emplace_back(get_string(program.string_constants, read_integer(offset, width)));
                break;
            }
            case 12U:
            case 13U:
            case 14U:
            {
                const std::size_t width = opcode == 12U ? 4U : opcode == 13U ? 1U : 2U;
                const std::string symbol = get_string(program.symbol_names, read_integer(offset, width));
                if (symbol == "TRUE" || symbol == "FALSE")
                {
                    stack.emplace_back(std::int32_t{symbol == "TRUE" ? 1 : 0});
                }
                else
                {
                    stack.emplace_back(symbol);
                }
                break;
            }
            case 15U:
                stack.emplace_back(std::int32_t{0});
                break;
            case 16U:
                stack.emplace_back(std::int32_t{1});
                break;
            case 17U:
                stack.emplace_back(std::int32_t{-1});
                break;
            case 18U:
                static_cast<void>(pop());
                break;
            case 20U:
            {
                const auto displacement = std::bit_cast<std::int32_t>(read_integer(offset, 4U));
                const std::int64_t destination = static_cast<std::int64_t>(offset) + displacement;
                if (destination < 0 || static_cast<std::uint64_t>(destination) >= program.code.size())
                {
                    throw QvmFormatError("QVM object jump leaves the bytecode region.");
                }
                offset = static_cast<std::size_t>(destination);
                break;
            }
            case 24U:
            {
                const QvmObjectValue function = pop();
                const auto *name = std::get_if<std::string>(&function);
                if (name == nullptr || (*name != "Task_New" && *name != "Task_DeclareParameters"))
                {
                    throw QvmFormatError("QVM object reader encountered an unsupported native call.");
                }
                const std::size_t argument_count = read_integer(offset, 4U);
                if (argument_count > (program.code.size() - offset) / 4U)
                {
                    throw QvmFormatError("QVM CALL argument table is truncated.");
                }
                auto call = std::make_shared<QvmObjectCall>();
                call->instruction_offset = instruction_offset;
                call->function_name = *name;
                call->arguments.reserve(argument_count);
                for (std::size_t argument_index = 0; argument_index < argument_count; ++argument_index)
                {
                    const std::size_t argument_offset = read_integer(offset, 4U);
                    if (argument_offset >= program.code.size())
                    {
                        throw QvmFormatError("QVM CALL argument points outside bytecode.");
                    }
                    call->arguments.push_back(evaluate(argument_offset, depth + 1U));
                }
                calls.push_back(call);
                stack.emplace_back(std::move(call));
                break;
            }
            case 43U:
            case 44U:
            case 45U:
            case 46U:
            {
                QvmObjectValue value = pop();
                if (auto *integer = std::get_if<std::int32_t>(&value))
                {
                    if (opcode == 44U)
                    {
                        *integer = std::bit_cast<std::int32_t>(0U - std::bit_cast<std::uint32_t>(*integer));
                    }
                    if (opcode == 45U)
                    {
                        *integer = ~*integer;
                    }
                    if (opcode == 46U)
                    {
                        *integer = !*integer;
                    }
                }
                else if (auto *real = std::get_if<float>(&value); real != nullptr && opcode <= 44U)
                {
                    if (opcode == 44U)
                    {
                        *real = -*real;
                    }
                }
                else
                {
                    throw QvmFormatError("QVM unary instruction has invalid operand type.");
                }
                stack.push_back(std::move(value));
                break;
            }
            default:
                throw QvmFormatError("QVM object reader does not support opcode " + std::to_string(opcode) +
                                     " at " + std::to_string(instruction_offset));
            }
        }
        throw QvmFormatError("QVM object expression has no terminating instruction.");
    }
};
} // namespace

std::vector<std::shared_ptr<QvmObjectCall>> Qvm_ReadObjectCalls(const QvmCompiledProgram &program)
{
    return ObjectCallReader(program).read();
}
} // namespace igi::retail::script
