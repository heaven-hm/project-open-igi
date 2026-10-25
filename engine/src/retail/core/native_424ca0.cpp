// Native: Cursor_WinProcMouseMove_424CA0; retail VA 0x00424CA0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00424CA0-0x00424CD1 (50 bytes, 4 blocks, 5 edges, 13 instructions).
// Tools/evidence: radare2 6.2.0 af/pd over the exact bytes; pushed at 0x00424BFB by the Cursor registrar 0x00424B90 and registered through WinProc_RegisterHandler 0x0048F490 into the window-message chain whose entries carry the four-word handler type declared in include/igi/retail/core/native_0048f970.hpp.
// Behavior/CFG: window-procedure handler. Four-word cdecl frame, no prologue, plain ret: arg1 window [esp+4] never read, arg2 message [esp+8], arg3 wparam [esp+0xC] never read, arg4 lparam [esp+0x10]. When message == 0x200 (WM_MOUSEMOVE) it calls Display_GetScreenMode (0x0048F130) and, only when the result is 0, sign-extends the low and high 16-bit halves of lparam and stores them at 0x0057BC58 and 0x0057BC5C (the cursor x/y the reference candidate for 0x00424CE0 reads at 0x00424CF2/0x00424CFC). Both paths return 1 in EAX.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The two globals are unowned elsewhere in the tree and are defined here with the retail initial value: 0x0057BC58/0x0057BC5C lie beyond the .data raw bytes (raw .data ends at VA 0x00552000), so the image leaves them zero-initialised. The stored coordinates are signed because retail uses movswl; no other writer or reader of 0x0057BC58/0x0057BC5C exists in the image beyond this store and the 0x00424CE0 loads.
#include "dev_log.hpp"
#include "igi/retail/core/app_startup_globals.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0057BC58 / 0x0057BC5C: cursor x/y, written by this handler and read by 0x00424CE0. No other translation unit owns them.
std::int32_t g_57bc58 = 0;
std::int32_t g_57bc5c = 0;

std::intptr_t NATIVE_DECL Cursor_WinProcMouseMove_424CA0(void* /*window*/, std::uint32_t message,
                                                         std::uintptr_t /*wparam*/, std::intptr_t lparam)
{
    NATIVE_LOG_ENTER("Cursor_WinProcMouseMove_424CA0", "0x00424CA0");
    if (message == 0x200U)                       // 0x00424CA0/0x00424CA8: cmpl $0x200,0x8(%esp); jne
    {
        if (Display_GetScreenMode() == 0)        // 0x00424CAA call 0x0048F130; 0x00424CAF/0x00424CB1: test eax,eax; jne
        {
            const std::uint32_t raw = static_cast<std::uint32_t>(lparam);  // 0x00424CB3: mov 0x10(%esp),%eax
            ::g_57bc58 = static_cast<std::int32_t>(static_cast<std::int16_t>(raw & 0xFFFFU));  // 0x00424CB7: movswl %ax,%ecx
            ::g_57bc5c = static_cast<std::int32_t>(
                static_cast<std::int16_t>(static_cast<std::uint16_t>(raw >> 16)));            // 0x00424CBA/0x00424CBD: shr 0x10; movswl %ax,%edx
            // 0x00424CC0: mov %ecx,0x57bc58; 0x00424CC6: mov %edx,0x57bc5c
        }
    }

    return 1;                                    // 0x00424CCC: mov $0x1,%eax
}                                                // 0x00424CD1: ret

} // extern "C"

#undef NATIVE_DECL
