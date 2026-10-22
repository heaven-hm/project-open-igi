#include "igi/retail/core/frame_natives.hpp"

#include "igi/retail/core/task_types.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

extern "C" {

std::jmp_buf QTask_SetjmpFrame{};
std::int32_t AppContext_IsActive = 0;
igi::retail::core::FrameFlagHandler AppContext_OptionalHandler = nullptr;
std::int32_t AppContext_OptionalHandlerContext = 0;
igi::retail::core::FrameDispatchRecordView* Game_FrameDispatchRecord = nullptr;

// Native: Game_GetContext; retail VA 0x00402880; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x00402880-0x00402886, 6 bytes, digest 64bb29e43e21aa80d264c1d4f7588317866c098a20c52f27e2d415debf912d7c.
// Behavior/CFG: returns the global game-context pointer stored at 0x00567C8C. radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: VERIFIED; x86 global pointer load and EAX return, with no caller-visible writes; checked against the retail's own bytes under unicorn in 8 of 8 cases, not a live game process.
igi::retail::core::FrameDispatchRecordView* IGI_RETAIL_CDECL Game_GetContext()
{
    return Game_FrameDispatchRecord;
}

// Native: Flow_SetState; retail VA 0x004028A0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004028A0-0x004028AE, 14 bytes, digest fe8cd535469ff604fe50034d4a1f8f6cd5cb3ae5b15c571c28c4c375d43e860d.
// Behavior/CFG: writes `value` into the 32-bit field at offset +0x2C of the global context at 0x00567C8C and leaves that value in EAX; matches GameNatives/flow/004028A0-Flow_SetState.cpp return and live instruction listings.
// Status/limits: VERIFIED; valid x86 context, exact DWORD store and EAX bits; original source type unknown; checked against the retail's own bytes under unicorn in 8 of 8 cases.
std::uint32_t IGI_RETAIL_CDECL Flow_SetState(std::uint32_t value)
{
    Game_FrameDispatchRecord->unknown_0x2C = value;
    return value;
}

// Native: Loop_SetStatusByte; retail VA 0x0048F480; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F480-0x0048F48A, 10 bytes, digest ee3c22c2c4363e72b7199c5ea3ec80e0c5661a1354a9c93b5b3ff1b4fd0e1464.
// Behavior/CFG: stores `value` into the global loop status byte at 0x005C8DE8. radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: VERIFIED; high for exact instruction behavior, checked against the retail's own bytes under unicorn in 8 of 8 cases.
void IGI_RETAIL_CDECL Loop_SetStatusByte(std::uint8_t value)
{
    Loop_StatusByte = value;
}

// Native: QTask_OpenEvent; retail VA 0x00401050; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00401050-0x00401094, 68 bytes, digest c1c4e05efbb946828539e30c465e82356be70d32d878a62a0512201816b1ff93.
// Behavior/CFG: establishes the retail __setjmp3 frame boundary on 0x0054F8E0; when 0, fetches Game_GetContext() (0x00402880), reads the 16-bit dispatch index at +0x1C, and invokes the callback at 0x00A970E0[index](Game_GetContext()); when non-zero, calls the loop status setter (0x0048F480). radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Status/limits: VERIFIED; high for exact control flow, offsets, and call order, checked against the retail's own bytes under unicorn in 5 of 5 cases.
void IGI_RETAIL_CDECL QTask_OpenEvent()
{
    if (setjmp(QTask_SetjmpFrame) == 0) {
        const std::uint16_t dispatch_index = Game_GetContext()->dispatch_table_index;
        const igi::retail::core::FrameDispatchCallback dispatch_callback =
            QTask_Phase1HandlerTable()[dispatch_index];
        dispatch_callback(Game_GetContext());
        return;
    }

    Loop_SetStatusByte(0U);
}

// Native: AppContext_SetIsActive; retail VA 0x0048F140; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F140-0x0048F164, 37 bytes.
// Behavior/CFG: sign-extends the low byte of the argument into the global frame flag at 0x005C8BFC, then when the optional handler pointer at 0x005C8C18 is not null, calls it with the argument stored at 0x005C8C1C and returns its result; otherwise it returns the argument. radare2 2 blocks/1 edge; Binary Ninja 2 blocks.
// Status/limits: VERIFIED; the handler globals are read exactly as the retail reads them, checked against the retail's own bytes under unicorn in 8 of 8 cases.
std::int32_t IGI_RETAIL_CDECL AppContext_SetIsActive(std::int32_t value)
{
    AppContext_IsActive = static_cast<std::int8_t>(value);
    if (AppContext_OptionalHandler == nullptr)
    {
        return value;
    }
    return AppContext_OptionalHandler(value, AppContext_OptionalHandlerContext);
}

// Native: AppContext_GetIsActive; retail VA 0x0048F170; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F170-0x0048F175, 6 bytes.
// Behavior/CFG: returns the global frame flag at 0x005C8BFC. The retail main loop (0x0048F544) calls it between the frame clear (0x00498460) and the per-frame dispatch (0x00401050) and branches on the result: non-zero dispatches the frame, zero sleeps 500 ms. radare2 1 block/0 edges; Binary Ninja 1 block.
// Status/limits: VERIFIED; the load and the EAX return match the retail exactly, checked against the retail's own bytes under unicorn in 8 of 8 cases.
std::int32_t IGI_RETAIL_CDECL AppContext_GetIsActive()
{
    return AppContext_IsActive;
}

} // extern "C"

#undef IGI_RETAIL_CDECL
