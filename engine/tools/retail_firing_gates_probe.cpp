#include <windows.h>

#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

namespace
{

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void release_code(void* allocation)
{
    if (allocation != nullptr)
    {
        VirtualFree(allocation, 0, MEM_RELEASE);
    }
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        require(argument_count == 2, "Supply original retail executable");
        std::ifstream input(arguments[1], std::ios::binary);
        require(static_cast<bool>(input), "Cannot read retail executable");
        const std::vector<char> image{std::istreambuf_iterator<char>(input), {}};
        constexpr std::size_t handler_size = 3035;
        require(image.size() > 0x78ba0 + handler_size, "Truncated executable");
        std::unique_ptr<void, decltype(&release_code)> code(
            VirtualAlloc(nullptr, handler_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE), release_code);
        require(code != nullptr, "Cannot allocate handler");
        std::memcpy(code.get(), image.data() + 0x78ba0, handler_size);
        DWORD previous_protection = 0;
        require(VirtualProtect(code.get(), handler_size, PAGE_EXECUTE_READ, &previous_protection) != 0,
                "Cannot protect handler");
        require(FlushInstructionCache(GetCurrentProcess(), code.get(), handler_size) != 0,
                "Cannot flush retail instruction cache");
        require(std::memcmp(code.get(), image.data() + 0x78ba0, handler_size) == 0,
                "Private Gun_Fire instructions differ from the original executable");
        using Handler = void(__cdecl*)(void*, void*);
        const auto handler = reinterpret_cast<Handler>(code.get());
        std::size_t checked_scenarios = 0;
        for (const int burst_limit : {1, 3, 30})
        {
            for (const bool trigger_held : {false, true})
            {
                std::array<std::uint32_t, 128> gun{};
                std::array<std::uint32_t, 442> weapon{};
                std::array<std::uint32_t, 8> message{};
                const std::array<std::uint32_t, 6> transform{};
                const std::array<std::uint32_t, 8> orientation{};
                weapon[0x64c / 4] = burst_limit;
                gun[0x128 / 4] = reinterpret_cast<std::uintptr_t>(weapon.data());
                gun[0x140 / 4] = burst_limit;
                gun[0x144 / 4] = 30;
                message[0] = trigger_held ? 1 : 0;
                message[1] = reinterpret_cast<std::uintptr_t>(transform.data());
                message[2] = reinterpret_cast<std::uintptr_t>(orientation.data());
                handler(gun.data(), message.data());
                require((message[0] & 0xff00) == 0, "Retail burst/release gate did not clear firing result");
                require(gun[0x140 / 4] == (trigger_held ? static_cast<unsigned>(burst_limit) : 0),
                        "Retail trigger release did not reset burst count");
                require(gun[0x144 / 4] == 30,
                        "Blocked original firing consumed ammunition");
                ++checked_scenarios;
            }
        }
        std::cout << "Retail blocked firing/release paths passed: " << checked_scenarios << " scenarios\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << '\n';
        return 1;
    }
}
