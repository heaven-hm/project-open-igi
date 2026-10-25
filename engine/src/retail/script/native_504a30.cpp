// Native: Script_RegisterCriteria; retail VA 0x00504A30-0x00504A8C (93 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (33c9 b840a6a700 803800 741a 83c00c 41 3da7a7b4 ... 7c ; then
//   68bc e65400 e8b7adfaff 83c404 ebfe ; found: 8b542404 8bc8 lea/shl 56 8b74240c 88... c6..04 89..08 b801.. d3e6 56 52 e8.. 83c408 89f0 5e c3).
//   The loop walks a 12-byte-stride table from 0xA7A640 up to but not including 0xA7A7B4, i.e. 31 slots, looking for a
//   zero flag byte. On exhaustion it reports "Unable to register AreaActivate criteria" (literal 0x0054E6BC, image
//   bytes read with ps) through 0x004AF7B0 = Debug_Error, then halts on `jmp 0x504A54`, a self-jump.
// Behavior/CFG: finds the first free slot, sets its flag byte to 1 and stores arg1 at +4 and arg2 at +8, calls
//   Script_QHashNodeRegister(arg1, 1 << index) - retail pushes the mask first and arg1 last, so cdecl arg1 is arg1 -
//   and returns the mask 1 << index in EAX. No `sub esp` and no callee-saved push beyond `push esi`, so there is no
//   stack frame and no 0x004A54A0 probe site; r2's `afi` reports stackframe: 12 for this VA, which is a derived value
//   and is not this body's frame. `mov esi,[esp+0xc]` is read after `push esi`, so it is the pre-push [esp+8], i.e. arg2.
// Status/limits: TRANSCRIBED; BUILD_VERIFIED (compiles and links). The self-jump halt is reproduced as an empty
//   infinite loop, which is the same non-terminating behaviour. Runtime UNOBSERVED. Domain: script/, because it
//   registers through Script_QHashNodeRegister and the table it fills is the criteria registry.
#include <cstdint>

extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value);
extern "C" int Debug_Error(const char* format, ...);

#if defined(_MSC_VER)
#define NATIVE_DECL __cdecl
#elif defined(__GNUC__)
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A7A640: the criteria registry, 31 records of 12 bytes. Retail walks it as bytes with a 0xC stride and reaches
// 0x00A7A7B4 at the end, which is 372 bytes = 31 * 12. The record layout is retail's encoded offsets, not a declared
// C++ type: +0 is the in-use flag byte, +4 and +8 are the two stored dwords.
std::uint8_t g_a7a640[31U * 12U] = {};

std::int32_t NATIVE_DECL Script_RegisterCriteria(void* const arg1, void* const arg2)
{
    std::uint32_t index = 0U;
    std::uint32_t cursor = 0U;

    // 0x00504A37: `cmp byte [eax],0` / `je` to the found path; `add eax,0xC` / `inc ecx` / `cmp eax,0xA7A7B4` / `jl`.
    for (;;) {
        if (g_a7a640[cursor] == 0U) {
            break;
        }
        cursor += 12U;
        ++index;
        if (cursor < 31U * 12U) {
            continue;
        }
        // 0x00504A47: the table is full. Retail reports and then halts on `jmp 0x504A54`, a jump to itself.
        Debug_Error("Unable to register AreaActivate criteria");
        for (;;) {
        }
    }

    // 0x00504A56: `mov edx,[esp+4]` is arg1, and `mov esi,[esp+0xc]` after `push esi` is the pre-push [esp+8], arg2.
    // 0x00504A65: the flag byte, then arg1 at +4 and arg2 at +8, in that order.
    g_a7a640[cursor] = 1U;
    *reinterpret_cast<std::uint32_t*>(&g_a7a640[cursor + 4U]) =
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(arg1));
    *reinterpret_cast<std::uint32_t*>(&g_a7a640[cursor + 8U]) =
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(arg2));

    // 0x00504A78: `mov esi,1` / `shl esi,cl`, where cl is the slot index, so the mask is 1 << index.
    std::uint32_t const mask = 1U << index;

    // 0x00504A7F: `push esi` (the mask) then `push edx` (arg1). Retail pushes the mask first, so cdecl arg1 is arg1.
    Script_QHashNodeRegister(arg1, reinterpret_cast<void*>(static_cast<std::uintptr_t>(mask)));

    return static_cast<std::int32_t>(mask); // 0x00504A89: `mov eax,esi`
}

} // extern "C"
