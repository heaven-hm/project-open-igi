// Native: AIGraph_InitStep_530270 (blessed address-suffixed placeholder; retail symbol not established); retail VA 0x00530270-0x00530292.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 35 bytes (0x23), 1 block, 0 edges,
//   7 instructions, counted over the exact image; an independent ndisasm 3.01 linear sweep of the same 35-byte extent also yields 7 and ends on the `jmp`. Extent ends at the tail `jmp` at 0x0053028E; 0x00530293 onward is `0x90` padding, not part of this routine.
// Tools/evidence: ndisasm 3.01 linear sweep over the exact 35-byte extent (authoritative instruction count and byte accounting; radare2 6.2.0 `pd` was
//   found to drop instructions silently elsewhere in this set and is not relied on for counts), and scripts/native_fidelity_audit.py's documented rules (rule 22: the extent is bounded by the
//   terminator, not by the gap to the next start; rule 14: read the byte after the terminator). The complete extent is six `E8` calls followed by one
//   `E9` tail jump: 0x00530270 call 0x005309C0; 0x00530275 call 0x00530960; 0x0053027A call 0x004EC160; 0x0053027F call 0x004EBE70;
//   0x00530284 call 0x004C1780; 0x00530289 call 0x00530800; 0x0053028E jmp 0x004ED000. No push occurs before any call, so all seven targets
//   take zero stack arguments. The tail target 0x004ED000 is a separate routine (`mov dword [0xa5ef98],0` then `ret` at 0x004ED00A, then padding),
//   which is why the jump is a genuine tail call and not internal control flow - it is a distinct function with its own epilogue.
// Behavior/CFG: no loop and no conditional branch - one straight-line block of 7 instructions. It calls six routines in retail's exact order and then
//   continues into 0x004ED000 as a tail call, so whatever 0x004ED000 does is the last thing this routine does. Nothing is loaded, stored or tested
//   here: the whole body is the call sequence, so the order of the six calls is the entire contract of this routine.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the disassembly above; runtime UNOBSERVED. Recorded limits: (a) the name is the blessed
//   address-suffixed placeholder and no descriptive name is claimed. (b) Six of the seven targets (0x005309C0, 0x00530960, 0x004EC160, 0x004EBE70,
//   0x00530800, 0x004ED000) are untranscribed, so this file reports them undefined in the tree; that is the true state of the frontier, not a defect
//   here. (c) 0x004C1780 is transcribed as AnimSound_LoadParameters and is called through its header declaration. (d) The return values of the six
//   calls are not read by this body, so none is carried.
#include <igi/retail/core/native_4c1780.hpp>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

extern void Sub_5309C0(); // 0x005309C0: untranscribed
extern void Sub_530960(); // 0x00530960: untranscribed
extern void Sub_4EC160(); // 0x004EC160: untranscribed
extern void Sub_4EBE70(); // 0x004EBE70: untranscribed
extern void Sub_530800(); // 0x00530800: untranscribed
extern void Sub_4ED000(); // 0x004ED000: untranscribed; reached by tail jump, so it is this routine's continuation

void NATIVE_DECL AIGraph_InitStep_530270()
{
    Sub_5309C0();              // 0x00530270: call 0x005309C0
    Sub_530960();              // 0x00530275: call 0x00530960
    Sub_4EC160();              // 0x0053027A: call 0x004EC160
    Sub_4EBE70();              // 0x0053027F: call 0x004EBE70
    AnimSound_LoadParameters(); // 0x00530284: call 0x004C1780
    Sub_530800();              // 0x00530289: call 0x00530800
    Sub_4ED000();              // 0x0053028E: jmp  0x004ED000 - tail call, the last thing this routine does
}

} // extern "C"

#undef NATIVE_DECL
