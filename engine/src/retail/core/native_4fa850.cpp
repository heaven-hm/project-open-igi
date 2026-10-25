// Native: GraphList_PopHead_4FA850; retail VA 0x004FA850 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FA850-0x004FA884 (53 bytes, 1 block, 20 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004FA850-0x004FA884; radare2 6.2.0 pd/afij (53 bytes, 1 block, 20 instructions); called from 0x00509492 and the 0x004F9DF0/0x00509480 path.
// Behavior/CFG: one block, no branch. Reads the four-byte handle at list+0x04, passes it to the qfile driver slot 0x004B17C0, then calls 0x004B16D0(handle, &frame-8-bytes, 5) and returns the dword the callee wrote at frame+1 through arg2. Always reports 1 in AL.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 20-instruction listing; runtime UNOBSERVED. Both callees are transcribed in
//                engine/src/retail/qfile/ (QFile_InvokeDriverSlot_4B17C0, QFile_DeviceSlot10Call_4B16D0), so this file has no missing callee.
//                The out word is retail's deliberately unaligned read of frame+1 (0x004FA875 `mov 0x15(%esp),%ecx`, four bytes at offset one),
//                preserved here as an unaligned load. Retail leaves the upper EAX bits as the arg2 pointer and only AL carries the 1.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004B17C0: qfile driver slot invoke, transcribed in engine/src/retail/qfile/native_4b17c0.cpp (one argument).
std::int32_t QFile_InvokeDriverSlot_4B17C0(std::int32_t* context);

// 0x004B16D0: qfile device slot 10 call, transcribed in engine/src/retail/qfile/native_4b16d0.cpp (three arguments).
std::int32_t QFile_DeviceSlot10Call_4B16D0(void* context, std::int32_t argument2, std::int32_t argument3);

std::uint8_t NATIVE_DECL GraphList_PopHead_4FA850(void* list, std::int32_t* out)
{
    // 0x004FA850: sub $0x8,%esp - retail's 8-byte frame, of which 0x004B16D0 fills the first five bytes and the out word is the dword at
    // frame+1. Kept as two scalars rather than a byte array: an address-taken array makes the host compiler emit a
    // __stack_chk_guard/__stack_chk_fail pair that retail never calls, and the two scalars reproduce the same eight bytes.
    struct
    {
        std::uint32_t low;
        std::uint32_t high;
    } frame = {0U, 0U};

    // 0x004FA858: mov 0x4(%esi),%eax; 0x004FA85B: push %eax; 0x004FA85C: call 0x4b17c0.
    std::uint32_t const handle = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(list) + 0x04U);
    QFile_InvokeDriverSlot_4B17C0(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(handle)));

    // 0x004FA861: mov 0x4(%esi),%edx (re-read); 0x004FA864: lea 0x8(%esp),%ecx; 0x004FA868: push $0x5; 0x004FA86A: push %ecx;
    // 0x004FA86B: push %edx; 0x004FA86C: call 0x4b16d0.
    std::uint32_t const handle_again = *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(list) + 0x04U);
    QFile_DeviceSlot10Call_4B16D0(reinterpret_cast<void*>(static_cast<std::uintptr_t>(handle_again)),
                                  static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&frame)), 5);

    // 0x004FA871: mov 0x24(%esp),%eax (arg2); 0x004FA875: mov 0x15(%esp),%ecx (the callee's 5-byte write, read from frame+1);
    // 0x004FA87C: mov (%eax),%ecx... i.e. *out = that dword.
    *out = *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uint8_t*>(&frame) + 1U);
    return 1U;                                                                // 0x004FA87E: mov $0x1,%al
}                                                                             // 0x004FA884: ret
} // extern "C"

#undef NATIVE_DECL
