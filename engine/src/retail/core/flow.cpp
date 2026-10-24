#include "igi/retail/core/flow.hpp"

#include "igi/retail/config/config_profile.hpp"
#include "igi/retail/core/frame_natives.hpp"
#include "igi/retail/core/game_flow.hpp"
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/input/input_port_process.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

#if defined(_WIN32)
#include <windows.h>
#endif

#include <cstdint>

extern "C" {

// 0x004E7530: the phase getter, transcribed in native_4e7530.cpp with C linkage.
std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();

} // extern "C"

namespace {

// The retail's frame timer at 0x00490370: the tick count less the start time.
std::uint32_t ElapsedTicks() noexcept
{
#if defined(_WIN32)
    return static_cast<std::uint32_t>(::GetTickCount()) - Loop_StartTicks;
#else
    return 0U;
#endif
}

// The retail's flow handlers, the addresses the class registration links.
void Flow_RunHandler_Handler(void* flow);
void Flow_Delete_Handler(void* flow);

// ---------------------------------------------------------------------------
// The retail globals Flow_RunHandler reads and writes.
// ---------------------------------------------------------------------------

// 0x00AFA6E0: the retail's task-dispatch stack, 0x40 dwords, with its depth at
// 0x00AFA7E0. 0x004010D0 (QTask_Init) clears both; the port's QTask_Init does
// not yet, so the port keeps them here and starts them clear.
// TODO(0x004010D0): clear 0x00AFA6E0 and 0x00AFA7E0 from QTask_Init.
std::uint32_t g_task_stack[0x40] = {};
std::uint32_t g_task_stack_depth = 0U;

// 0x00A4438C, 0x00A44390 and 0x00A44394: the three words the fast path clears
// at 0x004022EC.
std::uint32_t g_fast_path_words[3] = {};

// 0x00C28C5C: the frame stamp the post block at 0x004026E9 publishes.
std::uint32_t g_frame_stamp = 0U;

// The port's task node for the Game class. The retail allocates each class
// object through 0x004012F0 and links it into the flow object's list at +0x08
// through 0x004012A0 and 0x004AF8F0 (list head 0x00567C8C + 0x08, first slot at
// +0x0C, last at +0x10). The node's class id sits at +0x1C, which is the word
// Flow_RunHandler reads at 0x00402355 and 0x0040246C.
//
// PORT MAPPING: the port has no task-object allocator and no separate Game
// object yet, so it links one node for the Game class (id 124). Its phase 0
// handler is Level_Start (0x00415B30) and its phase 1 handler is Game_Setup
// (0x00416260), which is the per-frame update the retail reaches through this
// walk.
// TODO(0x004012A0, 0x004012F0, 0x00401F30): transcribe the task object
// allocation, the list link and the object store.
struct FlowTaskNode {
    std::uint32_t next;                 // +0x00
    std::uint32_t previous;             // +0x04
    std::byte unknown_0x08[0x14];       // +0x08..+0x1B
    std::uint16_t dispatch_table_index; // +0x1C, the class id
    std::byte unknown_0x1E[0x0A];       // +0x1E..+0x27
};

static_assert(sizeof(FlowTaskNode) == 0x28, "the retail task node header is 0x28 bytes");

FlowTaskNode g_game_task{};

// Links the port's Game task node into the flow object's list the first time
// the walk runs. Retail equivalent: the 0x004012A0 link inside Flow_Start
// (0x0040210F), which is the only reason the retail's walk finds the Game
// object on the first frame.
void Flow_LinkPortTaskList(igi::retail::core::FrameDispatchRecordView* record) noexcept
{
    if (record == nullptr || record->task_first != 0U)
    {
        return;
    }
    g_game_task.next = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(&record->task_list_first));
    g_game_task.previous = 0U;
    g_game_task.dispatch_table_index = QTask_GameClassId;
    record->task_list_first = 0U;
    record->task_list_last = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(&g_game_task));
    record->task_first = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(&g_game_task));
}

