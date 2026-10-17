#include "igi/retail/core/frame_natives.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

extern "C" {

std::jmp_buf DAT_0054F8E0{};
std::uint8_t DAT_005C8DE8 = 0U;
igi::retail::core::FrameDispatchRecordView* DAT_00567C8C = nullptr;

igi::retail::core::FrameDispatchRecordView* IGI_RETAIL_CDECL Game_GetFrameDispatchRecord()
{
    return DAT_00567C8C;
}

void IGI_RETAIL_CDECL Game_SetFrameDispatchRecordValue(std::uint32_t value)
{
    DAT_00567C8C->unknown_0x2C = value;
}

void IGI_RETAIL_CDECL Game_SetLoopStatus(std::uint8_t value)
{
    DAT_005C8DE8 = value;
}

void IGI_RETAIL_CDECL Game_DispatchCurrentFrameCallback()
{
    if (setjmp(DAT_0054F8E0) == 0) {
        const std::uint16_t dispatch_index = Game_GetFrameDispatchRecord()->dispatch_table_index;
        const igi::retail::core::FrameDispatchCallback dispatch_callback = DAT_00A970E0[dispatch_index];
        dispatch_callback(Game_GetFrameDispatchRecord());
        return;
    }

    Game_SetLoopStatus(0U);
}

} // extern "C"

#undef IGI_RETAIL_CDECL
