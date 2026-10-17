#pragma once

#include "igi/retail/script/qvm_compiled_program.hpp"
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace igi::retail::script
{
struct QvmObjectCall;
using QvmObjectValue = std::variant<std::int32_t, float, std::string, std::shared_ptr<QvmObjectCall>>;
struct QvmObjectCall
{
    std::size_t instruction_offset = 0;
    std::string function_name;
    std::vector<QvmObjectValue> arguments;
};

// QVM object-data reader, not a substitute for the complete Qvm_Execute task runtime.
// Static sources in igi.exe: dispatcher registration 0x004BD430, CALL 0x004BD880,
// PUSH/PUSHB/PUSHW/PUSHF 0x004BE870..0x004BE9A0, indexed strings 0x004BEA30,
// unary operations 0x004BE6A0 and jump 0x004BD730. Original image SHA256:
// dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verification: static translation; retail call-by-call runtime comparison remains pending.
[[nodiscard]] std::vector<std::shared_ptr<QvmObjectCall>>
Qvm_ReadObjectCalls(const QvmCompiledProgram &program);
} // namespace igi::retail::script
