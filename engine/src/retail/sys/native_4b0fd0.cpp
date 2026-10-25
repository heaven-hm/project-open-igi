// Native: Sys_LeaveRenderMode_4B0FD0; retail VA 0x004B0FD0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0FD0-0x004B0FF3 (36 bytes, 1 block, 0 edges, 13 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes (a104239400 8b542404 8b0d644aa900 48 52 a304239400 890c8508239400 e83effffff 59 c3); section 22.3 of the evidence sheet.
// Behavior/CFG: decrements the cursor word 0x00942304, pushes the current mode word 0x00A94A64 into the stack entry at 0x00942308 + (cursor-1)*4, and then activates the argument through Sys_SelectRenderMode (0x004B0F30). No branch, no return value.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. 0x00942304 is owned by sys/native_4b0dc0.cpp and the stack 0x00942308 by sys/native_4b0f90.cpp; both are referred to extern here. The store order is retail's: the argument is pushed for the call before the two stores, but it is only read after them.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00942304: the mode-stack cursor (owned by sys/native_4b0dc0.cpp).
extern std::int32_t g_942304;
// 0x00942308: the mode stack (owned by sys/native_4b0f90.cpp).
extern std::int32_t g_942308[10];
// 0x00A94A64: the current render-mode word (owned by sys/native_4b0f70.cpp).
extern std::int32_t g_a94a64;

// 0x004B0F30 (sys/native_4b0f30.cpp): records a mode word and copies its 0x208-byte block into the slot table.
void NATIVE_DECL Sys_SelectRenderMode_4B0F30(std::int32_t mode);

void NATIVE_DECL Sys_LeaveRenderMode_4B0FD0(std::int32_t previous_mode)
{
    const std::int32_t cursor = ::g_942304 - 1;                        // 0x004B0FD0-0x004B0FDF: mov eax,[0x00942304]; dec eax
    ::g_942304 = cursor;                                               // 0x004B0FE1: mov [0x00942304],eax
    ::g_942308[static_cast<std::uintptr_t>(cursor)] = ::g_a94a64;      // 0x004B0FD9/0x004B0FE6: mov ecx,[0x00A94A64]; mov [eax*4+0x00942308],ecx
    Sys_SelectRenderMode_4B0F30(previous_mode);                        // 0x004B0FE0/0x004B0FED: push edx; call 0x004B0F30; pop ecx
}

} // extern "C"

#undef NATIVE_DECL