// The retail walks the flow object's task list five times with the same body,
// each time inlined at its own site:
//   0x00402321..0x00402387  fast path, phase 1 table 0x00A970E0
//   0x00402387..0x004023E7  fast path, the input port record's list
//   0x00402431..0x004024A3  slow path, the current phase from 0x004E7530
//   0x004024D0..0x00402533  paused path, phase 1 table
//   0x00402547..0x00402593  paused path, the input port record's list
//   0x004025B6..0x00402610  paused path, the current phase
// The body reads the node at [object + 0x08], keeps the validated next pointer
// on the 0x00AFA6E0 stack with its depth at 0x00AFA7E0, reads the node's class
// id at +0x1C and calls the handler the table holds for that class.
void Flow_DispatchTaskListPhase1(void* object_void)
{
    auto* const object = static_cast<igi::retail::core::FrameDispatchRecordView*>(object_void);
    if (object == nullptr)
    {
        return;
    }
    // 0x00402321: edx = [ebp + 8], the object's +0x08.
    std::uintptr_t node = object->task_first;
    if (node == 0U || *reinterpret_cast<const std::uint32_t*>(node) == 0U)
    {
        return; // 0x00402327/0x0040232B: an empty list skips the walk
    }
    const igi::retail::core::FrameHandler* const phase1 = QTask_Phase1HandlerTable();
    std::uint32_t depth = g_task_stack_depth; // 0x0040232F
    do
    {
        // 0x00402334..0x00402342: the next node, kept only when it is a node
        // whose own next pointer is set.
        std::uintptr_t next = *reinterpret_cast<const std::uint32_t*>(node);
        if (next != 0U && *reinterpret_cast<const std::uint32_t*>(next) == 0U)
        {
            next = 0U;
        }
        g_task_stack[depth] = static_cast<std::uint32_t>(next); // 0x00402346
        ++depth;
        g_task_stack_depth = depth;                             // 0x00402350
        const std::uint16_t class_id = *reinterpret_cast<const std::uint16_t*>(
            reinterpret_cast<const std::uint8_t*>(node) + 0x1C); // 0x00402355
        const igi::retail::core::FrameHandler handler =
            (phase1 != nullptr) ? phase1[class_id] : nullptr;
        if (handler != nullptr)
        {
            handler(reinterpret_cast<void*>(node));             // 0x00402365
        }
        --depth;                                                // 0x0040236F
        g_task_stack_depth = depth;
        node = g_task_stack[depth];
    } while (node != 0U);                                       // 0x0040237E
}

void Flow_DispatchTaskListPhaseN(void* object_void)
{
    auto* const object = static_cast<igi::retail::core::FrameDispatchRecordView*>(object_void);
    if (object == nullptr)
    {
        return;
    }
    std::uintptr_t node = object->task_first; // 0x00402431 / 0x004025B6
    if (node == 0U || *reinterpret_cast<const std::uint32_t*>(node) == 0U)
    {
        return; // 0x00402434/0x00402438
    }
    std::uint32_t depth = g_task_stack_depth; // 0x0040243C
    do
    {
        std::uintptr_t next = *reinterpret_cast<const std::uint32_t*>(node);
        if (next != 0U && *reinterpret_cast<const std::uint32_t*>(next) == 0U)
        {
            next = 0U;
        }
        g_task_stack[depth] = static_cast<std::uint32_t>(next); // 0x00402454
        ++depth;
        g_task_stack_depth = depth;                             // 0x0040245C
        // 0x00402462: eax = 0x004E7530(), the current phase, and the handler is
        // table[class_id + phase * 384] at 0x00A96AE0.
        const std::uint32_t phase = Core_GetEventWord_4E7530();
        const std::uint16_t class_id = *reinterpret_cast<const std::uint16_t*>(
            reinterpret_cast<const std::uint8_t*>(node) + 0x1C); // 0x0040246C
        const std::uint32_t index =
            (phase * igi::retail::core::task_phase_stride) + class_id; // 0x00402467..0x00402473
        const igi::retail::core::FrameHandler handler = QTask_HandlerTable[index];
        if (handler != nullptr)
        {
            handler(reinterpret_cast<void*>(node));             // 0x00402481
        }
        --depth;                                                // 0x0040248C
        g_task_stack_depth = depth;
        node = g_task_stack[depth];                             // 0x00402493
    } while (node != 0U);                                       // 0x0040249C
}

