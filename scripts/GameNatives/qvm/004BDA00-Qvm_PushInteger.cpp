// IGI native Qvm_PushInteger - 0x004BDA00 - transcribed from retail igi.exe.
// Retail: cdecl (qvm, value). Decrements the value stack pointer at +0x18 and the type stack pointer at +0x1c, stores type 1 in the type stack at +0x28 and the value in the value stack at +0x24.
#include <cstdint>
#include <cstring>

int __cdecl Qvm_PushInteger(void* qvm, int value)
{
    std::uint8_t* base = reinterpret_cast<std::uint8_t*>(qvm);
    std::int32_t valueTop = *reinterpret_cast<std::int32_t*>(base + 0x18);
    std::int32_t typeTop = *reinterpret_cast<std::int32_t*>(base + 0x1c);
    valueTop--;
    typeTop--;
    *reinterpret_cast<std::int32_t*>(base + 0x18) = valueTop;
    std::int32_t* typeStack = *reinterpret_cast<std::int32_t**>(base + 0x28);
    *reinterpret_cast<std::int32_t*>(base + 0x1c) = typeTop;
    typeStack[typeTop] = 1;
    std::int32_t* valueStack = *reinterpret_cast<std::int32_t**>(base + 0x24);
    valueStack[*reinterpret_cast<std::int32_t*>(base + 0x18)] = value;
    return value;
}
