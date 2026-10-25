// Native: QTask_DestroyTask_4015F0; retail VA 0x004015F0 (placeholder name until VERIFIED).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004015F0-0x00401776 (391 bytes, 25 blocks, 134 instructions).
// Tools/evidence: radare2 6.2.0 pdj/afij/afbj and NASM ndisasm cross-decode (identical 134-instruction tiling); docs/fidelity/evidence/0x4EE970-data-extents.md section 18.
// Behavior/CFG: decrements 0x00567C58; fixes the task's slot on the 0x00AFA6E0 dispatch stack; calls the phase-3 handler (0x00A97CE0) and the phase-2 handler (0x00A976E0), one argument each; destroys the child list from its tail with recursive self-calls; clears 0x00567C38 slot-table entries; unlinks through 0x004015D0; then either swap-removes a pooled object from its 0x00AF5EE0 class pool or frees through 0x004B0D10.
// Status/limits: BUILD_VERIFIED (CFG 25 blocks/37 edges, score 1.000); runtime UNOBSERVED. Gaps in evidence section 18.8. Returns void because retail EAX differs per path and no caller reads it.
#include "igi/retail/core/qhash_slot_table.hpp"
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

// 0x00AFA6E0: retail task-dispatch stack, 0x40 dwords ending at the depth word; 0x004010D0 clears it with
// rep stosd 0x40 at 0x004010DD. 0x00AFA7E0: its depth, branched on signed (0x004015FF/0x0040160F).
std::uint32_t g_afa6e0[0x40];
std::int32_t g_afa7e0;

// 0x00567C58: the live-task count this native decrements (0x004015F7); defined by its retail writer in
// engine/src/retail/input/input_port_process.cpp. The current symbol name is the section 17.6 misnomer.
extern std::uint32_t QTask_InputPortCounter;

// 0x004B0D10 heap release and 0x004015D0 unlink; neither has a header yet, same local declaration form as
// engine/src/retail/script/native_4f0c90.cpp.
void Mem_DeAllocFn(void* pointer);
void QTask_UnlinkTask_4015D0(void* task);