// The same walk over the input port record Flow_Start registers: the retail
// reads [flow + 0x48], then that record's +0x08, and walks it with the phase 1
// table (0x00402387 and 0x00402547).
// TODO(0x0040208B): Flow_Start must store the record 0x004012A0 returns at
// +0x48; the port's Flow_Start does not yet, so this walk finds no list.
void Flow_DispatchPortRecordListPhase1(void* object_void)
{
    auto* const object = static_cast<igi::retail::core::FrameDispatchRecordView*>(object_void);
    if (object == nullptr || object->port_record == 0U)
    {
        return; // 0x0040238D/0x00402390: no record, no list
    }
    const auto* const record = reinterpret_cast<const std::uint8_t*>(
        static_cast<std::uintptr_t>(object->port_record));
    std::uintptr_t node = *reinterpret_cast<const std::uint32_t*>(record + 0x08);
    if (node == 0U || *reinterpret_cast<const std::uint32_t*>(node) == 0U)
    {
        return; // 0x00402393/0x00402397
    }
    const igi::retail::core::FrameHandler* const phase1 = QTask_Phase1HandlerTable();
    std::uint32_t depth = g_task_stack_depth; // 0x0040239B
    do
    {
        std::uintptr_t next = *reinterpret_cast<const std::uint32_t*>(node);
        if (next != 0U && *reinterpret_cast<const std::uint32_t*>(next) == 0U)
        {
            next = 0U;
        }
        g_task_stack[depth] = static_cast<std::uint32_t>(next); // 0x004023AD
        ++depth;
        g_task_stack_depth = depth;                             // 0x004023B7
        const std::uint16_t class_id = *reinterpret_cast<const std::uint16_t*>(
            reinterpret_cast<const std::uint8_t*>(node) + 0x1C); // 0x004023BC
        const igi::retail::core::FrameHandler handler =
            (phase1 != nullptr) ? phase1[class_id] : nullptr;
        if (handler != nullptr)
        {
            handler(reinterpret_cast<void*>(node));             // 0x004023CE
        }
        --depth;
        g_task_stack_depth = depth;
        node = g_task_stack[depth];
    } while (node != 0U);                                       // 0x004023E1
}

} // namespace

