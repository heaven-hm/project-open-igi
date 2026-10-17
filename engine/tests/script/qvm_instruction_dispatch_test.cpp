#include "igi/retail/script/qvm_stack_natives.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace
{

std::array<std::uint32_t, 2U> observed_opcodes{};
std::uint32_t observed_callback_count = 0U;
bool observed_default_arguments = true;

void require(bool condition, const char* message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void record_and_advance(
    igi::retail::script::QvmNativeStatePrefix* state,
    std::uint32_t second_argument,
    std::uint32_t opcode,
    std::uint32_t fourth_argument)
{
    observed_default_arguments = observed_default_arguments && second_argument == 0U && fourth_argument == 0U;
    observed_opcodes[observed_callback_count] = opcode;
    ++observed_callback_count;
    if (opcode == 0x15U) {
        ++state->instruction_offset;
    } else {
        state->stop_flag = 1U;
    }
}

void test_dispatches_bytes_and_stops_on_handler_flag()
{
    using namespace igi::retail::script;
    const std::array<std::byte, 2U> code{std::byte{0x15U}, std::byte{0x16U}};
    QvmNativeStatePrefix state{};
    state.code_base = code.data();
    state.error_status = 0U;
    state.instruction_offset = 0U;
    state.stop_flag = 0xFFFFFFFFU;
    observed_opcodes = {};
    observed_callback_count = 0U;
    observed_default_arguments = true;

    Qvm_SetOpcodeHandler(0x15, 3, record_and_advance);
    Qvm_SetOpcodeHandler(0x16, 0, record_and_advance);
    require(QvmOpcodeHandlerTable[0x15U].handlers[3U] == record_and_advance,
        "registration stores a callback in the requested variant slot");
    Qvm_SetOpcodeHandler(0x15, 0, record_and_advance);

    Qvm_ExecuteInstructions(&state);

    require(observed_callback_count == 2U, "dispatcher invokes each opcode through the handler table");
    require(observed_opcodes[0U] == 0x15U && observed_opcodes[1U] == 0x16U,
        "dispatcher reads opcodes at the current byte offset");
    require(observed_default_arguments, "dispatcher passes zero for callback arguments two and four");
    require(state.instruction_offset == 1U, "opcode callback controls instruction offset advancement");
    require(state.stop_flag == 1U, "dispatcher exits after a handler sets the stop flag");
}

void test_returns_on_error_status_after_clearing_stop_flag()
{
    using namespace igi::retail::script;
    QvmNativeStatePrefix state{};
    state.error_status = 1U;
    state.stop_flag = 0xFFFFFFFFU;
    observed_callback_count = 0U;

    Qvm_ExecuteInstructions(&state);

    require(state.stop_flag == 0U, "dispatcher clears its stop flag before checking status");
    require(observed_callback_count == 0U, "dispatcher returns without invoking a handler on error status");
}

} // namespace

int main()
{
    try {
        test_dispatches_bytes_and_stops_on_handler_flag();
        test_returns_on_error_status_after_clearing_stop_flag();
        std::cout << "QVM instruction dispatch checks passed.\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "QVM instruction dispatch checks failed: " << exception.what() << '\n';
        return 1;
    }
}