void NATIVE_DECL QTask_DestroyTask_4015F0(void* task)
{
    std::uint8_t* const p = static_cast<std::uint8_t*>(task);

    // 0x004015F0-0x0040160F: store the decremented live count before any call (section 18.6 row 1).
    --::QTask_InputPortCounter;                                             // 0x004015F0/0x004015F7/0x004015F9
    const std::int32_t depth = ::g_afa7e0;                                  // 0x004015FF
    std::int32_t index = 0;                                                 // 0x00401605

    // 0x00401611-0x0040163D: replace this task's dispatch-stack slot with its next node, or 0 when the
    // link is absent or its target word is null; stored before the phase-3 handler runs.
    if (depth > 0)                                                          // 0x0040160D/0x0040160F
    {
    scan:
        if (::g_afa6e0[index] == static_cast<std::uint32_t>(
                reinterpret_cast<std::uintptr_t>(task)))                    // 0x00401611/0x00401616
        {
            const std::uint32_t next = *reinterpret_cast<std::uint32_t*>(p); // 0x00401624: task +0x00
            std::uint32_t replacement;
            if (next == 0U)                                                 // 0x00401626/0x00401628
            {
                replacement = 0U;                                           // 0x00401634
            }
            else
            {
                replacement = (*reinterpret_cast<std::uint32_t*>(
                    static_cast<std::uintptr_t>(next)) != 0U) ? next : 0U;  // 0x0040162A-0x00401632
            }
            ::g_afa6e0[index] = replacement;                                // 0x00401636
        }
        else
        {
            ++index;                                                        // 0x0040161A
            if (index < depth) goto scan;                                   // 0x0040161E/0x00401620
            goto dispatch;                                                  // 0x00401622 jmp 0x0040163D
        }
    }
dispatch:
    // 0x0040163D-0x00401651: phase-3 handler from 0x00A97CE0, one argument.
    {
        const std::uint32_t phase3Type = *reinterpret_cast<volatile std::uint16_t*>(p + 0x1C); // 0x0040163F
        igi::retail::core::FrameHandler const handler =                     // 0x00401643
            ::QTask_HandlerTable[3U * igi::retail::core::task_phase_stride + phase3Type];
        if (handler != nullptr)                                             // 0x0040164A
        {
            handler(task);                                                  // 0x0040164E/0x0040164F
        }
    }

    // 0x00401654-0x0040166E: destroy children from the tail. The tail read must stay after the phase-3
    // handler because a handler may change the child list (section 18.6 row 10).
    {
        std::uint8_t* node = *reinterpret_cast<std::uint8_t**>(p + 0x10);   // 0x00401654
        std::uint8_t* previous = *reinterpret_cast<std::uint8_t**>(node + 0x04); // 0x00401657
        if (previous != nullptr)                                            // 0x0040165A/0x0040165C
        {
            do
            {
                QTask_DestroyTask_4015F0(node);                             // 0x0040165E/0x0040165F
                node = previous;                                            // 0x00401664
                previous = *reinterpret_cast<std::uint8_t**>(previous + 0x04); // 0x00401666
            } while (previous != nullptr);                                  // 0x0040166C/0x0040166E
        }
    }

    // 0x00401670-0x00401684: phase-2 handler from 0x00A976E0. The type word is re-read: the phase-3
    // handler ran in between, so a cached type selects the wrong handler (section 18.6 row 4).
    {
        const std::uint32_t phase2Type = *reinterpret_cast<volatile std::uint16_t*>(p + 0x1C); // 0x00401672
        igi::retail::core::FrameHandler const handler =                     // 0x00401676
            ::QTask_HandlerTable[2U * igi::retail::core::task_phase_stride + phase2Type];
        if (handler != nullptr)                                             // 0x0040167D
        {
            handler(task);                                                  // 0x00401681/0x00401682
        }
    }

    // 0x00401687-0x004016C2: clear the task from the 0x00567C38 slot table; the gate is the dword at
    // task+0x18. Count loaded once, base/index/stride re-read per iteration; every match is cleared.
    if (*reinterpret_cast<std::uint32_t*>(p + 0x18) != 0U)                  // 0x00401687/0x0040168A/0x0040168C
    {
        const std::int32_t count = static_cast<std::int32_t>(igi::retail::core::g_567c40); // 0x0040168E
        if (count > 0)                                                      // 0x00401695/0x00401699
        {
            for (std::int32_t slot = 0; slot < count; ++slot)               // 0x00401693/0x004016BF/0x004016C2
            {
                const std::uint32_t entryIndex = *reinterpret_cast<std::uint32_t*>(
                    static_cast<std::uintptr_t>(igi::retail::core::g_567c3c) +
                    static_cast<std::uintptr_t>(slot) * 4U);                // 0x0040169B/0x004016A6
                std::uint8_t* const entry = reinterpret_cast<std::uint8_t*>(
                    static_cast<std::uintptr_t>(igi::retail::core::g_567c38) +
                    static_cast<std::uintptr_t>(entryIndex) *
                        static_cast<std::uintptr_t>(igi::retail::core::g_567c48)); // 0x004016A0/0x004016A9/0x004016B0/0x004016B3
                if (*reinterpret_cast<std::uint32_t*>(entry) ==
                    static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(task))) // 0x004016B5/0x004016B7
                {
                    *reinterpret_cast<std::uint32_t*>(entry) = 0U;          // 0x004016B9
                }
            }
        }
    }

    // 0x004016C4-0x004016D2: unlink first, then read the per-instance pooled byte at task+0x1E; zero
    // selects the heap release (section 18.5). The read stays after the call as retail has it.
    QTask_UnlinkTask_4015D0(task);                                          // 0x004016C4/0x004016C5
    if (*reinterpret_cast<std::uint8_t*>(p + 0x1E) == 0U)                   // 0x004016CA/0x004016D0/0x004016D2
    {
        Mem_DeAllocFn(task);                                                // 0x00401769/0x0040176A
        return;                                                             // 0x00401772-0x00401776
    }

    // 0x004016D8-0x00401761: pooled release, the inverse of the allocator's claim at 0x0040132E-0x00401358.
    // The class record is re-read from the type word three times and its fields between the three stores,
    // so every class access below is volatile (section 18.6 rows 3, 5-8).
    {
        const std::uint32_t typeUsed = *reinterpret_cast<volatile std::uint16_t*>(p + 0x1C); // 0x004016D8/0x004016DA
        std::uint8_t* const record =
            reinterpret_cast<std::uint8_t*>(&::QTask_TypeTable[static_cast<std::size_t>(typeUsed)]); // 0x004016DE/0x004016E1
        const std::uint32_t tailOffset =
            *reinterpret_cast<volatile std::uint32_t*>(record + 0x20);      // 0x004016E4: class +0x20
        std::uint32_t used =
            *reinterpret_cast<volatile std::uint32_t*>(record + 0x14);      // 0x004016EA: class +0x14
        --used;                                                             // 0x004016F0
        const std::uint32_t position = *reinterpret_cast<std::uint32_t*>(
            static_cast<std::uintptr_t>(tailOffset) +
            reinterpret_cast<std::uintptr_t>(task));                        // 0x004016F1
        *reinterpret_cast<volatile std::uint32_t*>(record + 0x14) = used;   // 0x004016F4: store poolUsed

        const std::uint32_t typeCompare = *reinterpret_cast<volatile std::uint16_t*>(p + 0x1C); // 0x004016FA/0x004016FC
        std::uint8_t* const recordCompare =
            reinterpret_cast<std::uint8_t*>(&::QTask_TypeTable[static_cast<std::size_t>(typeCompare)]); // 0x00401700/0x00401703
        const std::uint32_t poolUsedNow =
            *reinterpret_cast<volatile std::uint32_t*>(recordCompare + 0x14); // 0x00401706
        if (position == poolUsedNow)                                        // 0x0040170C/0x0040170E
        {
            return;                                                         // 0x00401772-0x00401776: no swap needed
        }

        std::uint32_t* const indices = reinterpret_cast<std::uint32_t*>(
            static_cast<std::uintptr_t>(
                *reinterpret_cast<volatile std::uint32_t*>(recordCompare + 0x10))); // 0x00401710: class +0x10
        const std::uint32_t poolBase =
            *reinterpret_cast<volatile std::uint32_t*>(recordCompare + 0x0C);       // 0x00401716: class +0x0C
        const std::uint32_t lastSlot = indices[poolUsedNow];                // 0x0040171C
        const std::uint32_t mySlot = indices[position];                     // 0x0040171F
        std::uintptr_t const lastElement =
            static_cast<std::uintptr_t>(poolBase) +
            static_cast<std::uintptr_t>(lastSlot) *
                static_cast<std::uintptr_t>(
                    *reinterpret_cast<volatile std::uint32_t*>(recordCompare + 0x1C)); // 0x00401722/0x0040172F
        const std::uint32_t lastElementTail =
            *reinterpret_cast<volatile std::uint32_t*>(recordCompare + 0x20);       // 0x00401729
        const std::uint32_t lastPosition = *reinterpret_cast<std::uint32_t*>(
            lastElement + static_cast<std::uintptr_t>(lastElementTail));            // 0x00401731
        const std::uint32_t moved = indices[lastPosition];                  // 0x00401734
        indices[position] = moved;                                          // 0x00401737: store A

        const std::uint32_t typeIndices = *reinterpret_cast<volatile std::uint16_t*>(p + 0x1C); // 0x0040173A/0x0040173C
        std::uint8_t* const recordIndices =
            reinterpret_cast<std::uint8_t*>(&::QTask_TypeTable[static_cast<std::size_t>(typeIndices)]); // 0x00401740/0x00401743
        std::uint32_t* const indicesAgain = reinterpret_cast<std::uint32_t*>(
            static_cast<std::uintptr_t>(
                *reinterpret_cast<volatile std::uint32_t*>(recordIndices + 0x10))); // 0x00401746: class +0x10 re-read
        indicesAgain[lastPosition] = mySlot;                                // 0x0040174C: store B

        const std::uint32_t typeTail = *reinterpret_cast<volatile std::uint16_t*>(p + 0x1C); // 0x0040174F/0x00401751
        std::uint8_t* const recordTail =
            reinterpret_cast<std::uint8_t*>(&::QTask_TypeTable[static_cast<std::size_t>(typeTail)]); // 0x00401755/0x00401758
        const std::uint32_t tailOffsetAgain =
            *reinterpret_cast<volatile std::uint32_t*>(recordTail + 0x20);  // 0x0040175B: class +0x20 re-read
        *reinterpret_cast<std::uint32_t*>(
            lastElement + static_cast<std::uintptr_t>(tailOffsetAgain)) = position; // 0x00401761: store C
    }
}

} // extern "C"

#undef NATIVE_DECL
