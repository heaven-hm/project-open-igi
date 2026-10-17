#include "igi/retail/diagnostics/log_record.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string_view>

namespace
{

void require(bool condition, std::string_view description)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << description << '\n';
        std::exit(EXIT_FAILURE);
    }
}

} // namespace

int main()
{
    using namespace std::chrono;
    using igi::retail::diagnostics::Log_FormatTimestampedRecord;
    using igi::retail::diagnostics::LogLevel;

    const auto timestamp = sys_days{year{2026} / October / 17}
        + hours{5} + minutes{4} + seconds{3} + milliseconds{27};
    const std::string formatted_record = Log_FormatTimestampedRecord(
        LogLevel::error,
        timestamp,
        "resource",
        "Error in resource file: 'LEVEL1.RES'");

    require(formatted_record ==
        "[2026-10-17T05:04:03.027Z] [ERROR] [resource] Error in resource file: 'LEVEL1.RES'\n",
        "timestamped retail-boundary diagnostics retain stable UTC formatting and the source error text");

    std::cout << "Timestamped diagnostic record test passed.\n";
    return EXIT_SUCCESS;
}
