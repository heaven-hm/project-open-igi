#define WIN32_LEAN_AND_MEAN
#include "igi/retail/script/qvm_stack_natives.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>
#include <windows.h>

namespace
{

std::uint32_t observed_callback_count = 0U;
std::array<std::uint32_t, 2U> observed_opcodes{};
bool observed_callback_arguments_valid = true;

class OriginalQvmInstructionDispatcher
{
  public:
    explicit OriginalQvmInstructionDispatcher(const std::vector<unsigned char>& executable_bytes)
    {
        IMAGE_DOS_HEADER dos_header{};
        if (executable_bytes.size() < sizeof(dos_header)) {
            throw std::runtime_error("Retail executable has a truncated DOS header.");
        }
        std::memcpy(&dos_header, executable_bytes.data(), sizeof(dos_header));
        if (dos_header.e_magic != IMAGE_DOS_SIGNATURE || dos_header.e_lfanew < 0) {
            throw std::runtime_error("Retail executable has an invalid DOS header.");
        }

        IMAGE_NT_HEADERS32 nt_headers{};
        const std::size_t nt_offset = static_cast<std::size_t>(dos_header.e_lfanew);
        if (nt_offset > executable_bytes.size() || sizeof(nt_headers) > executable_bytes.size() - nt_offset) {
            throw std::runtime_error("Retail executable has a truncated PE header.");
        }
        std::memcpy(&nt_headers, executable_bytes.data() + nt_offset, sizeof(nt_headers));
        if (nt_headers.Signature != IMAGE_NT_SIGNATURE ||
            nt_headers.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC ||
            nt_headers.FileHeader.Machine != IMAGE_FILE_MACHINE_I386 ||
            nt_headers.OptionalHeader.ImageBase != 0x00400000U) {
            throw std::runtime_error("Retail executable is not the expected x86 image.");
        }

        constexpr std::array<unsigned char, 59U> expected_native_bytes{
            0x56, 0x8B, 0x74, 0x24, 0x08, 0xC7, 0x46, 0x2C, 0x00, 0x00, 0x00, 0x00,
            0x8B, 0x46, 0x20, 0x85, 0xC0, 0x75, 0x26, 0x8B, 0x46, 0x14, 0x8B, 0x0E,
            0x33, 0xD2, 0x6A, 0x00, 0x8A, 0x14, 0x08, 0x8B, 0xC2, 0x50, 0x6A, 0x00,
            0x8D, 0x0C, 0x40, 0x56, 0xFF, 0x14, 0xCD, 0x08, 0x61, 0xA9, 0x00, 0x8B,
            0x46, 0x2C, 0x83, 0xC4, 0x10, 0x85, 0xC0, 0x74, 0xD3, 0x5E, 0xC3};
        const std::size_t section_table_offset = nt_offset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) +
            nt_headers.FileHeader.SizeOfOptionalHeader;
        const std::uint32_t relative_address = 0x004BCFD0U - nt_headers.OptionalHeader.ImageBase;
        const unsigned char* source_bytes = nullptr;
        for (std::size_t section_index = 0U; section_index < nt_headers.FileHeader.NumberOfSections; ++section_index) {
            const std::size_t section_offset = section_table_offset + section_index * sizeof(IMAGE_SECTION_HEADER);
            if (section_offset > executable_bytes.size() ||
                sizeof(IMAGE_SECTION_HEADER) > executable_bytes.size() - section_offset) {
                throw std::runtime_error("Retail executable has a truncated section table.");
            }
            IMAGE_SECTION_HEADER section{};
            std::memcpy(&section, executable_bytes.data() + section_offset, sizeof(section));
            if (relative_address < section.VirtualAddress) {
                continue;
            }
            const std::size_t section_offset_from_start = relative_address - section.VirtualAddress;
            if (section_offset_from_start > section.SizeOfRawData ||
                expected_native_bytes.size() > section.SizeOfRawData - section_offset_from_start) {
                continue;
            }
            const std::size_t file_offset = section.PointerToRawData + section_offset_from_start;
            if (file_offset > executable_bytes.size() ||
                expected_native_bytes.size() > executable_bytes.size() - file_offset) {
                throw std::runtime_error("QVM dispatcher bytes lie outside the executable file.");
            }
            source_bytes = executable_bytes.data() + file_offset;
            break;
        }
        if (source_bytes == nullptr ||
            std::memcmp(source_bytes, expected_native_bytes.data(), expected_native_bytes.size()) != 0) {
            throw std::runtime_error("Retail QVM dispatcher bytes differ from the verified source sequence.");
        }

        executable_memory_ = VirtualAlloc(
            nullptr, expected_native_bytes.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (executable_memory_ == nullptr) {
            throw std::runtime_error("Could not allocate retail dispatcher comparison memory.");
        }
        std::memcpy(executable_memory_, source_bytes, expected_native_bytes.size());
        const std::uint32_t relocated_table_address = static_cast<std::uint32_t>(
            reinterpret_cast<std::uintptr_t>(igi::retail::script::QvmOpcodeHandlerTable));
        std::memcpy(static_cast<unsigned char*>(executable_memory_) + 43U,
            &relocated_table_address, sizeof(relocated_table_address));

        DWORD old_protection = 0U;
        if (!VirtualProtect(executable_memory_, expected_native_bytes.size(), PAGE_EXECUTE_READ, &old_protection) ||
            !FlushInstructionCache(GetCurrentProcess(), executable_memory_, expected_native_bytes.size())) {
            VirtualFree(executable_memory_, 0U, MEM_RELEASE);
            executable_memory_ = nullptr;
            throw std::runtime_error("Could not prepare retail dispatcher comparison code.");
        }
    }

