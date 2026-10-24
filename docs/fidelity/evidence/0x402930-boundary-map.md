# Boundary map: 0x00402930 direct callees

Measured from the retail disassembly at the exact retail function boundary (0x00402930..0x00402930+780).

- direct `call` sites: **155**
- distinct targets: **154**
- already implemented in this tree: **21** (0x4C1780, 0x489560, 0x4825C0, 0x47CAB0, 0x465A10, 0x464260, 0x45E450, 0x45DB70, 0x45CAB0, 0x459880, 0x456AD0, 0x454970, 0x44C8D0, 0x422B40, 0x415530, 0x40FE30, 0x407D60, 0x406450, 0x402FF0, 0x402F50, 0x401FC0)
- missing: **133**
- CRT helpers among the direct callees: **0** - every one is game code, so none can be delegated to the toolchain

The in-degree column is the number of call sites to that target across the whole image. Apart from 0x004C1780 (162 callers,
already implemented) and 0x00489560, every direct callee is called exactly once: 0x00402930 calls each registration routine in
sequence. That is why the boundary is 133 separate transcriptions rather than a few shared ones, and why the closure explodes
past them instead of collapsing.

## Missing direct callees, in retail call order

| target | image in-degree |
| --- | --- |
| `0x407290` | 1 |
| `0x413DF0` | 1 |
| `0x415070` | 1 |
| `0x4170A0` | 1 |
| `0x4190E0` | 1 |
| `0x419760` | 1 |
| `0x4199F0` | 1 |
| `0x41A1E0` | 1 |
| `0x41AAF0` | 1 |
| `0x41B310` | 1 |
| `0x41B8E0` | 1 |
| `0x41C700` | 1 |
| `0x41D080` | 1 |
| `0x41DC50` | 1 |
| `0x41E690` | 1 |
| `0x41F340` | 1 |
| `0x420300` | 1 |
| `0x420CC0` | 1 |
| `0x421430` | 1 |
| `0x421750` | 1 |
| `0x4223D0` | 1 |
| `0x4230A0` | 1 |
| `0x423FC0` | 1 |
| `0x424B90` | 1 |
| `0x424E80` | 1 |
| `0x425C80` | 1 |
| `0x426630` | 1 |
| `0x426D90` | 1 |
| `0x4272A0` | 1 |
| `0x427830` | 1 |
| `0x427D90` | 1 |
| `0x427FF0` | 1 |
| `0x42ACC0` | 1 |
| `0x42BEB0` | 1 |
| `0x42D2B0` | 1 |
| `0x42DAC0` | 1 |
| `0x42DCF0` | 1 |
| `0x42FE40` | 1 |
| `0x42FF00` | 1 |
| `0x4310F0` | 1 |
| `0x4330F0` | 1 |
| `0x4351E0` | 1 |
| `0x435870` | 1 |
| `0x435D90` | 1 |
| `0x4361D0` | 1 |
| `0x436780` | 1 |
| `0x436B10` | 1 |
| `0x437510` | 1 |
| `0x438390` | 1 |
| `0x4388B0` | 1 |
| `0x438DF0` | 1 |
| `0x4397C0` | 1 |
| `0x43A840` | 1 |
| `0x43AD80` | 1 |
| `0x43B2F0` | 1 |
| `0x43B850` | 1 |
| `0x43BFD0` | 1 |
| `0x43C6D0` | 1 |
| `0x43CC70` | 1 |
| `0x43E4F0` | 1 |
| `0x43E8D0` | 1 |
| `0x43EFF0` | 1 |
| `0x43F900` | 1 |
| `0x4403E0` | 1 |
| `0x440710` | 1 |
| `0x440C20` | 1 |
| `0x440FD0` | 1 |
| `0x443680` | 1 |
| `0x443ED0` | 1 |
| `0x444BF0` | 1 |
| `0x4464C0` | 1 |
| `0x447DB0` | 1 |
| `0x4485E0` | 1 |
| `0x448D30` | 1 |
| `0x449360` | 1 |
| `0x449970` | 1 |
| `0x449D30` | 1 |
| `0x44C130` | 1 |
| `0x4533B0` | 1 |
| `0x453550` | 1 |
| `0x465BC0` | 1 |
| `0x4663D0` | 1 |
| `0x466E40` | 1 |
| `0x466F70` | 1 |
| `0x467700` | 1 |
| `0x4682F0` | 1 |
| `0x4688B0` | 1 |
| `0x468E20` | 1 |
| `0x4696B0` | 1 |
| `0x46AB60` | 1 |
| `0x46B510` | 1 |
| `0x46B7F0` | 1 |
| `0x46BA60` | 1 |
| `0x46C0C0` | 1 |
| `0x46CB70` | 1 |
| `0x46D420` | 1 |
| `0x46DB40` | 1 |
| `0x46DD00` | 1 |
| `0x46DF10` | 1 |
| `0x46E370` | 1 |
| `0x46F2B0` | 1 |
| `0x470410` | 1 |
| `0x470610` | 1 |
| `0x470FA0` | 1 |
| `0x472670` | 1 |
| `0x472A70` | 1 |
| `0x472D00` | 1 |
| `0x4733F0` | 1 |
| `0x4739D0` | 1 |
| `0x473ED0` | 1 |
| `0x475380` | 1 |
| `0x475AC0` | 1 |
| `0x476110` | 1 |
| `0x476730` | 1 |
| `0x477220` | 1 |
| `0x477E30` | 1 |
| `0x47B4B0` | 1 |
| `0x47CF20` | 1 |
| `0x47CF70` | 1 |
| `0x47E700` | 1 |
| `0x4801B0` | 1 |
| `0x4813E0` | 1 |
| `0x484F30` | 1 |
| `0x4851B0` | 1 |
| `0x485330` | 1 |
| `0x485A40` | 1 |
| `0x486900` | 1 |
| `0x4872C0` | 1 |
| `0x487BE0` | 1 |
| `0x488620` | 1 |
| `0x488800` | 1 |
| `0x488BB0` | 1 |
| `0x508F80` | 1 |