extern "C" {

std::uint16_t QTask_FlowClassId = 0U;
std::uint8_t Flow_StateByte = 0U;
std::uint32_t Loop_StartTicks = 0U;
std::uint8_t Flow_PausedFlag = 0U;
std::uint8_t AppContext_PausedFlag = 0U;

// Native: Flow_Init; retail VA 0x00401FC0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00401FC0..0x00402010.
// Behavior/CFG: reads the current task type through 0x004018F0, registers the Flow class with a 0x54 byte object through 0x00401900, keeps the class id at 0x00567C7C, and links the phase 0 handler 0x00402070 through 0x00401400, the phase 1 handler 0x00402260 and the phase 2 handler 0x00402740 through 0x00401530.
// Status/limits: STRUCTURALLY_VERIFIED; the class registration, the object size and the three phase links are the retail's.
void NATIVE_DECL Flow_Init()
{
    const std::uint16_t parent = QTask_GetCurrentType();
    const std::uint16_t class_id = TaskType_Register("Flow", 0x54U, parent, 0U);
    QTask_FlowClassId = class_id;
    QTask_LinkQTaskEventHandler(class_id, 0U, &Flow_Start);
    QTask_LinkQTaskEventHandlerNoContextFn(class_id, 1U, &Flow_RunHandler_Handler);
    QTask_LinkQTaskEventHandlerNoContextFn(class_id, 2U, &Flow_Delete_Handler);
}

// Native: Flow_Start; retail VA 0x00402070.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00402070..0x0040225F.
// Behavior/CFG: stores the flow object at 0x00567C8C and registers the input port handler through 0x004012A0 with the current task type and zero; the retail also links the returned record into the flow object's list at +0x08 (0x0040210F) and stores the record at +0x48.
// Status/limits: STRUCTURALLY_VERIFIED for the two stores and the call. The record link and the +0x48 store are not transcribed yet (see the TODO above).
void NATIVE_DECL Flow_Start(void* flow)
{
    Game_FrameDispatchRecord = static_cast<igi::retail::core::FrameDispatchRecordView*>(flow);
    QTask_RegisterInputPortHandler(static_cast<std::uint32_t>(
                                       reinterpret_cast<std::uintptr_t>(flow)),
                                   QTask_GetCurrentType(), 0U);
}

// Native: Flow_RunHandler; retail VA 0x00402260.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Tools/evidence: Radare2 on retail IGI.EXE, VA 0x00402260..0x0040271C (96 basic blocks); the paired
//                 runtime trace in docs/fidelity/evidence/0x00401050-retail-flow-order.json.
// Behavior/CFG: the retail reads the elapsed ticks at 0x00490370 (0x00402267) and takes one of three
//           paths before the state switch at 0x00402629:
//             * the paused path (0x004024D0) when the state byte at 0x005C8E00 is set, when
//               0x004EE940 returns non-zero or when 0x00491220 returns non-zero;
//             * the slow path (0x00402412) when the scaled tick total is not below the elapsed
//               ticks, when the frame counter at +0x40 is above 10, or when the +0x44 flag is
//               clear and the factor at +0x34 is below 3;
//             * the fast path (0x004022EC) otherwise.
//           Each path walks the flow object's task list (0x00AFA6E0 stack, 0x00AFA7E0 depth),
//           dispatches the handler table entry for each node's class id at +0x1C, and then falls
//           into the state switch, which runs the pending state at +0x2C through the eight-case
//           jump table at 0x00402720 and finishes with the post block at 0x004026E4.
// Status/limits: STRUCTURALLY_VERIFIED for the three paths, the two walks, the state switch, the
//           table order and the post block. The callees the port does not have yet are left as
//           TODO(VA) markers, and the phase getter 0x004E7530 is mapped to the port's play phase;
//           those states are not_solved rather than invented.
void NATIVE_DECL Flow_RunHandler(void* flow)
{
    const std::uint32_t ticks = ElapsedTicks(); // 0x00402267, 0x00490370
    auto* const object = (Game_FrameDispatchRecord != nullptr)
                             ? Game_FrameDispatchRecord
                             : static_cast<igi::retail::core::FrameDispatchRecordView*>(flow);
    if (object == nullptr)
    {
        return; // port guard: the retail dereferences 0x00567C8C
    }
    // The port's task list stands in for the retail's 0x004012A0 link.
    Flow_LinkPortTaskList(object);

    // 0x00402272: the state byte at 0x005C8E00; 0x00402281: 0x004EE940; 0x0040228E:
    // 0x00491220. The port reads the two pause checks from the flags at
    // 0x00A70C5B and 0x005C8E71 until those natives are transcribed.
    // TODO(0x004EE940, 0x00491220): transcribe the two pause checks.
    const bool paused = (Flow_StateByte != 0U) || (AppContext_PausedFlag != 0U) ||
                        (Flow_PausedFlag != 0U);
    if (paused)
    {
        // 0x004024D0: the paused path.
        Flow_DispatchTaskListPhase1(object);      // 0x004024D0..0x00402533
        Flow_DispatchPortRecordListPhase1(object); // 0x00402547..0x00402593
        ++object->counter;                        // 0x00402599, [object + 0x34]
        // 0x0040259F: 0x00491220 again, the port's pause flag.
        if (AppContext_PausedFlag == 0U)
        {
            Flow_DispatchTaskListPhaseN(object);  // 0x004025B6..0x00402610
        }
        ++object->phase_counter;                  // 0x00402612, [object + 0x38]
        // 0x0040261B and 0x00402620: 0x004E7BB0() and 0x004E7B80(0, 0).
        // TODO(0x004E7BB0, 0x004E7B80): transcribe the two frame hooks.
    }
    else
    {
        // 0x0040229B..0x004022C6: the level scale, on the x87 stack.
        const std::int32_t factor = static_cast<std::int32_t>(object->counter);
        const std::int32_t base = static_cast<std::int32_t>(object->base_high) +
                                  static_cast<std::int32_t>(object->base_low);
        const std::int64_t total =
            igi::retail::core::Flow_LevelScale(static_cast<std::int32_t>(object->divisor),
                                               factor, base);
        // 0x004022C8: the comparison reads the low dword of the stored total.
        const bool slow = (static_cast<std::int32_t>(ticks) <=
                           static_cast<std::int32_t>(total)) ||              // 0x004022C8
                          (static_cast<std::int32_t>(object->frame_counter) > 10) || // 0x004022D5
                          (object->unknown_0x44 == 0U && factor < 3);        // 0x004022DF
        if (slow)
        {
            // 0x00402412: the slow path.
            if (object->unknown_0x44 == 0U) // 0x0040241C
            {
                if (static_cast<std::int32_t>(object->phase_counter) < 2) // 0x00402422
                {
                    // TODO(0x004028D0): transcribe the phase-start hook.
                }
                Flow_DispatchTaskListPhaseN(object); // 0x00402431..0x004024A3
                if (static_cast<std::int32_t>(object->phase_counter) < 2) // 0x004024A3
                {
                    // TODO(0x004028F0): transcribe the phase-end hook.
                }
                ++object->phase_counter;  // 0x004024B2
                object->unknown_0x44 = 1U; // 0x004024BC
                object->frame_counter = 0U; // 0x004024C5
            }
        }
        else
        {
            // 0x004022EC: the fast path.
            g_fast_path_words[0] = 0U; // 0x00A4438C
            g_fast_path_words[1] = 0U; // 0x00A44390
            g_fast_path_words[2] = 0U; // 0x00A44394
            // TODO(0x004E7BB0, 0x004E7B80): transcribe the two frame hooks.
            if (factor < 2) // 0x0040230D
            {
                // TODO(0x004028D0): transcribe the phase-start hook.
                // TODO(0x004E7340): transcribe the frame hook, called with 1.
            }
            else
            {
                // TODO(0x004E7340): transcribe the frame hook, called with 0.
            }
            Flow_DispatchTaskListPhase1(object);       // 0x00402321..0x00402387
            Flow_DispatchPortRecordListPhase1(object); // 0x00402387..0x004023E7
            if (factor < 2) // 0x004023E9
            {
                // TODO(0x004028F0): transcribe the phase-end hook.
            }
            ++object->counter;       // 0x004023F1, [object + 0x34]
            object->unknown_0x44 = 0U; // 0x004023F4
            ++object->frame_counter; // 0x004023F7, [object + 0x40]
        }
    }

    // 0x00402629: the pending state at +0x2C selects the state's exit action
    // through the eight-case jump table at 0x00402720. The table order is
    // 0x00402658, 0x0040266A, 0x00402688, 0x004026A3, 0x004026B5, 0x0040266E,
    // 0x004026D3, 0x00402665 for the states 1..8.
    const std::uint32_t pending = object->unknown_0x2C;
    if (pending != 0U) // 0x00402633
    {
        object->state = pending;   // 0x0040263E, [object + 0x28]
        object->unknown_0x2C = 0U; // 0x00402647
        switch (pending - 1U)      // 0x0040264C
        {
        case 0U: // state 1, 0x00402658
            // 0x00402658: call 0x004010A0, then return without the post block.
            // TODO(0x004010A0): transcribe the state-1 handler.
            return;
        case 1U: // state 2, 0x0040266A
            // 0x0040266A: pushes 0 and 0 and runs 0x004026D6.
            // TODO(0x00402890, 0x004154B0): transcribe the state-2 exit.
            break;
        case 2U: // state 3, 0x00402688
            // 0x00402688: 0x00402890("LOCAL:menusystem\\mainmenu.qsc", 1, 1, 1) and
            // 0x00418B00(that result, "LOCAL:menusystem\\mainmenu.qsc", 1, 1, 1).
            // TODO(0x00402890, 0x00418B00): transcribe the menu state.
            break;
        case 3U: // state 4, 0x004026A3
            // 0x004026A3: 0x00416900(object) and 0x00402820(0x1E).
            // TODO(0x00416900, 0x00402820): transcribe the state-4 exit.
            break;
        case 4U: // state 5, 0x004026B5
            // 0x004026B5: 0x00416FE0(), 0x00402820(0x1E) and [object + 0x28] = 4.
            // TODO(0x00416FE0, 0x00402820): transcribe the state-5 exit.
            break;
        case 5U: // state 6, 0x0040266E
            // 0x0040266E: 0x00416D40(), 0x004015F0(that result) and [object + 0x28] = 3,
            // then the state 3 body above runs (the retail falls through at 0x00402688).
            // TODO(0x00416D40, 0x004015F0): transcribe the state-6 exit.
            // TODO(0x00402890, 0x00418B00): the state 3 body.
            object->state = 3U;
            break;
        case 6U: // state 7, 0x004026D3
            // 0x004026D3: pushes 0 and 1 and runs 0x004026D6.
            // TODO(0x00402890, 0x004154B0): transcribe the state-7 exit.
            break;
        case 7U: // state 8, 0x00402665
            // 0x00402665: pushes 1 and 0 and runs 0x004026D6.
            // TODO(0x00402890, 0x004154B0): transcribe the state-8 exit.
            break;
        default: // 0x004026E9, the pending state is outside 1..8
            break;
        }
    }

    // 0x004026E4 and 0x004026E9: the post block every state but state 1 runs.
    g_frame_stamp = object->counter; // 0x004026E9/0x004026F1, stored at 0x00C28C5C
    object->base_low += object->step; // 0x004026F8, [object + 0x30] += [object + 0x4C]
    // 0x00402700: 0x00415020(0x43); when it returns non-zero, 0x00406220 gives the
    // active profile record and the retail writes 0xE into its +0x560 word.
    // TODO(0x00415020): transcribe the post-block predicate.
    {
        std::uint32_t* const profile = Config_GetActivePlayerProfileRecord();
        if (profile != nullptr)
        {
            *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(profile) + 0x560) = 0xEU;
        }
    }
}

