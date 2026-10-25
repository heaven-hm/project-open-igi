// Native: AiDetection_RegisterTaskMask_489640 (behavior-derived placeholder name from the error string this routine can raise; retail symbol unknown);
//   retail VA 0x00489640-0x0048967D.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 62 bytes, 5 blocks, 16 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (5 blocks / 16 instructions / 62 bytes) plus the objdump listing
//   build/retail-dis.txt for the same extent. `px 64` at 0x005C8AC4 shows the image bytes there are ZERO - the 0x0181 sentinels this routine searches
//   for are not image values: 0x004895A6-0x004895B4 (native_489560.cpp) fills the same 0x40 bytes with `rep stosd` of EAX=0x01810181 and ECX=16, which
//   is exactly the initialiser this loop depends on. `ps` at 0x00541280 (byte-identical to "AIDetection - QTask mask is full"). Every callee was
//   swept against the tree's `// Native:` provenance index: the single call resolves to Script_WarningLog (error/native_4af810.cpp), and the array
//   0x005C8AC4 is OWNED by core/native_489560.cpp, so this file declares it extern rather than defining it.
// Behavior/CFG: scans the 32-word table at 0x005C8AC4 for the first slot still holding the unregistered sentinel 0x0181 - one word per iteration,
//   index in ecx, cursor in eax, terminating when the cursor reaches 0x005C8B04 (= 0x005C8AC4 + 32 words). If a free slot is found it stores the
//   sixteen-bit argument into that slot and returns `1 << slot`, i.e. the mask bit that corresponds to the slot just claimed. If all 32 slots are
//   taken it calls Script_WarningLog("AIDetection - QTask mask is full") and returns 0. Five basic blocks: 0x00489640 (prologue), 0x00489647 (test),
//   0x00489659 (table-full path), 0x00489669 (claim path), 0x00489676 (mask construction and return). 1 call site, 1 distinct callee, no loop-carried
//   state other than the slot index and cursor.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **This routine is not one of my lane targets: it is a 0x00489640 dependency of the lane target
//   0x00440FD0, which calls it once, and it had NO implementation anywhere in the tree ("*** MISSING ***" on the provenance sweep) - so it is
//   transcribed here as its own native rather than stubbed or inlined, per the one-implementation-per-address rule.** Its address lies in my original
//   write scope (0x0046xxxx-0x0048xxxx).
//   The sentinel comparison and the caller's store are the point of this file: **0x0181 is the "unregistered" value of a sixteen-bit task-type id
//   slot.** This routine is the second independent confirmation of that - it searches for 0x0181 and writes a real id over it - and the first
//   (0x0053C1DC, 0x0053C2D0, 0x0053C2FC, 0x0053C31C, 0x0053C2E0, 0x0053C4A8 all carrying the image value 0x0181 and all overwritten by `mov word
//   [slot],ax` right after a TaskType_Register call) is recorded in native_43e8d0.cpp. The two mechanisms differ and both are consistent: those slots
//   carry 0x0181 as an image value, while this table is ZERO in the image and is filled with 0x0181 at startup by 0x004895A6.
//   Unestablished: the meaning of the returned mask bit is not established by this routine alone (the caller stores it at 0x0057C198); that the table
//   holds exactly 32 slots is established by the loop bound (0x005C8B04 - 0x005C8AC4 = 0x40 bytes / 2), but nothing here proves that all 32 are
//   meaningful to the AI layer. **The loop bound comparison is a SIGNED `jl` (`cmp eax,0x5C8B04; jl`)** - reproduced as a plain pointer comparison,
//   which is equivalent because both addresses are below 0x80000000, and recorded here so a reviewer does not have to rediscover the polarity.
//   Unresolved: the 0x0181 sentinel is an observation about this image; no documentation claims it is a general IGI convention.
#include <cstdint>

extern "C" void Script_WarningLog(char const* const format, ...); // 0x004AF810 (error/native_4af810.cpp)

extern "C" {

// 0x005C8AC4: the task-mask table, 0x40 bytes. **OWNED by core/native_489560.cpp**, which defines it as 16 dwords and fills them with 0x01810181 at
// startup (0x004895A6-0x004895B4); this file only reads and writes through it, so it is declared extern here with the owner's exact type.
extern std::uint32_t g_5c8ac4[16]; // 0x005C8AC4

std::uint32_t AiDetection_RegisterTaskMask_489640(std::uint16_t const arg1)
{
    // 0x00489640: xor ecx,ecx - the slot index starts at zero; 0x00489642: mov eax,0x005C8AC4 - the cursor starts at the table base. This body has
    // no frame and no saved-register pushes: the argument arrives at [esp+4] and is read once, in the claim path.
    std::uint16_t* const mask_table = reinterpret_cast<std::uint16_t*>(::g_5c8ac4);
    std::int32_t slot = 0;
    std::uint16_t* cursor = mask_table;
    const std::uint16_t* const mask_table_end = mask_table + 32; // 0x00489652: cmp eax,0x005C8B04 (0x40 bytes / two bytes per slot)

    for (;;)
    {
        // 0x00489647: cmp word [eax],0x181 - is this slot still unregistered? 0x0048964C: je 0x00489669 - the sentinel means it is free.
        if (*cursor == 0x181U)
        {
            // 0x00489669: mov ax,word [esp+4] (the argument); 0x0048966E: mov word [ecx*2+0x005C8AC4],ax - claim the slot.
            mask_table[slot] = arg1;
            // 0x00489676: mov eax,1; 0x0048967B: shl eax,cl - the caller's mask bit for this slot. Returned as a 32-bit value.
            return static_cast<std::uint32_t>(1U) << slot;
        }
        // 0x0048964E: add eax,2 - advance one WORD; 0x00489651: inc ecx - advance the slot index; 0x00489652: cmp eax,0x005C8B04;
        // 0x00489657: jl 0x00489647 - signed loop while the cursor is below the table end.
        cursor += 1;
        slot += 1;
        if (!(cursor < mask_table_end))
            break;
    }
    // 0x00489659: push 0x00541280 ("AIDetection - QTask mask is full"); 0x0048965E: call 0x4AF810; 0x00489663: add esp,4;
    // 0x00489666: xor eax,eax - the table-full result is zero, so the caller sees "no mask bit".
    Script_WarningLog("AIDetection - QTask mask is full");
    return 0U;
}

} // extern "C"
