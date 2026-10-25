// Native: Task_WalkAndDispatchEvent_424280; retail VA 0x00424280 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00424280-0x004242F4 (117 bytes, 10 blocks, 14 edges, 40 instructions).
// Tools/evidence: radare2 6.2.0 af/pd over the exact bytes; pushed as the current-event-word handler at 0x004240CD, 0x00469706, 0x00485203 (GameScreen registrar 0x004851B0), 0x004E4E5A and 0x004F5683, and dispatched with one word at 0x00402481 (single-argument site).
// Behavior/CFG: takes the node linked at arg1+0x08 and walks it: while the node is non-null it pushes the node's child (0, or the child when the child's first word is set) onto the 0x00AFA6E0 dispatch stack at depth 0x00AFA7E0, increments the depth, reads the event word through 0x004E7530, dispatches QTask_HandlerTable[event*0x180 + word[node+0x1C]] with the node when that entry is set, then pops one depth level and continues while the popped node is non-null. 10 blocks / 14 edges with the back edge 0x004242F1 -> 0x00424297.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. Same walk as the transcribed phase-1 dispatcher 0x00488700, except that the phase comes from the current event word (0x004E7530) instead of the constant 1 and the handler is tested against zero only; both shapes follow the retail bytes. The 0x00AFA6E0 stack and 0x00AFA7E0 depth are the storage owned by native_4015f0.cpp and declared extern here. The routine is not an r2 function start (it is only referenced as an immediate), so the extent above is the recovered body through its ret and nop pad.
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

// 0x004E7530, transcribed in engine/src/retail/core/native_4e7530.cpp: returns the word at 0x005488C8.
std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();

void NATIVE_DECL Task_WalkAndDispatchEvent_424280(void* task)
{
    std::uint8_t* const self = static_cast<std::uint8_t*>(task);
    std::uint8_t* node = *reinterpret_cast<std::uint8_t**>(self + 0x08);   // 0x00424285: mov 0x8(%eax),%esi
    if (*reinterpret_cast<std::uint32_t*>(node) == 0U)                    // 0x00424288/0x0042428B: cmpl $0,(%esi); je
    {
        return;                                                           // 0x004242F4: ret
    }
    if (node == nullptr)                                                  // 0x0042428D/0x0042428F: test %esi,%esi; je
    {
        return;                                                           // 0x004242F4: ret
    }

    std::int32_t depth = ::g_afa7e0;                                      // 0x00424291: mov 0xafa7e0,%ecx
    do
    {
        std::uint8_t* const child = *reinterpret_cast<std::uint8_t**>(node);  // 0x00424297: mov (%esi),%edx
        std::uint32_t entry;
        if (child == nullptr)                                             // 0x00424299/0x0042429B: test %edx,%edx; je
        {
            entry = 0U;                                                   // 0x004242A7: xor %eax,%eax
        }
        else
        {
            // 0x0042429D-0x004242A3: mov (%edx),%eax; neg %eax; sbb %eax,%eax; and %edx,%eax
            // -> the child pointer when the child's first word is set, otherwise 0.
            entry = (*reinterpret_cast<std::uint32_t*>(child) != 0U)
                        ? static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(child))
                        : 0U;
        }
        ::g_afa6e0[depth] = entry;                                        // 0x004242A9: mov %eax,0xafa6e0(,%ecx,4)
        ++depth;                                                          // 0x004242B0: inc %ecx
        ::g_afa7e0 = depth;                                               // 0x004242B1: mov %ecx,0xafa7e0

        std::uint32_t index = Core_GetEventWord_4E7530() * 3U;            // 0x004242B7 call 0x004E7530; 0x004242BC lea (%eax,%eax,2),%ecx
        const std::uint32_t type =
            *reinterpret_cast<std::uint16_t*>(node + 0x1C);               // 0x004242BF/0x004242C1: xor %edx,%edx; mov 0x1c(%esi),%dx
        index <<= 7;                                                      // 0x004242C5: shl $0x7,%ecx
        index += type;                                                    // 0x004242C8: add %edx,%ecx
        igi::retail::core::FrameHandler const handler =
            ::QTask_HandlerTable[index];                                  // 0x004242CA: mov 0xa96ae0(,%ecx,4),%eax
        if (handler != nullptr)                                           // 0x004242D1/0x004242D3: test %eax,%eax; je
        {
            handler(node);                                                // 0x004242D5/0x004242D6: push %esi; call *%eax
        }

        depth = ::g_afa7e0;                                               // 0x004242DB: reload after the call
        --depth;                                                          // 0x004242E1: dec %ecx
        ::g_afa7e0 = depth;                                               // 0x004242E2: mov %ecx,0xafa7e0
        node = reinterpret_cast<std::uint8_t*>(
            static_cast<std::uintptr_t>(::g_afa6e0[depth]));              // 0x004242E8: mov 0xafa6e0(,%ecx,4),%esi
    } while (node != nullptr);                                            // 0x004242EF/0x004242F1: test %esi,%esi; jne
}                                                                         // 0x004242F4: ret

} // extern "C"

#undef NATIVE_DECL