## What this implies for the next attempt

Transcribing 0x00402930 itself is not the hard part: it is 780 bytes of sequential calls. The hard part is that all 133 callees
must exist first, and each of those drags its own dependency closure - this is the same 1,670-native figure measured before, seen
from the boundary end instead of the leaf end.

A workable order is therefore:

1. take one direct callee at a time, in retail call order, and close its own closure first (the in-degree ranking inside that
   sub-closure is the same method that worked for the getter waves);
2. transcribe 0x00402930 only when all 133 are present, because a partial version with 133 unresolved extern calls cannot link and
   must never be stubbed;
3. keep the project-authored guard in place until then - it fails closed, which is the honest behaviour.

No native was landed from this analysis: it is a map, and it says plainly that the remaining work is 133 closures, not 133 functions.

## What the first callees actually are (round 238)

Sampling the first missing callees shows they are window-class registration routines, not game logic: 0x004813E0 (256 instructions)
and 0x00488620 (56 instructions) both fill a WNDCLASS-style block, call a registrar, then create and show a window, and both store the
resulting class atom into a two-byte static (0x0054097C and 0x005C8A90).

Their calls do not go to import thunks. They go through function-pointer globals:

    0x00401400: mov 0x00567C6C,%ecx     ; entry point of one routine
    0x00401530: mov 0x00567C70,%ecx     ; entry point of another

so a faithful transcription of any of these callees also needs the ownership of 0x00567C6C and 0x00567C70, plus the routines they
point at. That is a third dependency layer beyond the callee list, and it is the reason this boundary is not a "just transcribe 133
functions" job: the first three sampled callees already pull in platform entry pointers that the candidate must obtain from the host
rather than from retail bytes.

