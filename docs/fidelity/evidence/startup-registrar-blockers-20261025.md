# Who blocks the startup registrars, 2026-10-25

Question: the startup sequence `0x00402930` still calls 139 natives we have not transcribed, and a worker can only carry a few per pass. Which ones
can be taken right now, and which single natives would unblock the most of the rest?

Method: take the 154 distinct callees of `0x00402930` from the retail listing (`build/retail-dis.txt`), subtract everything the engine implements
(`scripts/native_coverage.py` engine scan), the 17 PE import slots and the known CRT routines, leaving 139. For each of those, walk its body forward
from its entry in the listing (cap 2400 bytes), collect the `call` targets, and keep only the ones that are themselves unimplemented. A body whose
forward walk hits a conditional jump or a tail `jmp` is flagged uncertain (12 of them) because a linear walk can undercount blocks - those numbers
are planning data, not transcriptions.

## Headline

- **71 of the 139 registrars have no unimplemented callee at all.** They are ready to transcribe today; nothing blocks them.
- **4 more are blocked only by natives inside `0x00415xxx`-`0x00419xxx`**, needing just two callees: `0x004169D0` (required by 10 registrars) and
  `0x00416870` (1).
- **64 are blocked by something else**, and the blockers concentrate hard:

| blocker | registrars it blocks |
| --- | --- |
| `0x00467D90` | 17 |
| `0x004E6610` | 9 |
| `0x00460BF0` | 8 |
| `0x004A54A0` | 7 |
| `0x00401FA0` | 6 |
| `0x00413B30`, `0x00413B40` | 6 each |
| `0x00460BE0`, `0x00460C40`, `0x00460C50`, `0x0047E990`, `0x00460C10` | 5 each |
| `0x004F1DB0`, `0x004978D0`, `0x004978F0`, `0x004CEBA0` | 3 each |

## What this changes about scheduling

Transcribing registrars alphabetically or in call order wastes the leverage the table above exposes. The cheapest path to a much higher
`0x00402930` closure is:

1. `0x004169D0` and `0x00416870` - two small config leaves that unblock four registrars and also unblock the two registrars the startup-chain lane
   reported as blocked (`0x0047E700`, `0x00476110`).
2. `0x00467D90` - one native that unblocks 17 registrars, the single highest-leverage body in the sequence.
3. Then `0x004E6610`, `0x00460BF0`, `0x004A54A0`, `0x00401FA0`, `0x00413B30`/`0x00413B40`, which together account for most of the remaining 64.
4. Everything else in the ready-71 list can be taken in parallel at any time, since nothing blocks them.

Caveat worth repeating: a "ready" registrar is ready in the sense that its callees exist, not that it has been transcribed or verified. Each one
still needs its own per-instruction provenance, per-block counts, cleanup arithmetic and record before it counts towards closure.

## Addendum: `0x004A54A0` is not a game native

Follow-up measurement corrected the table above: `0x004A54A0` is MSVC `__chkstk`, the 4 KB stack-probe helper (- push ecx; cmp eax,0x1000;
lea ecx,[esp+8]; jb done; page-touch loop; sub ecx,eax; mov eax,esp; test [ecx],eax; mov esp,ecx; ... push eax; ret -), confirmed against the
image at `0x004A54A0..0x004A54CE`. It sits between the known CRT routines `0x004A53B3` (sprintf) and `0x004A5500` (strncpy) and is compiler-emitted,
so it is classified CRT and must never be hand-transcribed: a C++ transcription of it would be an invented native.

Consequences:
- The 7 registrars counted behind it are not blocked on a missing game native. They are free to transcribe, but each owns a stack frame larger than
  0x1000, so each has a real local array of at least 4 KB that must be reproduced from the retail frame size rather than invented.
- `build/retail-dis.txt` does not render the calls to it at all (grep for `call 0x4a54a0` returns nothing), while `r2 /r 0x4a54a0` lists callers such as
  0x004170A5, 0x0041AAF5, 0x0041B8E5, 0x0042DCF5, 0x004310F5, 0x0044C8D5, 0x0046E375, 0x00485DB5. Any tool that counts callees from that listing is
  blind to this one, so the CRT classification is done in `scripts/native_coverage.py` rather than by the listing.
