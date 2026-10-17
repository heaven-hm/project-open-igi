#include "igi/retail/script/qvm_stack_natives.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace
{

void require(bool condition, const char* message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_push_integer_matches_observed_stack_writes()
{
    std::array<std::uint32_t, 4U> value_stack{0xA5A5A5A5U, 0xA5A5A5A5U, 0xA5A5A5A5U, 0xA5A5A5A5U};
    std::array<std::uint32_t, 4U> type_stack{0x5A5A5A5AU, 0x5A5A5A5AU, 0x5A5A5A5AU, 0x5A5A5A5AU};
    igi::retail::script::QvmNativeStatePrefix state{};
    state.value_stack_index = 3;
    state.type_stack_index = 2;
    state.value_stack = value_stack.data();
    state.type_stack = type_stack.data();

    igi::retail::script::Qvm_PushInteger(&state, 0xDEADBEEFU);

    require(state.value_stack_index == 2, "native decrements the value-stack index once");
    require(state.type_stack_index == 1, "native decrements the type-stack index once");
    require(type_stack[1U] == 1U, "native writes the observed integer type tag");
    require(value_stack[2U] == 0xDEADBEEFU, "native stores the input DWORD unchanged");
    require(value_stack[1U] == 0xA5A5A5A5U, "native leaves other value slots unchanged");
    require(type_stack[2U] == 0x5A5A5A5AU, "native leaves other type slots unchanged");
}

} // namespace

int main()
{
    try {
        test_push_integer_matches_observed_stack_writes();
        std::cout << "QVM integer stack native checks passed.\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "QVM integer stack native checks failed: " << exception.what() << '\n';
        return 1;
    }
}
