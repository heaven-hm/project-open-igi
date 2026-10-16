#include "test_framework.hpp"
#include "igi/logger.hpp"

#include <thread>
#include <vector>

TEST_CASE(LoggerAndExceptions, ExceptionHierarchyAndFormatting) {
    igi::logging::EngineException base_ex("TestSubsystem", "Something went wrong");
    ASSERT_EQ(base_ex.subsystem(), "TestSubsystem");
    ASSERT_EQ(base_ex.raw_message(), "Something went wrong");
    ASSERT_EQ(std::string(base_ex.what()), "[TestSubsystem] Something went wrong");

    igi::logging::AssetLoadException asset_ex("Missing texture file");
    ASSERT_EQ(asset_ex.subsystem(), "Asset");
    ASSERT_EQ(std::string(asset_ex.what()), "[Asset] Missing texture file");

    igi::logging::PhysicsException phys_ex("NaN velocity detected");
    ASSERT_EQ(phys_ex.subsystem(), "Physics");
    ASSERT_EQ(std::string(phys_ex.what()), "[Physics] NaN velocity detected");
}

TEST_CASE(LoggerAndExceptions, AssertThrowsAndCatches) {
    auto throwing_func = []() {
        throw igi::logging::AssetLoadException("Corrupt QSC header");
    };

    ASSERT_THROWS(throwing_func(), igi::logging::AssetLoadException);
    ASSERT_THROWS(throwing_func(), igi::logging::EngineException);
    ASSERT_THROWS(throwing_func(), std::exception);

    auto non_throwing_func = []() {
        int x = 42;
        (void)x;
    };
    ASSERT_NO_THROW(non_throwing_func());
}

TEST_CASE(LoggerAndExceptions, LoggerLevelsAndSnapshot) {
    auto& logger = igi::logging::Logger::instance();
    logger.clear_history();
    logger.set_min_level(igi::logging::LogLevel::Debug);

    ASSERT_EQ(static_cast<int>(logger.min_level()), static_cast<int>(igi::logging::LogLevel::Debug));

    // Trace should be filtered out
    logger.log(igi::logging::LogLevel::Trace, "Core", "Trace message - should be ignored");
    auto entries = logger.snapshot_recent_entries();
    ASSERT_EQ(entries.size(), 0u);

    // Debug and Info should be accepted
    logger.log(igi::logging::LogLevel::Debug, "Physics", "Debug physics tick");
    logger.log(igi::logging::LogLevel::Info, "Audio", "Sound stream opened");
    logger.log(igi::logging::LogLevel::Warning, "Renderer", "Fallback texture used");
    logger.log(igi::logging::LogLevel::Error, "AI", "Navigation path blocked");

    entries = logger.snapshot_recent_entries();
    ASSERT_EQ(entries.size(), 4u);
    ASSERT_EQ(entries[0].subsystem, "Physics");
    ASSERT_EQ(entries[0].message, "Debug physics tick");
    ASSERT_EQ(entries[1].subsystem, "Audio");
    ASSERT_EQ(entries[2].subsystem, "Renderer");
    ASSERT_EQ(entries[3].subsystem, "AI");
}

TEST_CASE(LoggerAndExceptions, ConcurrentLoggingThreadSafety) {
    auto& logger = igi::logging::Logger::instance();
    logger.clear_history();
    logger.set_min_level(igi::logging::LogLevel::Info);

    constexpr int kThreads = 8;
    constexpr int kLogsPerThread = 25;
    std::vector<std::thread> workers;
    workers.reserve(kThreads);

    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([t]() {
            for (int i = 0; i < kLogsPerThread; ++i) {
                IGI_LOG_INFO("Worker", "Thread " << t << " iteration " << i);
            }
        });
    }

    for (auto& w : workers) {
        w.join();
    }

    const auto entries = logger.snapshot_recent_entries(1000);
    ASSERT_EQ(entries.size(), static_cast<std::size_t>(kThreads * kLogsPerThread));
}
