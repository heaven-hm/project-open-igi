#include "igi/retail/qfile/alias_resolve.hpp"

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

void reset_table()
{
    for (std::size_t i = 0; i < igi::retail::qfile::device_entry_count; ++i) {
        std::memset(&igi::retail::qfile::QFile_GetDeviceTable()[i], 0,
                    sizeof(igi::retail::qfile::DeviceEntry));
    }
}

// 0x004B1020 replaces the device prefix and appends the remainder.
void test_alias_substitution()
{
    reset_table();
    igi::retail::qfile::DeviceEntry& entry = igi::retail::qfile::QFile_GetDeviceTable()[0];
    std::strcpy(entry.name, "LOCAL:");
    std::strcpy(entry.alias, "levels/level1");
    entry.active = 1;

    char destination[128] = {};
    const std::int32_t result = igi::retail::qfile::QFile_AliasResolve(destination, "LOCAL:models/a.mef", 0);

    require(std::strcmp(destination, "levels/level1models/a.mef") == 0,
            "alias segment replaces the device prefix, remainder appended");
    require(result == 0, "no semicolon remainder means depth is not advanced");
    require(std::strcmp(igi::retail::qfile::QFile_GetResolvedAlias(), destination) == 0,
            "resolved text is stored in the global alias buffer");
}

// An unknown device copies the input unchanged.
void test_unknown_device_copies_input()
{
    reset_table();
    char destination[64] = {};
    const std::int32_t result = igi::retail::qfile::QFile_AliasResolve(destination, "MISSING:x", 0);
    require(std::strcmp(destination, "MISSING:x") == 0, "unknown alias is copied verbatim");
    require(result == 0, "unknown alias returns 0");
}

// depth selects the semicolon-separated segment; a remainder advances the depth.
void test_depth_selects_the_alias_segment()
{
    reset_table();
    igi::retail::qfile::DeviceEntry& entry = igi::retail::qfile::QFile_GetDeviceTable()[0];
    std::strcpy(entry.name, "LANGUAGE:");
    std::strcpy(entry.alias, "english;french");
    entry.active = 1;

    char destination[128] = {};
    const std::int32_t result = igi::retail::qfile::QFile_AliasResolve(destination, "LANGUAGE:movies", 1);
    require(std::strcmp(destination, "frenchmovies") == 0, "depth 1 selects the second segment");
    require(result == 0, "the last segment has no remainder");
}

// Inactive devices are skipped even when the name matches.
void test_inactive_entry_is_skipped()
{
    reset_table();
    igi::retail::qfile::DeviceEntry& entry = igi::retail::qfile::QFile_GetDeviceTable()[0];
    std::strcpy(entry.name, "LOCAL:");
    std::strcpy(entry.alias, "ignored");
    entry.active = 0;

    char destination[64] = {};
    igi::retail::qfile::QFile_AliasResolve(destination, "LOCAL:x", 0);
    require(std::strcmp(destination, "LOCAL:x") == 0, "inactive devices do not resolve");
}

} // namespace

int main()
{
    test_alias_substitution();
    test_unknown_device_copies_input();
    test_depth_selects_the_alias_segment();
    test_inactive_entry_is_skipped();

    std::cout << "All retail QFile alias-resolve tests passed.\n";
    return EXIT_SUCCESS;
}