    ~OriginalQvmInstructionDispatcher()
    {
        if (executable_memory_ != nullptr) {
            VirtualFree(executable_memory_, 0U, MEM_RELEASE);
        }
    }

    OriginalQvmInstructionDispatcher(const OriginalQvmInstructionDispatcher&) = delete;
    OriginalQvmInstructionDispatcher& operator=(const OriginalQvmInstructionDispatcher&) = delete;

    using Function = void(__cdecl*)(igi::retail::script::QvmNativeStatePrefix*);

    [[nodiscard]] Function entry() const
    {
        return reinterpret_cast<Function>(executable_memory_);
    }

  private:
    void* executable_memory_ = nullptr;
};

void advance_or_stop(
    igi::retail::script::QvmNativeStatePrefix* state,
    std::uint32_t second_argument,
    std::uint32_t opcode,
    std::uint32_t fourth_argument)
{
    if (second_argument != 0U || fourth_argument != 0U || observed_callback_count >= observed_opcodes.size()) {
        observed_callback_arguments_valid = false;
        state->stop_flag = 1U;
        return;
    }
    observed_opcodes[observed_callback_count] = opcode;
    ++observed_callback_count;
    if (opcode == 0x15U) {
        ++state->instruction_offset;
    } else {
        state->stop_flag = 1U;
    }
}

void require(bool condition, const char* message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_dispatcher_matches_retail_instructions(const OriginalQvmInstructionDispatcher& original_dispatcher)
{
    using namespace igi::retail::script;
    const std::array<std::byte, 2U> code{std::byte{0x15U}, std::byte{0x16U}};
    Qvm_SetOpcodeHandler(0x15, 0, advance_or_stop);
    Qvm_SetOpcodeHandler(0x16, 0, advance_or_stop);

    QvmNativeStatePrefix original_state{};
    original_state.code_base = code.data();
    original_state.instruction_offset = 0U;
    original_state.error_status = 0U;
    original_state.stop_flag = 0xFFFFFFFFU;
    observed_callback_count = 0U;
    observed_opcodes = {};
    observed_callback_arguments_valid = true;
    original_dispatcher.entry()(&original_state);
    const auto original_opcodes = observed_opcodes;
    const std::uint32_t original_callback_count = observed_callback_count;
    const bool original_arguments_valid = observed_callback_arguments_valid;

    QvmNativeStatePrefix ported_state{};
    ported_state.code_base = code.data();
    ported_state.instruction_offset = 0U;
    ported_state.error_status = 0U;
    ported_state.stop_flag = 0xFFFFFFFFU;
    observed_callback_count = 0U;
    observed_opcodes = {};
    observed_callback_arguments_valid = true;
    Qvm_ExecuteInstructions(&ported_state);

    require(original_callback_count == observed_callback_count,
        "original and C++ dispatcher callback counts match");
    require(original_opcodes == observed_opcodes,
        "original and C++ dispatcher opcode sequence matches");
    require(original_arguments_valid && observed_callback_arguments_valid,
        "original and C++ dispatcher callback arguments are correct");
    require(original_state.instruction_offset == ported_state.instruction_offset &&
            original_state.error_status == ported_state.error_status &&
            original_state.stop_flag == ported_state.stop_flag,
        "original and C++ dispatcher state fields match");
    require(observed_callback_count == 2U && observed_opcodes[0U] == 0x15U && observed_opcodes[1U] == 0x16U,
        "the two-opcode fixture stops after its second callback");
}

void test_dispatcher_matches_retail_error_exit(const OriginalQvmInstructionDispatcher& original_dispatcher)
{
    using namespace igi::retail::script;
    QvmNativeStatePrefix original_state{};
    original_state.error_status = 1U;
    original_state.stop_flag = 0xFFFFFFFFU;
    observed_callback_count = 0U;
    observed_callback_arguments_valid = true;
    original_dispatcher.entry()(&original_state);

    QvmNativeStatePrefix ported_state{};
    ported_state.error_status = 1U;
    ported_state.stop_flag = 0xFFFFFFFFU;
    Qvm_ExecuteInstructions(&ported_state);

    require(original_state.stop_flag == ported_state.stop_flag && original_state.stop_flag == 0U,
        "original and C++ dispatcher clear stop state before the error exit");
    require(observed_callback_count == 0U,
        "original dispatcher does not call a handler when status is nonzero");
}

} // namespace

int main(int argument_count, char** arguments)
{
    try {
        if (argument_count != 2) {
            throw std::runtime_error("Pass the hash-verified retail igi.exe path.");
        }
        std::ifstream executable_stream(std::filesystem::path(arguments[1]), std::ios::binary);
        if (!executable_stream) {
            throw std::runtime_error("Could not open the retail igi.exe file.");
        }
        const std::vector<unsigned char> executable_bytes{
            std::istreambuf_iterator<char>(executable_stream), {}};
        if (executable_bytes.size() != 1384448U) {
            throw std::runtime_error("Retail executable size differs from the verified image.");
        }

        const OriginalQvmInstructionDispatcher original_dispatcher(executable_bytes);
        test_dispatcher_matches_retail_instructions(original_dispatcher);
        test_dispatcher_matches_retail_error_exit(original_dispatcher);
        std::cout << "Retail QVM dispatcher/C++ differential: dispatch and status cases matched.\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "QVM dispatcher differential failed: " << exception.what() << '\n';
        return 1;
    }
}
