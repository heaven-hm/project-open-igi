#include "igi/retail/sys/resource_lookup.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

// 0x004AD73D
void test_ascii_upper_case()
{
    require(igi::retail::sys::Camera_ApplyWideScreen_28('a') == 'A', "a -> A");
    require(igi::retail::sys::Camera_ApplyWideScreen_28('z') == 'Z', "z -> Z");
    require(igi::retail::sys::Camera_ApplyWideScreen_28('/') == '/', "non-letters pass through");
    require(igi::retail::sys::Camera_ApplyWideScreen_28('A') == 'A', "A stays A");
}

// 0x004B5AF0
void test_upper_case_copy_writes_a_terminator()
{
    char destination[32] = {};
    char source[] = "local:/Models/x.mef";
    const std::int32_t returned = igi::retail::sys::Sys_ResolveSymbol_15(destination, source);
    require(returned == 0, "returns the terminating NUL");
    require(std::strcmp(destination, "LOCAL:/MODELS/X.MEF") == 0, "upper-cases the whole path");

    char empty_destination[4] = {'x', 'x', 'x', 'x'};
    char empty_source[] = "";
    igi::retail::sys::Sys_ResolveSymbol_15(empty_destination, empty_source);
    require(empty_destination[0] == 0, "an empty source writes a single NUL");
}

// 0x004B5B30
void test_hash_fold_and_modulus()
{
    std::uint32_t expected = 1U;
    for (const char* p = "LOCAL:/MODELS/X.MEF"; *p != 0; ++p) {
        expected = (expected + 1U) * static_cast<std::uint32_t>(static_cast<std::int8_t>(*p));
    }
    expected %= 0x3037U;
    require(igi::retail::sys::Sys_CheckHeap_15("LOCAL:/MODELS/X.MEF") == expected,
            "hash is (h + 1) * c folded, modulo 0x3037");
    require(igi::retail::sys::Sys_CheckHeap_15("") == 1U % 0x3037U,
            "empty string hashes to 1");
}

} // namespace

int main()
{
    test_ascii_upper_case();
    test_upper_case_copy_writes_a_terminator();
    test_hash_fold_and_modulus();

    std::cout << "All retail resource-lookup tests passed.\n";
    return EXIT_SUCCESS;
}
