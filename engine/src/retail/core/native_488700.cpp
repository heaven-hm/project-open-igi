// Native: Task_WalkAndDispatch_488700; retail VA 0x00488700 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00488700-0x00488767 (104 bytes, 11 blocks, 15 edges).
// Tools/evidence: radare2 6.2.0 pd; section 21 of the evidence sheet (installed as a phase-1 handler by 0x004688B0; two-argument phase-0 dispatch is the other shape, this one takes one).
// Behavior/CFG: from arg1+0x08 takes a node, walks it while non-null: pushes the node's own child (0 or [node] when the child's first word is set) onto the 0x00AFA6E0 dispatch stack at the 0x00AFA7E0 depth and increments the depth, reads the node's type word at +0x1C, dispatches the phase-1 handler table entry 0x00A970E0 + type*4 with the node when it is set, then pops one level (0x00AFA6E0[depth-1]), decrements the depth and continues while the popped node is non-null. Returns when the first word of arg1+0x08 is zero.
// Status/limits: BUILD_VERIFIED (11 blocks / 15 edges from the generated artifacts vs the 11/15 section 21 reports); runtime UNOBSERVED. The 0x00AFA6E0 stack and its 0x00AFA7E0 depth are the storage defined by native_4015f0.cpp.
#include "dev_log.hpp"
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00AFA6E0 and 0x00AFA7E0: the retail dispatch stack and its depth, defined by the task destructor's translation unit.
extern std::uint32_t g_afa6e0[0x40];
extern std::int32_t g_afa7e0;

void NATIVE_DECL Task_WalkAndDispatch_488700(void* task)
{
    NATIVE_LOG_ENTER("Task_WalkAndDispatch_488700", "0x00488700");
    std::uint8_t* const self = static_cast<std::uint8_t*>(task);
    std::uint8_t* node = *reinterpret_cast<std::uint8_t**>(self + 0x08);   // 0x00488700/0x00488704
    if (*reinterpret_cast<std::uint32_t*>(node) == 0U)                    // 0x00488707/0x0048870A
    {
        return;                                                           // 0x00488767: ret
    }
    if (node == nullptr)                                                  // 0x0048870C/0x0048870E
    {
        return;                                                           // 0x00488767: ret
    }

    std::int32_t depth = ::g_afa7e0;                                      // 0x00488710
    do
    {
        std::uint32_t entry;                                              // 0x00488717: esi = [ecx]
        std::uint8_t* const child = *reinterpret_cast<std::uint8_t**>(node);
        if (child == nullptr)                                             // 0x00488719/0x0048871B
        {
            entry = 0U;                                                   // 0x00488727: xor eax, eax
        }
        else
        {
            // 0x0048871D-0x00488723: mov eax,[esi]; neg eax; sbb eax,eax; and eax,esi -> esi when [esi] != 0, else 0.
            entry = (*reinterpret_cast<std::uint32_t*>(child) != 0U)
                        ? static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(child))
                        : 0U;
        }
        ::g_afa6e0[depth] = entry;                                        // 0x00488729
        ++depth;                                                          // 0x00488730
        ::g_afa7e0 = depth;                                               // 0x00488733

        const std::uint32_t type = *reinterpret_cast<std::uint16_t*>(node + 0x1C); // 0x00488739
        igi::retail::core::FrameHandler const handler =
            ::QTask_HandlerTable[1U * igi::retail::core::task_phase_stride + type]; // 0x0048873D: base 0x00A970E0
        if (handler != nullptr)                                           // 0x00488744/0x00488746
        {
            handler(node);                                                // 0x00488748/0x00488749
            depth = ::g_afa7e0;                                           // 0x0048874B: reload after the call
        }

        node = reinterpret_cast<std::uint8_t*>(
            static_cast<std::uintptr_t>(::g_afa6e0[depth - 1]));          // 0x00488754: mov ecx,[edx*4+0x00AFA6DC]
        --depth;                                                          // 0x0048875B
        ::g_afa7e0 = depth;                                               // 0x0048875E
    } while (node != nullptr);                                            // 0x00488764: jne 0x00488717
}

} // extern "C"

#undef NATIVE_DECL
