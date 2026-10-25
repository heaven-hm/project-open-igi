// Native: Resource_Flush; retail VA 0x004B63D0-0x004B6413 (68 bytes, 9 blocks, 15 edges, 25 instructions in the reachable set). The name is evidenced twice: the manifest record for this VA carries it, and the fatal path formats "Resource_Flush() : Couldn't locate resource at %p." at 0x00547150.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000; .text VA 0x00401000 raw 0x1000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret) over the exact image. The extent is bounded by the `ret` at 0x004B6413, and TWELVE alignment nops `90` x12 follow it (the next entry 0x004B6420 starts `push %esi`). Exact bytes 8B 0D 38 3E 94 00 8B 54 24 04 8B 01 85 C0 74 0D 39 51 34 74 18 8B C8 8B 00 85 C0 75 F3 52 68 50 71 54 00 E8 B8 93 FF FF 83 C4 08 EB FE 8B 41 24 83 F8 03 74 0E 83 F8 01 74 09 51 E8 80 F8 FF FF 83 C4 04 C3 = `mov 0x00943E38,%ecx` / `mov 0x4(%esp),%edx` / `mov (%ecx),%eax` / `test %eax,%eax` / `je 0x004B63ED` / `cmp %edx,0x34(%ecx)` / `je 0x004B63FD` / `mov %eax,%ecx` / `mov (%eax),%eax` / `test %eax,%eax` / `jne 0x004B63E0` / `push %edx` / `push $0x00547150` / `call 0x004AF7B0` / `add $0x8,%esp` / `jmp 0x004B63FB` / `mov 0x24(%ecx),%eax` / `cmp $0x3,%eax` / `je 0x004B6413` / `cmp $0x1,%eax` / `je 0x004B6413` / `push %ecx` / `call 0x004B5C90` / `add $0x4,%esp` / `ret`. Every call displacement resolves as v+5+rel32: 0x004B63F3 + 0xFFFF93B8 = 0x004AF7B0 (Debug_Error); 0x004B640B + 0xFFFFF880 = 0x004B5C90 (Resource_FreeNode). The string at 0x00547150 is "Resource_Flush() : Couldn't locate resource at %p." byte-for-byte. Call census from the image's 0xE8 enumeration: TWENTY-SIX call sites image-wide (0x0041545D, 0x004165AF, 0x004165C6, 0x00417C82, 0x00417C9B, 0x00417CBE, 0x0046B4B2, 0x0046B4BB, 0x0046B4C3, 0x004868C6, 0x00486F1A, 0x0048A5B2, 0x004913BF, 0x004BBBAE, 0x004BD3B1, 0x004C5DE1, 0x004C5DF1, 0x004C5E06, 0x004C5E19, 0x004C5E2C, 0x004CFAFB, 0x004CFB0E, 0x004CFB88, 0x004CFBB9, 0x005238B6, 0x00523E0D), so this is a widely used flush and not a private helper of the QVM teardown.
// Behavior/CFG: one cdecl 32-bit argument (a resource identifier compared against a node field), no frame, no prologue beyond the register use. 0x004B63D0 reads the DWORD at the global 0x00943E38 and treats it as the first node, so that global holds the head pointer and the list links through +0x00; each visited node is compared by its dword at +0x34. TWO RETAIL QUIRKS ARE PRESERVED VERBATIM: (1) the null test at 0x004B63DC happens BEFORE the key comparison, so when the head has no successor the function reports not-found WITHOUT ever comparing the head's own key - a one-element list can never be found; (2) the walk advances through the node it just compared, so the last node of a longer list is compared before the null test ends the loop. On a match, the dword at +0x24 selects the outcome: 3 returns it, 1 returns it, and any other value releases the node through 0x004B5C90 Resource_FreeNode and returns that call's result. On no match it formats the 0x00547150 message with the argument through 0x004AF7B0 Debug_Error and then spins forever at 0x004B63FB - the fatal path never returns, so there is no error return value. Nine blocks, fifteen edges, all four branch targets inside the extent.
// Status/limits: TRANSCRIBED; NOT LINKABLE - undefined in tree, measured: the global 0x00943E38 (no owner anywhere under engine/, no manifest record; declared below as an evidenced NATIVE GAP) and nothing else - Debug_Error 0x004AF7B0 and Resource_FreeNode 0x004B5C90 are both tree natives, so this file closes the Resource_Flush half of the terrain closure and leaves the sentinel half open. RETURN TYPE: the bytes cannot distinguish `void` from `int` here - on the 1 and 3 paths EAX holds the compared field value and on the third path it holds Resource_FreeNode's result, which is exactly what a void function that ends with that call would also leave behind. This transcription declares std::int32_t and RETURNS those values, because that is what preserves the machine's EAX at the `ret`; the existing caller script/native_4bd380.cpp declares `void NATIVE_DECL Resource_Flush(void* data)` (a call-site inference, where EAX is discarded - retail overwrites it at 0x004BD3B6), so that one declaration should be updated to std::int32_t for interface agreement; C linkage means the mismatch cannot break the link. STRUCTURALLY_VERIFIED against the 25-instruction listing (9 blocks, 15 edges) and BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, rc=0, zero warning lines); runtime UNOBSERVED. Domain: resource/, because the nodes are resource records released through Resource_FreeNode.

