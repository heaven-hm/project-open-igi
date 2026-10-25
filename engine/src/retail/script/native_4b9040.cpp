// Native: Qvm_StoreArgumentValue_4B9040; retail VA 0x004B9040 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B9040-0x004B9112 (211 bytes,
//   11 blocks, 70 instructions), bytes re-read from the image (81 ec 00 08 00 00 56 8b b4 24 0c 08 00 00 8b 46 04 48 74 ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image; callers: 0x004B8F60, 0x004B9556 and 0x004B9ADA - each pushes two arguments.
// Behavior/CFG: cdecl store helper with two stack arguments ([esp+4] expression node, [esp+8] target descriptor). The target's dword 4 selects
//   the store: 1 writes an integer read from node through 0x004B9120, sized by target dword 0x0C (1 -> byte store 0x004B9109, 2 -> word store
//   0x004B90EB, anything else -> dword store 0x004B90D6); 2 evaluates the node as a real through 0x004B9660 and stores eight bytes with
//   `fstpl` at the target pointer (0x004B90B0); 3 renders the node into a 0x800-byte stack buffer with
//   0x004B8CE0(node, buffer, 0x800, target dword 0x0C - 1) (0x004B906D-0x004B9075), copies the returned string into the target pointer with the
//   MSVC strncpy at 0x004A5500 using target dword 0x0C - 1 as the count (0x004B907D-0x004B9082, the count word left on the stack by the first
//   call's deliberately short 12-byte cleanup) and terminates target[count - 1] (0x004B908A-0x004B9090). Any other dword 4 value returns without
//   writing (0x004B9058/0x004B910B). The target pointer is read from target +0x18 in every store path.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED; STRUCTURALLY_VERIFIED against the instruction listing only. Link: needs 0x004B9120 (item 1 of
//   this wave, not yet transcribed), 0x004B9660 (`Qvm_EvaluateArgumentValue`, declared in qvm_execute.cpp and native_4b8a50.cpp, body not yet
//   transcribed) and 0x004B8CE0 (item 2 of this wave, not yet transcribed); the strncpy at 0x004A5500 is the linked MSVC CRT routine and is bound
//   to `std::strncpy` (see docs/fidelity/evidence/qvm-4a5500-strncpy.md). The target descriptor's field meanings beyond the observed type, count
//   and pointer roles are not established.
#include <cstddef>
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

// 0x004B9120 - integer expression evaluator; leaves its result in EAX.
std::int32_t NATIVE_DECL Qvm_EvaluateIntegerNode_4B9120(const void* node);
// 0x004B9660 - real expression evaluator; leaves its result in ST0.
long double NATIVE_DECL Qvm_EvaluateArgumentValue(const void* node);
// 0x004B8CE0 - renders an expression node into a caller buffer; returns the buffer pointer.
char* NATIVE_DECL Qvm_RenderNodeText_4B8CE0(const void* node, char* buffer, std::uint32_t buffer_size);

void NATIVE_DECL Qvm_StoreArgumentValue_4B9040(const void* node, void* target_object)
{
    std::uint8_t* const target = static_cast<std::uint8_t*>(target_object);
    std::int32_t const stored_type = *reinterpret_cast<std::int32_t*>(target + 4);      // 0x004B904E

    if (stored_type == 1) {
        // 0x004B90BA-0x004B9112: integer store, width from target +0x0C.
        std::int32_t const width = *reinterpret_cast<std::int32_t*>(target + 0x0C);     // 0x004B90BA
        void* const destination = *reinterpret_cast<void**>(target + 0x18);             // 0x004B9103/0x004B90E5/0x004B90D0
        if (width == 1) {
            // 0x004B90F6-0x004B9109: byte store.
            *static_cast<std::int8_t*>(destination) = static_cast<std::int8_t>(Qvm_EvaluateIntegerNode_4B9120(node));
        } else if (width == 2) {
            // 0x004B90E0-0x004B90EB: word store.
            *static_cast<std::int16_t*>(destination) = static_cast<std::int16_t>(Qvm_EvaluateIntegerNode_4B9120(node));
        } else {
            // 0x004B90CB-0x004B90D6: dword store.
            *static_cast<std::int32_t*>(destination) = Qvm_EvaluateIntegerNode_4B9120(node);
        }
    } else if (stored_type == 2) {
        // 0x004B909D-0x004B90B9: real store, `fstpl` of the evaluated long double into target +0x18.
        void* const destination = *reinterpret_cast<void**>(target + 0x18);             // 0x004B90AA
        const long double value = Qvm_EvaluateArgumentValue(node);                       // 0x004B90A5
        *static_cast<double*>(destination) = static_cast<double>(value);                 // 0x004B90B0: fstpl
    } else if (stored_type == 3) {
        // 0x004B905E-0x004B909C: render into the 0x800-byte stack buffer, then copy into the target pointer.
        char text[0x800];
        std::int32_t const count = *reinterpret_cast<std::int32_t*>(target + 0x0C);      // 0x004B905E
        // 0x004B9068-0x004B9075 pushes four words but 0x004B8CE0 takes three: the first word pushed is `count - 1`, which
        // survives the deliberately short `add $0xc` at 0x004B907A and becomes the cleanup argument-free strncpy count below.
        char* const rendered = Qvm_RenderNodeText_4B8CE0(node, text, 0x800U);             // 0x004B9068-0x004B9075
        char* const destination = static_cast<char*>(*reinterpret_cast<void**>(target + 0x18));   // 0x004B907E
        std::strncpy(destination, rendered, static_cast<std::size_t>(count - 1));        // 0x004B9082 (retail 0x004A5500)
        destination[count - 1] = 0;                                                      // 0x004B908A-0x004B9090
    }

    // 0x004B9112 is the shared epilogue; the retail leaves whatever the last call left in EAX there and all three observed callers
    // (0x004B8F65, 0x004B955B and 0x004B9ADF) reload their own values immediately, so the port declares no return value.
}

} // extern "C"

#undef NATIVE_DECL
