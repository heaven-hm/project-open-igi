// Native: Sys_ResetRenderTables_4B0DC0; retail VA 0x004B0DC0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0DC0-0x004B0DFC (61 bytes, 1 block, 0 edges, 15 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes (57 b910040000 33c0 bfa050a900 f3ab b982000000 bf804ea900 f3ab b900010000 bf804aa900 f3ab c7050423940008000000 c705644aa900ffffffff 5f c3); section 22.3 of the evidence sheet.
// Behavior/CFG: zeroes four regions in retail's order and then writes two constants: 0x1040 bytes at 0x00A950A0 (the eight 0x208 render-mode slots), 0x208 bytes at 0x00A94E80 (the mode source block), 0x400 bytes at 0x00A94A80 (the 128 eight-byte registration records), then 8 into 0x00942304 and 0xFFFFFFFF into 0x00A94A64. The rep stosd counts are 0x410, 0x82 and 0x100 dwords; converted to byte counts they are 0x1040, 0x208 and 0x400, which is what the transcription clears. No branch, no call, no argument, no return value.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. All four targets are owned elsewhere in the tree and declared extern here; 0x00942304 has no other owner, so this file defines it and it starts zeroed. What 0x00942304 counts is not established, only that this reset writes 8 into it.
#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A950A0: render-mode slot table (owned by sys/native_4b0e00.cpp).
extern std::uint8_t g_a950a0[0x1244];
// 0x00A94E80: render-mode source block (owned by sys/native_4b0f30.cpp).
extern std::uint8_t g_a94e80[0x208];
// 0x00A94A80: 128 eight-byte registration records (owned by sys/native_4b0e70.cpp).
extern std::uint8_t g_a94a80[0x404];
// 0x00A94A64: current render-mode word (owned by sys/native_4b0f70.cpp).
extern std::int32_t g_a94a64;
// 0x00942304: the word this reset sets to 8. No other owner in the tree.
std::int32_t g_942304;

void NATIVE_DECL Sys_ResetRenderTables_4B0DC0(void)
{
    std::memset(&::g_a950a0[0], 0, 0x1040U);   // 0x004B0DC1-0x004B0DCD: mov ecx,0x410; xor eax,eax; mov edi,0x00A950A0; rep stosd
    std::memset(&::g_a94e80[0], 0, 0x208U);    // 0x004B0DCF-0x004B0DD9: mov ecx,0x82;  mov edi,0x00A94E80; rep stosd
    std::memset(&::g_a94a80[0], 0, 0x400U);    // 0x004B0DDB-0x004B0DE5: mov ecx,0x100; mov edi,0x00A94A80; rep stosd
    ::g_942304 = 8;                            // 0x004B0DE7: movl bashx8,0x00942304
    ::g_a94a64 = -1;                           // 0x004B0DF1: movl bashxFFFFFFFF,0x00A94A64
}   // 0x004B0DFB-0x004B0DFC: pop edi; ret

} // extern "C"

#undef NATIVE_DECL
