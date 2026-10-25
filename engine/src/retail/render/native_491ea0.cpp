// Native: Display_DriverDispatch_491EA0 (placeholder until VERIFIED); retail VA 0x00491EA0-0x00491EE3 (68 bytes, 3 blocks, 3 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) bytes 83ec10 8b4c2414 8d542400 894c2404 8b4c2418 89542408 8b1534235400 894c240c 33c0 8b0c95844ea900 89442400 85c9 740e 8d442404 50 ffd1 8b442404 83c404 83c410 c3; radare2 6.2.0 af/afij (68 B, 3 blocks, 20 instructions, 3 edges) and afbj (block 0x00491EA0 50 B/13 i -> 0x00491EE0 taken / 0x00491ED2 fall-through, block 0x00491ED2 14 B/5 i, block 0x00491EE0 2 i); the image dword at 0x00542334 is 0xFFFFFFFF.
// Behavior/CFG: builds a three-dword descriptor {argument1, address of a zeroed dword, argument2} on its own frame, loads the active backend id from 0x00542334, reads the render-backend init pointer from 0x00A94E84 + id*4, returns 0 without calling when that pointer is null, and otherwise calls it with the descriptor address and returns the descriptor's first dword, which the callee may have replaced.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 20-instruction listing and the per-block counts; runtime UNOBSERVED. The dispatch table 0x00A94E84 is data - sixteen function pointers, all zero in the image - and is already declared by the tree as igi::retail::dynobj::g_a94e84[0x10], so this file declares no table and synthesizes no dispatch helper. The id slot 0x00542334 is declared here as g_542334 with its image value -1 because retail also writes it at 0x004917AB and reads it at 0x00491E60; note that render/native_00491e60.cpp reads the same slot through a raw address literal instead of this object. Retail bounds-checks nothing, so the image value -1 makes the load read 0x00A94E80, the dword before the table; the port keeps the same index arithmetic and adds no guard.
#include "dev_log.hpp"
#include <cstdint>

#include "igi/retail/dynobj/local_models_mef.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Retail 0x00542334: the active render-backend id. Written by 0x004917AB (not transcribed yet), read by 0x00491E60 and here.
std::int32_t g_542334 = -1;

std::int32_t NATIVE_DECL Display_DriverDispatch_491EA0(void* const argument1, std::int32_t const argument2)
{
    NATIVE_LOG_ENTER("Display_DriverDispatch_491EA0", "0x00491EA0");
    struct DriverDescriptor
    {
        void* argument1;                // retail frame +0x04
        void* zero_slot_address;        // retail frame +0x08
        void* argument2;                // retail frame +0x0C
    };

    std::uint32_t zero_slot = 0U;       // 0x00491EC1: xor %eax,%eax; 0x00491ECA: mov %eax,0x0(%esp)
    DriverDescriptor descriptor;
    descriptor.argument1 = argument1;                                     // 0x00491EA3/0x00491EAB: mov 0x14(%esp),%ecx; mov %ecx,0x4(%esp)
    descriptor.zero_slot_address = &zero_slot;                            // 0x00491EA7/0x00491EB3: lea 0x0(%esp),%edx; mov %edx,0x8(%esp)
    igi::retail::dynobj::ModelBackendInitFn const handler =               // 0x00491EB7/0x00491EC3: mov 0x542334,%edx; mov 0xa94e84(,%edx,4),%ecx
        igi::retail::dynobj::g_a94e84[g_542334];
    descriptor.argument2 = reinterpret_cast<void*>(                       // 0x00491EAF/0x00491EBD: mov 0x18(%esp),%ecx; mov %ecx,0xc(%esp)
        static_cast<std::uintptr_t>(argument2));

    std::int32_t result = 0;                                              // 0x00491EC1: xor %eax,%eax
    if (handler != nullptr)                                              // 0x00491ECE/0x00491ED0: test %ecx,%ecx; je 0x00491EE0
    {
        handler(reinterpret_cast<void**>(&descriptor));                  // 0x00491ED2/0x00491ED6/0x00491ED7: lea 0x4(%esp),%eax; push %eax; call *%ecx
        result = static_cast<std::int32_t>(                              // 0x00491ED9: mov 0x4(%esp),%eax
            reinterpret_cast<std::uintptr_t>(descriptor.argument1));
    }
    return result;                                                       // 0x00491EDD/0x00491EE0/0x00491EE3: add $0x4,%esp; add $0x10,%esp; ret
}

} // extern "C"

#undef NATIVE_DECL