Recorded, not acted on: transcribing a window registration that calls through a retail function pointer requires knowing what that
pointer is initialised to, and inventing that binding is exactly what the rules forbid. The next increment is to trace where
0x00567C6C and 0x00567C70 are assigned.

## Third layer resolved: the pointer globals are CRT internals (round 239)

The two globals that the first window-registration callees call through are assigned only inside the routines that read them:

    0x00401435: mov %ecx,0x00567C6C        0x0040149D: mov %eax,0x00567C6C
    0x0040155C: mov %ecx,0x00567C70        0x004015C7: mov %eax,0x00567C70

and those routines live in 0x00401000-0x00402000, the MSVC CRT/startup support region of this image. 0x00401400 increments a counter
at 0x00536148, forms an index of `(arg & 0xff) * 3 << 7` from its arguments and writes through a large table at 0x00A96AE0; it is
runtime-support code with a cached entry pointer, not game state.

Consequence, and it is the useful part of this round: the "third dependency layer" is compiler/runtime territory. Under the standing
boundary rule the candidate supplies CRT behaviour from its own toolchain rather than transcribing retail's internal stubs, so those two
globals do not need retail transcription and must not be declared as game natives. The window-registration callees are therefore gated
only on their game-side dependencies - the WNDCLASS statics (0x0054097C, 0x005C8A90), the string pointers they pass, and the routines
they call that are not in the CRT range - and those are tractable in the same way the getter waves were.

So the dependency tree of the boundary ends at CRT on one side and at game statics on the other. The next increment is one
window-registration callee transcribed end to end, with every CRT call left to the toolchain and every game static owned explicitly.


## Tractability ranking of the 133 callees (round 240)

Sorted by instruction count, with the number of dependencies that are still missing game natives (CRT calls excluded):

| callee | instructions | missing game deps | first calls |
| --- | --- | --- | --- |
| `0x47CF20` | 10 | 2 | `0x4F1CD0`, `0x4F1DB0` |
| `0x42DAC0` | 15 | 2 | `0x4EE0E0`, `0x4EE0F0` |
| `0x42FE40` | 15 | 2 | `0x4EE0E0`, `0x4EE0F0` |
| `0x46DF10` | 22 | 1 | `0x4F45F0` |
| `0x472A70` | 27 | 1 | `0x460C10` |
| `0x415070` | 30 | 1 | `0x4E68D0` |
| `0x4688B0` | 31 | 0 |  |
| `0x468E20` | 31 | 0 |  |

The smallest callee, `0x0047CF20` at 10 instructions, is the honest illustration of why none of these can be landed yet:

    0x0047CF20: push $0x005C10F0        ; argument
    0x0047CF25: call 0x004F1CD0        ; game, MISSING
    0x0047CF2A: push $0x005408EC
    0x0047CF2F: push $0x005C10F0
    0x0047CF34: call 0x004F1DB0        ; game, MISSING
    0x0047CF39: push $0x0
    0x0047CF3B: call 0x004017C0        ; CRT range
    0x0047CF43: mov %al,0x005C10E8     ; stores the result byte
    0x0047CF48: ret

Two of its three calls are missing game natives, so it cannot be transcribed without stubbing - which is forbidden. The same pattern holds
for the rest of the ranking: every callee is small in itself and gated on one or two smaller missing routines.

The correct order is therefore leaf-first inside the boundary, not boundary-first: close `0x004F1CD0` and `0x004F1DB0` (both small, both
called from several registration routines), then the callee that uses them becomes landable. That is the same in-degree logic that worked
for the getter waves, applied one level down from the boundary instead of at the leaves of the whole image.

## The two dependencies are themselves gated (round 241)

Reading the two routines the smallest boundary callee needs:

    0x004F1CD0 (6 instructions): tail-constructs a list head with context 0x004F1CF0
      0x004F1CD4: push $0x004F1CF0        ; context argument is a CODE address
      0x004F1CD9: push %eax               ; the caller's object
      0x004F1CDA: call 0x004F1D60         ; already transcribed (Sys_InitListHead_4F1D60)

    0x004F1DB0 (22 instructions): allocates 0x480C bytes via 0x004B0C60, then initialises regions through
      0x004AF9D0 and 0x004AF8F0.

