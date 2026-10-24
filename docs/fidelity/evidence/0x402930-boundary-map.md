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
