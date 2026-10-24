// IGI native QTask_OpenEvent - 0x00401050 - transcribed from retail igi.exe.
// Retail: cdecl, no arguments. _setjmp3(g_54f8e0, 0); on the direct return (0) it dispatches the handler table at 0xA970E0 indexed by the word at context + 0x1C with the context as argument,
// otherwise (longjmp return) it calls Loop_SetStatusByte(0).
#include <cstdint>
#include <cstring>

extern "C" int __cdecl Native_004A5000(void* buffer, unsigned count, ...);   // 0x004A5000
extern std::uint8_t g_54f8e0[];
extern void (__cdecl* g_a970e0[])(void* context);

void* __cdecl Game_GetContext(void);        // 0x00402880
void __cdecl Loop_SetStatusByte(std::int8_t value);   // 0x0048F480

void __cdecl QTask_OpenEvent(void)
{
    if (Native_004A5000(g_54f8e0, 0) == 0)
    {
        std::uint32_t index = *reinterpret_cast<std::uint16_t*>(static_cast<std::uint8_t*>(Game_GetContext()) + 0x1C);
        void (__cdecl* handler)(void*) = g_a970e0[index];
        handler(Game_GetContext());
        return;
    }
    Loop_SetStatusByte(0);
}