#include "dev_log.hpp"
#include <cstdint>

#include "igi/retail/resource/sys_resource.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004AF7B0: retail's fatal error reporter, varargs (core/retail_error.cpp).
int NATIVE_DECL Debug_Error(char const* format, ...);

// NATIVE GAP: 0x00943E38 has no owner anywhere under engine/ (measured: no definition, no declaration, no manifest
// record). 0x004B63D0 reads the dword AT this address and dereferences it as the first node, so the global holds the
// list head pointer and the list links through +0x00. Declared here as a real symbol reference rather than a cast,
// and THIS FILE CANNOT LINK UNTIL WHOEVER OWNS THAT GLOBAL DEFINES IT. The alternative reading - a sentinel struct
// living at 0x00943E38 whose first field is read - is machine-identical, because both make the dword at that address
// the first node.
extern "C" void* g_943e38;

std::int32_t NATIVE_DECL Resource_Flush(void* resource)
{
    NATIVE_LOG_ENTER("Resource_Flush", "0x004B63D0");
    // 0x004B63D0/0x004B63D6: mov 0x00943E38,%ecx; mov 0x4(%esp),%edx - the head and the argument.
    std::uint8_t* node = static_cast<std::uint8_t*>(g_943e38);

    // 0x004B63DA/0x004B63DC/0x004B63DE: mov (%ecx),%eax; test %eax,%eax; je not-found. The null test precedes the
    // key comparison, so the head is only ever compared when it has a successor (see the provenance note).
    std::uint8_t* next = *reinterpret_cast<std::uint8_t**>(node);
    if (next != nullptr)
    {
        for (;;)
        {
            // 0x004B63E0: cmp %edx,0x34(%ecx) - the node's key at +0x34 against the argument.
            if (*reinterpret_cast<std::uint32_t*>(node + 0x34) ==
                static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(resource)))
            {
                // 0x004B63FD: mov 0x24(%ecx),%eax - the node's type at +0x24.
                std::int32_t const node_type = *reinterpret_cast<std::int32_t*>(node + 0x24);
                if (node_type == 3)                       // 0x004B6400/0x004B6403: cmp $0x3,%eax; je ret
                {
                    return node_type;
                }
                if (node_type == 1)                       // 0x004B6405/0x004B6408: cmp $0x1,%eax; je ret
                {
                    return node_type;
                }
                // 0x004B640A/0x004B640B/0x004B6410: push %ecx; call 0x004B5C90; add $0x4,%esp - any other type is
                // released and the release call's result is what the machine leaves in EAX at the ret.
                return igi::retail::resource::Resource_FreeNode(node);
            }
            node = next;                                          // 0x004B63E5: mov %eax,%ecx
            next = *reinterpret_cast<std::uint8_t**>(next);        // 0x004B63E7: mov (%eax),%eax
            if (next == nullptr)                                  // 0x004B63E9/0x004B63EB: test/jne loop
            {
                break;
            }
        }
    }

    // 0x004B63ED/0x004B63EE/0x004B63F3/0x004B63F8: push %edx; push $0x00547150; call 0x004AF7B0; add $0x8,%esp.
    Debug_Error("Resource_Flush() : Couldn't locate resource at %p.", resource);
    for (;;)                                                      // 0x004B63FB: jmp 0x004B63FB (retail spins here)
    {
    }
}

} // extern "C"

#undef NATIVE_DECL
