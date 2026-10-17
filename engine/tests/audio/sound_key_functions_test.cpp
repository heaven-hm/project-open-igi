#include "igi/retail/audio/sound_key_functions.hpp"

#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string_view>

namespace {

int logger_call_count = 0;
const char* logger_format = nullptr;
const char* logger_argument = nullptr;

void require(bool condition, std::string_view message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void reset_logger_observation()
{
    logger_call_count = 0;
    logger_format = nullptr;
    logger_argument = nullptr;
}

void test_sound_key_extraction_preserves_case_and_stops_at_first_dot()
{
    char output[64] = "previous value";
    char input_path[] = "C:\\IGI\\SOUND\\Ak47_1.wav.backup";
    Sound_ExtractNameFromPath(output, input_path);
    require(std::strcmp(output, "Ak47_1") == 0, "extraction preserves case and stops at the first dot");
}

void test_sound_key_extraction_accepts_retail_path_separators()
{
    char output[64] = {};
    char colon_and_slash_path[] = "volume:folder/name.raw";
    Sound_ExtractNameFromPath(output, colon_and_slash_path);
    require(std::strcmp(output, "name") == 0, "colon and slash delimit path components");

    char backslash_path[] = "volume:folder\\name.raw";
    Sound_ExtractNameFromPath(output, backslash_path);
    require(std::strcmp(output, "name") == 0, "backslash delimits path components");
}

void test_sound_key_extraction_matches_invalid_name_paths()
{
    char output[64] = "stale";
    char empty_path[] = "";
    reset_logger_observation();
    Sound_ExtractNameFromPath(output, empty_path);
    require(output[0] == '\0', "empty path clears the output first");
    require(logger_call_count == 1, "empty path calls the retail logger");
    require(std::strcmp(logger_format, "SOUND - Invalid filename %s") == 0, "logger format matches the binary string");
    require(std::strcmp(logger_argument, "") == 0, "logger receives the invalid path");

    char dotless_path[] = "folder/name";
    Sound_ExtractNameFromPath(output, dotless_path);
    require(logger_call_count == 2, "dotless path calls the retail logger");
    require(std::strcmp(logger_argument, "folder/name") == 0, "logger receives the dotless path");

    char no_separator_path[] = "name.wav";
    Sound_ExtractNameFromPath(output, no_separator_path);
    require(output[0] == '\0', "a path with no separator leaves output empty");
    require(logger_call_count == 2, "a missing separator does not call the logger");
}

void test_retail_name_functions_mutate_and_hash()
{
    char sound_key[] = "abcz";
    const std::int32_t sound_key_hash = Sound_ComputeRollingNameHash(sound_key);
    require(std::strcmp(sound_key, "ABCZ") == 0, "sound-key function uppercases ASCII lowercase in place");
    require(sound_key_hash == 66, "listing-derived sound-key remainder for ABCZ is 66");

    char retail_name[] = "ak47_1";
    const std::int32_t name_hash = Sound_ComputeWeightedNameHash(retail_name);
    require(std::strcmp(retail_name, "AK47_1") == 0, "name hash function uppercases ASCII lowercase in place");
    require(name_hash == 307564, "weighted retail name hash for AK47_1 is 307564");
}

void test_rolling_name_hash_preserves_signed_byte_arithmetic()
{
    char name[] = { static_cast<char>(0x80), 'a', '\0' };
    const std::int32_t hash = Sound_ComputeRollingNameHash(name);
    require(static_cast<unsigned char>(name[0]) == 0x80U, "rolling hash preserves a high-bit byte");
    require(name[1] == 'A', "rolling hash uppercases following ASCII lowercase bytes");
    require(hash == 71, "rolling hash uses signed-byte arithmetic and unsigned x86 division");
}

void test_retail_name_functions_stop_at_nul()
{
    char name[] = {'b', 'c', '\0', 'd', 'e', '\0'};
    static_cast<void>(Sound_ComputeRollingNameHash(name));
    require(std::memcmp(name, "BC\0de\0", sizeof(name)) == 0, "uppercasing stops at the first NUL byte");

    char path[] = {'d', 'i', 'r', '/', 'n', 'a', 'm', 'e', '.', 'w', 'a', 'v', '\0', 'x', '\0'};
    char output[64] = {};
    Sound_ExtractNameFromPath(output, path);
    require(std::strcmp(output, "name") == 0, "path extraction stops at the first NUL byte");
}

} // namespace

extern "C" void Engine_LogFormattedMessage(const char* format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    logger_format = format;
    logger_argument = va_arg(arguments, const char*);
    va_end(arguments);
    ++logger_call_count;
}

int main()
{
    test_sound_key_extraction_preserves_case_and_stops_at_first_dot();
    test_sound_key_extraction_accepts_retail_path_separators();
    test_sound_key_extraction_matches_invalid_name_paths();
    test_retail_name_functions_mutate_and_hash();
    test_rolling_name_hash_preserves_signed_byte_arithmetic();
    test_retail_name_functions_stop_at_nul();

    std::cout << "All retail audio routine tests passed.\n";
    return EXIT_SUCCESS;
}
