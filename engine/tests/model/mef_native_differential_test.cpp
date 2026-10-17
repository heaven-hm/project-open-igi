#define WIN32_LEAN_AND_MEAN
#include "igi/retail/model/mef_native_attachments.hpp"

#include <array>
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
    std::cout << "mef_native_differential_test requires Windows x86 runtime; skipped on non-Windows host.\n";
    return 0;
}
#else

namespace
{
// Verification tooling, not a game native: execute an exact, position-independent leaf body.
class OriginalLeafCode

{
  public:
    template <std::size_t byte_count>
    OriginalLeafCode(const std::vector<unsigned char>& binary,
                     std::uint32_t address,
                     const std::array<unsigned char, byte_count>& expected)
    {
        if (binary.size() < sizeof(IMAGE_DOS_HEADER))
        {
            throw std::runtime_error("Truncated DOS header");
        }
        IMAGE_DOS_HEADER dos_header{};
        std::memcpy(&dos_header, binary.data(), sizeof(dos_header));
        if (dos_header.e_magic != IMAGE_DOS_SIGNATURE || dos_header.e_lfanew < 0 ||
            static_cast<std::size_t>(dos_header.e_lfanew) > binary.size() - sizeof(IMAGE_NT_HEADERS32))
        {
            throw std::runtime_error("Invalid PE header location");
        }
        IMAGE_NT_HEADERS32 pe_header{};
        std::memcpy(&pe_header, binary.data() + dos_header.e_lfanew, sizeof(pe_header));
        if (pe_header.Signature != IMAGE_NT_SIGNATURE ||
            pe_header.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC ||
            pe_header.FileHeader.Machine != IMAGE_FILE_MACHINE_I386 ||
            pe_header.OptionalHeader.ImageBase != 0x00400000U)
        {
            throw std::runtime_error("Wrong retail image architecture or base");
        }
        const std::size_t sections_offset = static_cast<std::size_t>(dos_header.e_lfanew) + sizeof(DWORD) +
                                            sizeof(IMAGE_FILE_HEADER) +
                                            pe_header.FileHeader.SizeOfOptionalHeader;
        const std::uint32_t relative_address = address - pe_header.OptionalHeader.ImageBase;
        const unsigned char* source = nullptr;
        for (std::size_t section_index = 0; section_index < pe_header.FileHeader.NumberOfSections;
             ++section_index)
        {
            const std::size_t section_offset = sections_offset + section_index * sizeof(IMAGE_SECTION_HEADER);
            if (section_offset > binary.size() ||
                sizeof(IMAGE_SECTION_HEADER) > binary.size() - section_offset)
            {
                throw std::runtime_error("Truncated section table");
            }
            IMAGE_SECTION_HEADER section{};
            std::memcpy(&section, binary.data() + section_offset, sizeof(section));
            if (relative_address < section.VirtualAddress)
            {
                continue;
            }
            const std::size_t within_section = relative_address - section.VirtualAddress;
            if (within_section > section.SizeOfRawData || byte_count > section.SizeOfRawData - within_section)
            {
                continue;
            }
            const std::size_t file_offset = section.PointerToRawData + within_section;
            if (file_offset > binary.size() || byte_count > binary.size() - file_offset)
            {
                throw std::runtime_error("Native code outside file");
            }
            source = binary.data() + file_offset;
            break;
        }
        if (source == nullptr || std::memcmp(source, expected.data(), byte_count) != 0)
        {
            throw std::runtime_error("Retail native bytes differ from independently verified leaf body");
        }
        memory = VirtualAlloc(nullptr, byte_count, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (memory == nullptr)
        {
            throw std::runtime_error("Cannot allocate native comparison code");
        }
        std::memcpy(memory, source, byte_count);
        DWORD old_protection = 0;
        if (!VirtualProtect(memory, byte_count, PAGE_EXECUTE_READ, &old_protection) ||
            !FlushInstructionCache(GetCurrentProcess(), memory, byte_count))
        {
            VirtualFree(memory, 0, MEM_RELEASE);
            memory = nullptr;
            throw std::runtime_error("Cannot prepare original native instructions");
        }
    }

    ~OriginalLeafCode()
    {
        if (memory != nullptr)
        {
            VirtualFree(memory, 0, MEM_RELEASE);
        }
    }

    OriginalLeafCode(const OriginalLeafCode&) = delete;
    OriginalLeafCode& operator=(const OriginalLeafCode&) = delete;

    template <typename Function> Function entry() const
    {
        return reinterpret_cast<Function>(memory);
    }

  private:
    void* memory = nullptr;
};
} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        if (argument_count != 2)
        {
            throw std::runtime_error("Pass the verified retail igi.exe path");
        }
        std::ifstream input(std::filesystem::path(arguments[1]), std::ios::binary);
        if (!input)
        {
            throw std::runtime_error("Cannot open retail igi.exe");
        }
        const std::vector<unsigned char> binary{std::istreambuf_iterator<char>(input), {}};
        if (binary.size() != 1384448U)
        {
            throw std::runtime_error("Unexpected retail executable size");
        }
        const OriginalLeafCode pointer_code(
            binary, 0x004D0CF0U, std::array<unsigned char, 31>{0x8b, 0x44, 0x24, 0x04, 0x8b, 0x48, 0x0c, 0x8b,
                                                               0x44, 0x24, 0x08, 0x8b, 0x51, 0x0c, 0x8b, 0xc8,
                                                               0xc1, 0xe1, 0x04, 0x8b, 0x92, 0x8c, 0x00, 0x00,
                                                               0x00, 0x03, 0xc8, 0x8d, 0x04, 0x8a, 0xc3});
        const OriginalLeafCode count_code(
            binary,
            0x004D0D10U,
            std::array<unsigned char, 15>{
                0x8b, 0x44, 0x24, 0x04, 0x8b, 0x48, 0x0c, 0x8b, 0x51, 0x0c, 0x0f, 0xbf, 0x42, 0x7e, 0xc3});
        using namespace igi::retail::model;
        const auto original_pointer =
            pointer_code.entry<MefNativeAttachmentRecord*(__cdecl*) (MefNativeModelRecord*, std::int32_t)>();
        const auto original_count = count_code.entry<std::int32_t(__cdecl*)(MefNativeModelRecord*)>();
        std::array<MefNativeAttachmentRecord, 128> records{};
        MefNativeMeshHeader mesh{};
        MefNativeResourceRecord resource{};
        MefNativeModelRecord model{};
        resource.mesh_header = &mesh;
        model.resource_record = &resource;
        std::size_t pointer_cases = 0;
        for (std::size_t base_index = 0; base_index < records.size(); ++base_index)
        {
            mesh.attachments = records.data() + base_index;
            for (std::int32_t record_index = -static_cast<std::int32_t>(base_index);
                 record_index < static_cast<std::int32_t>(records.size() - base_index);
                 ++record_index)
            {
                if (Model_GetAttachment(&model, record_index) != original_pointer(&model, record_index))
                {
                    throw std::runtime_error("Original native/C++ ATTA pointer mismatch");
                }
                ++pointer_cases;
            }
        }
        for (std::int32_t count = -32768; count <= 32767; ++count)
        {
            mesh.attachment_count = static_cast<std::int16_t>(count);
            if (Model_GetAttachmentCount(&model) != original_count(&model))
            {
                throw std::runtime_error("Original native/C++ signed count mismatch");
            }
        }
        std::cout << "Original IGI x86 leaf instructions vs C++: " << pointer_cases
                  << " pointer cases and 65536 signed-count cases passed.\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Native differential test failed: " << exception.what() << '\n';
        return 1;
    }
}
#endif

