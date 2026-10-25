// Native: Task_UnlimitedAmmo_410360 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00410360.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00410360-0x00410367 (8 bytes, 1 block, 0 edges, 2 instructions: c605 14e25600 01 / c3; the extent is measured from the image - the next byte 0x00410368 is nop padding).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the raw bytes decode as `mov byte [0x0056E214],1` then `ret`. The routine is the handler that 0x0040FE30 registers for the command-line option "UnlimitedAmmo" (push 0x00410360 at 0x0041005C, name 0x00538E74 at 0x00410061, AppMain_ParseCmdLineArgs 0x0048F360), and that caller discards the handler's result: app_main_commands.cpp transcribes the invocation as `(void)handler(token + option_length)`. The image holds 0x00 at 0x0056E214.
// Behavior/CFG: stores the byte 1 at 0x0056E214; no branch, no loop, no call, no stack use, and the single argument is not read.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Retail writes only that byte and leaves EAX exactly as the caller left it, so the retail body produces no return value of its own; the port returns 0 because C++ requires a value, which cannot change behaviour because the only caller discards the result. 0x0056E214 is owned by this file; no other file in the tree references it. The flag's meaning beyond "set by the UnlimitedAmmo option" is not established by this body. Domain: core/, because it is a handler of the 0x00402930 startup registrar 0x0040FE30.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0056E214: the byte the UnlimitedAmmo option sets; zero in the image.
std::uint8_t g_56e214 = 0x00U;

std::int32_t NATIVE_DECL Task_UnlimitedAmmo_410360(char*) // 0x00410360: the argument is never read
{
    ::g_56e214 = 1U;   // 0x00410360: mov byte [0x0056E214],1
    return 0;          // 0x00410367: ret (EAX is left as the caller passed it; 0x0048F360 discards it)
}

} // extern "C"

#undef NATIVE_DECL
