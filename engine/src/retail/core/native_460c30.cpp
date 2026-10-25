// Native: Core_GetGlobal8_460C30; retail VA 0x00460C30 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00460C30-0x00460C35
//   (6 bytes, 1 block, 2 instructions: a0 16 f3 53 00 / c3).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image (1 block / 2 instructions); found by scanning the whole image for the
//   0x0053F316 immediate after the Lead asked whether that byte has any reader at all, and confirmed aligned here (`/r 0x53f316` reports this
//   site as a DATA reference at 0x00460C30). The same search shows the byte is written at 0x0045E492 (`mov byte [0x53F316],al`) and read at
//   0x0045E812 (`mov cl,[0x53F316]`) and 0x00460B5E (`mov dl,[0x53F316]`), so this is the fourth member of the 0x0053F314-0x0053F317 block.
// Behavior/CFG: returns the byte at 0x0053F316; no branch, no call, no stack use.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Only AL is written, so a C++ byte return zero-extends into EAX where retail leaves the upper
//   bits as the caller left them; the byte result is identical for every AL/uint8_t consumer. Ownership: no other translation unit defines
//   0x0053F316, and its writer at 0x0045E492 is not transcribed yet, so this file owns the byte for now and says so; the initial value is the
//   image's 0xFF at that address.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0053F316: the byte this native reads. Written at 0x0045E492 (not transcribed yet); read at 0x0045E812, 0x00460B5E and here.
std::uint8_t g_53f316 = 0xFFU;

std::uint8_t NATIVE_DECL Core_GetGlobal8_460C30()
{
    return ::g_53f316;   // 0x00460C30: mov al, byte [0x0053F316]
}                        // 0x00460C35: ret

} // extern "C"

#undef NATIVE_DECL
