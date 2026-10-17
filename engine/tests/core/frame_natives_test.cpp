#include "igi/retail/core/frame_natives.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string_view>

namespace {

int callback_count = 0;
void* callback_record = nullptr;

void require(bool condition, std::string_view message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void record_dispatch(void* record)
{
    ++callback_count;
    callback_record = record;
}

void abort_dispatch(void*)
{
    std::longjmp(DAT_0054F8E0, 1);
}

void reset_dispatch_observation()
{
    callback_count = 0;
    callback_record = nullptr;
}

void test_frame_getter_and_setter_match_observed_global_access()
{
    igi::retail::core::FrameDispatchRecordView record{};
    DAT_00567C8C = &record;

    require(Game_GetFrameDispatchRecord() == &record, "0x00402880 returns the global record pointer");
    Game_SetFrameDispatchRecordValue(0xA1B2C3D4U);
    require(record.unknown_0x2C == 0xA1B2C3D4U, "0x004028A0 writes a dword at offset 0x2c");
}

void test_frame_native_dispatches_by_record_word_at_offset_0x1c()
{
    igi::retail::core::FrameDispatchRecordView record{};
    DAT_00567C8C = &record;
    record.dispatch_table_index = 2U;
    DAT_00A970E0[2] = record_dispatch;
    DAT_005C8DE8 = 1U;
    reset_dispatch_observation();

    Game_DispatchCurrentFrameCallback();

    require(callback_count == 1, "frame dispatcher invokes one selected callback");
    require(callback_record == &record, "frame dispatcher passes the record returned by the getter");
    require(DAT_005C8DE8 == 1U, "normal frame dispatch leaves the global status byte unchanged");
}

void test_frame_abort_path_clears_the_retail_status_byte()
{
    igi::retail::core::FrameDispatchRecordView record{};
    DAT_00567C8C = &record;
    record.dispatch_table_index = 3U;
    DAT_00A970E0[3] = abort_dispatch;
    DAT_005C8DE8 = 1U;

    Game_DispatchCurrentFrameCallback();

    require(DAT_005C8DE8 == 0U, "nonlocal frame abort stores zero through 0x0048F480");
}

} // namespace

extern "C" igi::retail::core::FrameDispatchCallback DAT_00A970E0[4] = {};

int main()
{
    DAT_00A970E0[2] = record_dispatch;
    DAT_00A970E0[3] = abort_dispatch;

    test_frame_getter_and_setter_match_observed_global_access();
    test_frame_native_dispatches_by_record_word_at_offset_0x1c();
    test_frame_abort_path_clears_the_retail_status_byte();

    std::cout << "All retail frame-native tests passed.\n";
    return EXIT_SUCCESS;
}
