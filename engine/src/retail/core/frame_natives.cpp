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

// Native: Game_GetContext; retail VA 0x00402880; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x00402880-0x00402886, 6 bytes, digest 64bb29e43e21aa80d264c1d4f7588317866c098a20c52f27e2d415debf912d7c.
// Behavior/CFG: returns the global game-context pointer stored at 0x00567C8C. radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: VERIFIED; x86 global pointer load and EAX return, with no caller-visible writes; emulator evidence, not a live game process.
igi::retail::core::FrameDispatchRecordView* IGI_RETAIL_CDECL Game_GetContext()
{
    return DAT_00567C8C;
}

// Native: Flow_SetState; retail VA 0x004028A0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004028A0-0x004028AE, 14 bytes, digest fe8cd535469ff604fe50034d4a1f8f6cd5cb3ae5b15c571c28c4c375d43e860d.
// Behavior/CFG: writes `value` into the 32-bit field at offset +0x2C of the global context at 0x00567C8C and leaves that value in EAX; matches GameNatives/flow/004028A0-Flow_SetState.cpp return and live instruction listings.
// Status/limits: VERIFIED; valid x86 context, exact DWORD store and EAX bits; original source type unknown; emulator evidence.
std::uint32_t IGI_RETAIL_CDECL Flow_SetState(std::uint32_t value)
{
    DAT_00567C8C->unknown_0x2C = value;
    return value;
}

// Native: Loop_SetStatusByte; retail VA 0x0048F480; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F480-0x0048F48A, 10 bytes, digest ee3c22c2c4363e72b7199c5ea3ec80e0c5661a1354a9c93b5b3ff1b4fd0e1464.
// Behavior/CFG: stores `value` into the global loop status byte at 0x005C8DE8. radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact instruction behavior.
// Previous catalog alias: FX_SmokeEmit_27.
void IGI_RETAIL_CDECL Loop_SetStatusByte(std::uint8_t value)
{
    DAT_005C8DE8 = value;
}

// Native: QTask_OpenEvent; retail VA 0x00401050; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00401050-0x00401094, 68 bytes, digest c1c4e05efbb946828539e30c465e82356be70d32d878a62a0512201816b1ff93.
// Behavior/CFG: establishes the retail __setjmp3 frame boundary on 0x0054F8E0; when 0, fetches Game_GetContext() (0x00402880), reads the 16-bit dispatch index at +0x1C, and invokes the callback at 0x00A970E0[index](Game_GetContext()); when non-zero, calls FX_SmokeEmit_27(0) (0x0048F480). radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact control flow, offsets, and call order.
void IGI_RETAIL_CDECL QTask_OpenEvent()
{
    if (setjmp(DAT_0054F8E0) == 0) {
        const std::uint16_t dispatch_index = Game_GetContext()->dispatch_table_index;
        const igi::retail::core::FrameDispatchCallback dispatch_callback = DAT_00A970E0[dispatch_index];
        dispatch_callback(Game_GetContext());
        return;
    }

    FX_SmokeEmit_27(0U);
}

} // extern "C"

#undef IGI_RETAIL_CDECL
