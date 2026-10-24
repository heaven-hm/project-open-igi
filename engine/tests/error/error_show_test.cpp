#include "igi/retail/error/error_show.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

namespace {

void require(bool condition, std::string_view message)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

std::string g_displayed_text;
std::int32_t g_exit_calls = 0;

void capture_display(const char* text)
{
    g_displayed_text = text != nullptr ? text : "";
}

std::int32_t capture_exit()
{
    g_exit_calls += 1;
    return 77;
}

void test_game_data_symbol_load_formats_string()
{
    char buf[128]{};
    const std::int32_t len =
        igi::retail::error::sprintf(buf, "LOCAL:models/%s.mef", "crate");
    require(len == static_cast<std::int32_t>(std::strlen("LOCAL:models/crate.mef")),
            "sprintf returns formatted length");
    require(std::strcmp(buf, "LOCAL:models/crate.mef") == 0,
            "sprintf writes NUL-terminated formatted string");
}

void test_error_show_when_callbacks_are_null()
{
    igi::retail::error::Error_ResetGlobalsForTest();
    g_displayed_text.clear();
    g_exit_calls = 0;

    const std::int32_t ret =
        igi::retail::error::Debug_Error("Resource '%s' is not loaded.", "a.mef");
    require(ret == 0, "Error_Show returns 0 when g_93626c is null");
    require(g_displayed_text.empty(), "g_936268 not invoked when null");
}

void test_error_show_formats_prefix_and_tailcalls_exit()
{
    igi::retail::error::Error_ResetGlobalsForTest();
    auto& globals = igi::retail::error::Error_GetGlobals();
    globals.g_936268 = &capture_display;
    globals.g_93626c = &capture_exit;
    g_displayed_text.clear();
    g_exit_calls = 0;

    const std::int32_t ret =
        igi::retail::error::Debug_Error("Failed to load resource: '%s'", "LOCAL:models/prop.mef");
    require(ret == 77, "Error_Show returns result of tailcall g_93626c()");
    require(g_exit_calls == 1, "g_93626c called once");
    require(g_displayed_text == "Fatal error:\n\nFailed to load resource: 'LOCAL:models/prop.mef'",
            "Error_Show prepends 'Fatal error:\\n\\n' into 0x00A84640 and appends formatted message");
    require(std::strcmp(globals.g_a84640,
                        "Fatal error:\n\nFailed to load resource: 'LOCAL:models/prop.mef'")
                == 0,
            "0x00A84640 buffer holds the complete formatted fatal error");
}

} // namespace

int main()
{
    test_game_data_symbol_load_formats_string();
    test_error_show_when_callbacks_are_null();
    test_error_show_formats_prefix_and_tailcalls_exit();
    std::cout << "All retail Error_Show / String_VFormat / sprintf tests passed.\n";
    return EXIT_SUCCESS;
}
