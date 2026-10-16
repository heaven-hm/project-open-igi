#include "igi/platform_application.hpp"

#include <chrono>
#include <ctime>
#include <exception>
#include <iomanip>
#include <iostream>

namespace {

void log_startup_failure(const char* error_message) {
    const auto current_time = std::chrono::system_clock::now();
    const std::time_t current_time_seconds =
        std::chrono::system_clock::to_time_t(current_time);
    const auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time.time_since_epoch()) % 1000;
    const std::tm* local_time = std::localtime(&current_time_seconds);
    if (local_time == nullptr) {
        std::cerr << "[ERROR] [Application] " << error_message << '\n';
        return;
    }

    std::cerr << std::put_time(local_time, "%Y-%m-%dT%H:%M:%S") << '.'
              << std::setw(3) << std::setfill('0') << milliseconds.count()
              << " [ERROR] [Application] " << error_message << '\n';
}

} // namespace

int main(int argument_count, char** arguments) {
    try {
        return igi::platform::run_application(argument_count, arguments);
    } catch (const std::exception& exception) {
        log_startup_failure(exception.what());
    } catch (...) {
        log_startup_failure("Application startup failed with an unknown exception.");
    }

    return 1;
}
