#define WIN32_LEAN_AND_MEAN
#include "igi/retail/script/qvm_stack_natives.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#if defined(_WIN32)
#include <windows.h>
#endif

#if !defined(_WIN32)
int main()
{
    std::cout << "qvm_stack_native_differential_test requires Windows x86 runtime; skipped on non-Windows host.\n";
    return 0;
}
#else

namespace
{

class OriginalQvmPushInteger

{
  public:
    explicit OriginalQvmPushInteger(const std::vector<unsigned char>& executable_bytes)
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

        constexpr std::array<unsigned char, 42U> expected_bytes{
            0x8B, 0x44, 0x24, 0x04, 0x8B, 0x50, 0x18, 0x8B, 0x48, 0x1C, 0x4A, 0x49,
            0x89, 0x50, 0x18, 0x8B, 0x50, 0x28, 0x89, 0x48, 0x1C, 0xC7, 0x04, 0x8A,
            0x01, 0x00, 0x00, 0x00, 0x8B, 0x48, 0x18, 0x8B, 0x50, 0x24, 0x8B, 0x44,
            0x24, 0x08, 0x89, 0x04, 0x8A, 0xC3};
        const std::size_t section_table_offset = nt_offset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) +
            nt_headers.FileHeader.SizeOfOptionalHeader;
        const std::uint32_t relative_address = 0x004BDA00U - nt_headers.OptionalHeader.ImageBase;
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
                expected_bytes.size() > section.SizeOfRawData - section_offset_from_start) {
                continue;
            }
            const std::size_t file_offset = section.PointerToRawData + section_offset_from_start;
            if (file_offset > executable_bytes.size() ||
                expected_bytes.size() > executable_bytes.size() - file_offset) {
                throw std::runtime_error("QVM native bytes lie outside the executable file.");
            }
            source_bytes = executable_bytes.data() + file_offset;
            break;
        }
        if (source_bytes == nullptr || std::memcmp(source_bytes, expected_bytes.data(), expected_bytes.size()) != 0) {
            throw std::runtime_error("Retail QVM native bytes differ from the verified instruction sequence.");
        }

        executable_memory_ = VirtualAlloc(nullptr, expected_bytes.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (executable_memory_ == nullptr) {
            throw std::runtime_error("Could not allocate memory for the retail QVM native.");
        }
        std::memcpy(executable_memory_, source_bytes, expected_bytes.size());
        DWORD old_protection = 0U;
        if (!VirtualProtect(executable_memory_, expected_bytes.size(), PAGE_EXECUTE_READ, &old_protection) ||
            !FlushInstructionCache(GetCurrentProcess(), executable_memory_, expected_bytes.size())) {
            VirtualFree(executable_memory_, 0U, MEM_RELEASE);
            executable_memory_ = nullptr;
            throw std::runtime_error("Could not prepare the retail QVM native for execution.");
        }
    }

    ~OriginalQvmPushInteger()
    {
        if (executable_memory_ != nullptr) {
            VirtualFree(executable_memory_, 0U, MEM_RELEASE);
        }
    }

    OriginalQvmPushInteger(const OriginalQvmPushInteger&) = delete;
    OriginalQvmPushInteger& operator=(const OriginalQvmPushInteger&) = delete;

    using Function = void(__cdecl*)(igi::retail::script::QvmNativeStatePrefix*, std::uint32_t);

    [[nodiscard]] Function entry() const
    {
        return reinterpret_cast<Function>(executable_memory_);
    }

  private:
    void* executable_memory_ = nullptr;
};

} // namespace

int main(int argument_count, char** arguments)
{
    try {
        if (argument_count != 2) {
            throw std::runtime_error("Pass the SHA-256-verified retail igi.exe path.");
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

        const OriginalQvmPushInteger original_native(executable_bytes);
        std::size_t matched_cases = 0U;
        for (std::uint32_t value_stack_index = 1U; value_stack_index < 8U; ++value_stack_index) {
            for (std::uint32_t type_stack_index = 1U; type_stack_index < 8U; ++type_stack_index) {
                const std::uint32_t value = 0xA5C30000U ^ (value_stack_index << 8U) ^ type_stack_index;
                std::array<std::uint32_t, 8U> original_values{};
                std::array<std::uint32_t, 8U> original_types{};
                std::array<std::uint32_t, 8U> ported_values{};
                std::array<std::uint32_t, 8U> ported_types{};
                original_values.fill(0x55555555U);
                original_types.fill(0xAAAAAAAAU);
                ported_values = original_values;
                ported_types = original_types;

                igi::retail::script::QvmNativeStatePrefix original_state{};
                original_state.value_stack_index = static_cast<std::int32_t>(value_stack_index);
                original_state.type_stack_index = static_cast<std::int32_t>(type_stack_index);
                original_state.value_stack = original_values.data();
                original_state.type_stack = original_types.data();
                auto ported_state = original_state;
                ported_state.value_stack = ported_values.data();
                ported_state.type_stack = ported_types.data();

                original_native.entry()(&original_state, value);
                igi::retail::script::Qvm_PushInteger(&ported_state, value);
                if (original_state.value_stack_index != ported_state.value_stack_index ||
                    original_state.type_stack_index != ported_state.type_stack_index ||
                    original_values != ported_values || original_types != ported_types) {
                    throw std::runtime_error("Original native and C++ state differ for a stack fixture.");
                }
                ++matched_cases;
            }
        }

        std::cout << "Retail x86 QVM native/C++ differential: " << matched_cases << " stack-state cases matched.\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "QVM native differential test failed: " << exception.what() << '\n';
        return 1;
    }
}
#endif

