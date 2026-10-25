// Native: Task_GetHumanPlayerContainer_4108B0 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004108B0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004108B0-0x004108BC (13 bytes, 1 block, 0 edges, 4 instructions: 8b442408 / 8b0d9ce05600 / 8908 / c3; the next byte 0x004108BD is nop padding and 0x004108C0 is the next function's prologue).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the raw bytes decode as `mov eax,[esp+8]` (the second argument), `mov ecx,[0x0056E09C]`, `mov [eax],ecx`, `ret`. 0x0040FE30 stores the HumanPlayer task container at 0x0056E09C (Script_TaskContainer_Grow of Core_GetGlobal32_460C50, 0x004F1980, at 0x00410003) and links this routine as event 1's handler for that type through 0x00401400 at 0x0040FEB3.
// Behavior/CFG: publishes the container pointer held at 0x0056E09C through the pointer passed as the second argument; the first argument is not read, there is no branch, no call, no stack frame and no other memory write.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The second argument's slot is fixed by retail's [esp+8] read, so the declaration keeps two parameters even though the first is never read. 0x0040FE30 links this address as a FrameHandler, whose tree typedef is void(*)(void*), i.e. one parameter; the link table stores the address and retail's dispatcher supplies the arguments, so the cast does not change the stored value. 0x0056E09C is referenced by name and owned by core/native_40fe30.cpp. The container's meaning is only what 0x0040FE30 and 0x004F1980 establish. Domain: core/, because it is a handler of the 0x00402930 startup registrar 0x0040FE30.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0056E09C: the HumanPlayer script task container; owned by core/native_40fe30.cpp, zero in the image.
extern void* g_56e09c;

void NATIVE_DECL Task_GetHumanPlayerContainer_4108B0(void*, void** out) // 0x004108B0: the first argument is never read
{
    *out = ::g_56e09c;   // 0x004108B4/0x004108BA: mov ecx,[0x0056E09C]; mov [eax],ecx
}                        // 0x004108BC: ret

} // extern "C"

#undef NATIVE_DECL