0x004F1CD0 is gated for a subtle reason worth recording: its second argument is the *code address* 0x004F1CF0, so a faithful
transcription must reference that routine by its candidate address rather than store the retail number. That makes 0x004F1CF0 a
dependency of 0x004F1CD0 even though it is never called from it - a data reference to code, not a call. 0x004F1DB0 needs the
allocator 0x004B0C60 and the two initialisers, none of which are transcribed.

So the boundary's dependency tree does not stop one level below the callees: it continues through code-address constants and
allocator helpers. This is recorded so the next attempt does not mistake "small callee" for "one-step callee". The tractable order
remains leaf-first, but the leaf set for the boundary is those allocator/initialiser helpers, not the registration routines.

## 0x004AF9D0 is out of reach as a leaf (round 243)

Reading it settles the question the last round left open: 0x004AF9D0 is not a small initialiser. It is 329 instructions with two
jump tables (0x004AFE78 and 0x004AFF04), a 0x40-dword scratch class compared with `repz cmpsl`, `rep stos` fills using
0x01010101, and per-slot flag words written to `+0x08` with values 0x100, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000,
0x10000, 0x20000 and 0x40000. That is a character-pattern compiler: it walks a bracketed pattern, expands `\` escapes and
`a-z` ranges, and emits a 0x14-byte record per slot.

It also calls 0x004AFFB0 and 0x004AFF90, neither transcribed, and those two sit beside it in 0x004AF000-0x004B0000 with the same
shape of jump-table dispatch.

Consequence for this work: this is not a leaf in any practical sense, and it may be compiler-library code rather than game logic
(the bracket/escape expansion with `repz cmpsl`/`rep stos` idioms is what an MSVC regex or wildcard matcher looks like). Deciding that
honestly needs the two callees read first, which is a round of its own, so no transcription was attempted and nothing was stubbed.

What this does mean for the boundary: 0x004F1DB0's dependence on 0x004AF9D0 does not make 0x004F1DB0 landable now, and 0x004F1CD0 is
gated on the code-address constant 0x004F1CF0 (which has no function body at all - the address is referenced as data only). The two
small boundary dependencies turned out to be a list helper that is already landed (0x004AF8F0) and a 329-instruction pattern compiler.

## The 0x004AF000-0x004B0000 region is CRT, and one landed native belongs to it (round 244)

Two measurements settle the region's nature.

First, its callers are the CRT startup code, not game code:

    0x0040124A -> 0x004AF810
    0x00401280 -> 0x004AF810
    0x004012BF -> 0x004AF8F0
    0x004013CF -> 0x004AF910
    0x004015DC -> 0x004AF960

Second, its own helpers are CRT idioms: 0x004AFF90 is a 0x40-dword fill with 0x01010101 followed by `movl $1,(%eax)` - a
class-table initialiser - and 0x004AFFB0 sets up a 0x414-byte frame and walks the same structures. Combined with 0x004AF9D0's
bracket/escape compiler, the region is the C runtime's pattern and list support, supplied by the toolchain rather than transcribed.

### Correction to an earlier landing

0x004AF8F0 - which was landed as `Sys_InsertListTail_4AF8F0` and reviewed as game code - is called from 0x004012BF, inside the CRT
startup region. On this evidence it is almost certainly CRT list support (the same list layout 0x004F1D60/0x004AF8F0 manipulate is a
runtime list), not a game native. It must not be counted as boundary progress.

It stays in the tree as a TRANSCRIBED record with its evidence, because deleting transcribed retail code would hide the work, but its
classification is corrected here: the record's role is "CRT helper mistakenly landed as a game native", and it should not be used to
justify further boundary work. The honest count of boundary-relevant landings from rounds 242-243 is therefore zero, not one.
