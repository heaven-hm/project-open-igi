#include <windows.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace
{

using Handler = void(__cdecl*)(std::uint32_t*);

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

class RetailImage final
{
  public:
    explicit RetailImage(const char* path)
    {
        std::ifstream input(path, std::ios::binary);
        require(static_cast<bool>(input), "Cannot open retail executable");
        const std::vector<char> file{std::istreambuf_iterator<char>(input), {}};
        require(file.size() >= sizeof(IMAGE_DOS_HEADER), "Truncated DOS header");
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(file.data());
        require(dos->e_magic == IMAGE_DOS_SIGNATURE && dos->e_lfanew >= 0 &&
                    static_cast<std::size_t>(dos->e_lfanew) <= file.size() - sizeof(IMAGE_NT_HEADERS32),
                "Invalid retail DOS header");
        const auto* header = reinterpret_cast<const IMAGE_NT_HEADERS32*>(file.data() + dos->e_lfanew);
        require(header->Signature == IMAGE_NT_SIGNATURE && header->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 &&
                    header->OptionalHeader.ImageBase == 0x400000 &&
                    header->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC &&
                    header->OptionalHeader.SizeOfImage < 32 * 1024 * 1024,
                "Expected original PE32 retail image at 0x400000");
        const auto* sections = IMAGE_FIRST_SECTION(header);
        const auto section_offset = reinterpret_cast<const char*>(sections) - file.data();
        require(section_offset >= 0 && static_cast<std::size_t>(section_offset) <= file.size() &&
                    header->FileHeader.NumberOfSections <=
                        (file.size() - section_offset) / sizeof(IMAGE_SECTION_HEADER),
                "Invalid retail section table");
        for (std::size_t section_index = 0; section_index < header->FileHeader.NumberOfSections; ++section_index)
        {
            const auto& section = sections[section_index];
            require(section.PointerToRawData <= file.size() &&
                        section.SizeOfRawData <= file.size() - section.PointerToRawData &&
                        section.VirtualAddress <= header->OptionalHeader.SizeOfImage &&
                        section.SizeOfRawData <= header->OptionalHeader.SizeOfImage - section.VirtualAddress,
                    "Invalid retail section bounds");
        }
        image_ = VirtualAlloc(nullptr, header->OptionalHeader.SizeOfImage,
                              MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        require(image_ != nullptr, "Cannot allocate retail instruction image");
        for (std::size_t section_index = 0; section_index < header->FileHeader.NumberOfSections; ++section_index)
        {
            const auto& section = sections[section_index];
            std::memcpy(static_cast<char*>(image_) + section.VirtualAddress,
                        file.data() + section.PointerToRawData, section.SizeOfRawData);
        }
        DWORD previous_protection = 0;
        if (!VirtualProtect(image_, header->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READ, &previous_protection))
        {
            VirtualFree(image_, 0, MEM_RELEASE);
            image_ = nullptr;
            throw std::runtime_error("Cannot protect retail instruction image");
        }
        FlushInstructionCache(GetCurrentProcess(), image_, header->OptionalHeader.SizeOfImage);
        std::cerr << "Original instruction image mapped at " << image_
                  << "; valid integer paths use unchanged relative calls; no imports or entrypoint executed\n";
    }

    ~RetailImage()
    {
        if (image_ != nullptr)
        {
            VirtualFree(image_, 0, MEM_RELEASE);
        }
    }

    RetailImage(const RetailImage&) = delete;
    RetailImage& operator=(const RetailImage&) = delete;

    [[nodiscard]] std::uintptr_t resolve(std::uintptr_t original_address) const
    {
        require(original_address >= 0x400000 && original_address < 0x500000,
                "Handler address is outside the verified instruction range");
        return reinterpret_cast<std::uintptr_t>(image_) + original_address - 0x400000;
    }

  private:
    void* image_{};
};

void execute_operand(std::uint8_t opcode, std::uint32_t operand, std::size_t width,
                     std::uintptr_t address, std::ostream& trace)
{
    std::array<std::uint8_t, 5> code{opcode};
    for (std::size_t byte_index = 0; byte_index < width; ++byte_index)
    {
        code[byte_index + 1] = static_cast<std::uint8_t>(operand >> (byte_index * 8));
    }
    std::array<std::uint32_t, 9> values;
    std::array<std::uint32_t, 9> tags;
    values.fill(0xcafebabe);
    tags.fill(0xdeadbeef);
    std::array<std::uint32_t, 12> state{};
    state[0] = reinterpret_cast<std::uintptr_t>(code.data());
    state[6] = 8;
    state[7] = 8;
    state[9] = reinterpret_cast<std::uintptr_t>(values.data());
    state[10] = reinterpret_cast<std::uintptr_t>(tags.data());
    auto expected_state = state;
    expected_state[5] = static_cast<std::uint32_t>(width + 1);
    expected_state[6] = 7;
    expected_state[7] = 7;
    reinterpret_cast<Handler>(address)(state.data());
    require(state == expected_state && tags[7] == 1, "Retail handler changed unexpected VM state");
    for (std::size_t slot_index = 0; slot_index < values.size(); ++slot_index)
    {
        if (slot_index != 7)
        {
            require(values[slot_index] == 0xcafebabe && tags[slot_index] == 0xdeadbeef,
                    "Retail handler changed stack canaries");
        }
    }
    std::int32_t signed_value = 0;
    std::memcpy(&signed_value, &values[7], sizeof(signed_value));
    trace << static_cast<unsigned>(opcode) << ',' << operand << ',' << signed_value << '\n';
    require(static_cast<bool>(trace), "Cannot write retail operand trace");
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        require(argument_count == 3, "Usage: retail-operands.exe original-igi.exe output.csv");
        const RetailImage image(arguments[1]);
        std::ofstream trace(arguments[2], std::ios::binary);
        require(static_cast<bool>(trace), "Cannot create retail operand trace");
        for (std::uint32_t operand = 0; operand < 256; ++operand)
        {
            execute_operand(3, operand, 1, image.resolve(0x4be870), trace);
        }
        for (std::uint32_t operand = 0; operand < 65536; ++operand)
        {
            execute_operand(4, operand, 2, image.resolve(0x4be8b0), trace);
        }
        for (const std::uint32_t operand :
             {0u, 1u, 127u, 128u, 32767u, 32768u, 65535u, 0x7fffffffu, 0x80000000u, 0xffffffffu})
        {
            execute_operand(2, operand, 4, image.resolve(0x4be8f0), trace);
        }
        trace.close();
        require(static_cast<bool>(trace), "Cannot flush retail operand trace");
        std::cout << "Retail QVM instruction execution passed: 65802 operands, VM state and stack canaries\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Retail QVM instruction execution failed: " << exception.what() << '\n';
        return 1;
    }
}