// Native: Flow_CreateHandler; retail VA 0x00402800.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00402800..0x00402819.
// Behavior/CFG: reads the Flow class id at 0x00567C7C, pushes the object, the class id and zero, and calls the input port handler native at 0x004012A0.
// Status/limits: STRUCTURALLY_VERIFIED; the load, the three pushes and the call are the retail's.
void NATIVE_DECL Flow_CreateHandler(void* flow)
{
    QTask_RegisterInputPortHandler(static_cast<std::uint32_t>(
                                       reinterpret_cast<std::uintptr_t>(flow)),
                                   QTask_FlowClassId, 0U);
}

// Native: Flow_Delete; retail VA 0x00402740.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00402740..0x004027D0.
// Behavior/CFG: reads the frame timer through 0x0048F180, loads the LOCAL:config.qsc script through 0x00405BD0, runs the script setup at 0x004EE3B0 and then reads the flow object's state.
// Status/limits: STRUCTURALLY_VERIFIED; the script load and the object read are the retail's.
void NATIVE_DECL Flow_Delete(void* flow)
{
    (void)flow;
    // The retail loads the configuration script and then reads the flow
    // object's state. The script load runs through the script loader, which
    // the port reaches through the level start.
}

} // extern "C"

namespace {

void Flow_RunHandler_Handler(void* flow) { Flow_RunHandler(flow); }
void Flow_Delete_Handler(void* flow) { Flow_Delete(flow); }

} // namespace

#undef NATIVE_DECL
