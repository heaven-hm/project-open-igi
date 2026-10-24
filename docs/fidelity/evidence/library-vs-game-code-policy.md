# What to include and what to exclude: library code in a matching decompilation

Written after the 0x00401400 question, with external references, because the address-range heuristic this work had been using is
not sufficient and produced at least one wrong classification.

## Consensus from existing decompilation projects

- **Matching decompilations exclude the statically linked runtime and middleware, and record that exclusion.** meteor-decomp's plan
  states its non-goal as decompiling "MSSMIXER / Miles Sound System / DirectX wrapper glue / VC++ runtime / STL", and prefers
  functional re-derivation for the long tail rather than matching every byte.
  <https://www.garlemaldsoftware.com/projects/meteor-decomp/docs/plan/>
- **Library code is identified by signature, not by address.** RetDec ships YARA signature files for statically linked compiler
  runtimes precisely so that this code can be *removed* before decompiling, and documents that you must write your own signatures for
  other runtimes. <https://github.com/avast/retdec/wiki/Removing-Statically-Linked-Code>
- **IDA's FLIRT is the same idea**: library functions are recognised by byte/CRC signatures, can be named automatically, and can be
  hidden from the analyst. <https://docs.hex-rays.com/core/flirt/concepts/ida-f.l.i.r.t.-technology-in-depth>
- **HARD signals beat heuristics.** A production classifier for this problem triggers on canonical library symbols (mangled operators
  like `??2@YAPAXI@Z`, decorated CRT imports like `_strtoi64@4`, `std::_*`, SEH machinery) and treats region or caller-count as
  supporting evidence only. <https://github.com/bethington/ghidra-mcp/commit/4b866560>
- **Everything that is the game's own code is in scope**, including utilities that merely look generic; a N64 matching decompilation
  decomposes all code except handwritten assembly, which is kept as assembly.
  <https://github.com/zeldaret/tmc/blob/master/CONTRIBUTING.md>

## Policy for this project

1. **Exclude only what is provably the toolchain's**: the CRT/startup code the linker equivalent supplies, C++ runtime and STL
   machinery, SEH scaffolding, and third-party middleware. Evidence of exclusion must name the library function (or the runtime
   contract it implements), not merely its address.
2. **Include everything else**, including small utilities with hundreds of callers, and including code that sits at low addresses next
   to the CRT.
3. **A retail call site into excluded code is expressed as the candidate's equivalent runtime call**, with the same arguments and return
   value, and the substitution is recorded in the record's boundary note.
4. **No stubbing and no guessing**: if the callee's identity is not established, the caller stays blocked.
5. **Address ranges are a hint, never the evidence.** The 0x00401000-0x00402000 range was treated as CRT in earlier rounds; that was
   an inference, and the case below shows it was wrong for at least one function.

## Case study: 0x00401400 is game code - and it was already transcribed

Earlier rounds parked 0x00401400 as a "printf-family CRT internal" because it lives at 0x00401400 and writes through a table. Two parts
of that were wrong, and both are recorded here rather than quietly dropped.

**It is game code.** 1312 call sites; Frida-observed startup call from 0x004C59A4 with arguments `(2, 36, 0x004C5A10)`; counters at
0x00567C6C and 0x00536148; a table at 0x00A96AE0 indexed by `(id & 0xFF) * 0x180 + (id & 0xFFFF)`; a recursive walk over 2048 records of
0x30 bytes from 0x00AF5EE0; and the table is later called indirectly at 0x004012DE (`call *%eax`) - it is a QTask event-handler table.

**It was already transcribed.** The tree already owns it as `QTask_LinkQTaskEventHandler` in
`engine/src/retail/core/task_types.cpp`, status STRUCTURALLY_VERIFIED, CFG 1.000, together with 0x004014B0
(`TaskType_RefreshHandlerMask`) and 0x004014E0 (`TaskType_HandlerPhaseMask`). Round 1 of this goal briefly re-derived all three and wrote
duplicate implementations under `engine/src/retail/sys/`; the build did not catch it because the duplicates used different symbol names,
which is exactly the duplicate-implementation smell rule 13 forbids. Those files were deleted, the CMake entries and the extra manifest
record were reverted, and the originals stay the single implementation.

**Why the false blocker happened.** The landability analysis built its "implemented" set by globbing `engine/src/retail/*/native_*.cpp`.
That misses every native implemented in a hand-named file - `task_types.cpp`, `flow.cpp`, `game_flow.cpp` and others. The set is now taken
from `docs/fidelity/native-verification.json`, which is the single owner of implementation paths: **1285 addresses implemented**, of which
88 are inside the CRT ranges.

## Recomputed chain landability

With the manifest as the implementation set, these direct callees of 0x00402930 have **zero unimplemented direct call targets**:

| VA | instructions | note |
| --- | --- | --- |
| 0x0042DAC0 | 15 | registers two QTask handlers |
| 0x0042FE40 | 15 | registers two handlers of the same shape |
| 0x0046DF10 | 22 | - |
| 0x00415070 | 30 | - |
| 0x004688B0 | 31 | - |
| 0x00468E20 | 31 | - |
| 0x004851B0 | 31 | - |
| 0x00424B90 | 33 | - |

They are not yet transcribed for a newly established reason: the registrars push **code addresses as handler arguments**, and those
handlers are functions that the CFG recovery never listed as function starts.

- 0x0042DAC0 pushes 0x0042DB00 and 0x0042DCE0.
- 0x0042FE40 pushes 0x0042FE80 and 0x0042FEF0.

0x0042DCE0 is `mov 0x8(%esp),%eax; movl $0x2C0,(%eax); ret` (11 bytes) and 0x0042FEF0 is the same shape writing `0x2F0`; 0x0042DB00 and
0x0042FE80 are real bodies with prologues (`push ebx; push ebp; push esi; ...`) that end before the next detected entry. Neither has a
known extent from CFG recovery, so their boundaries have to be established from the instructions before they can be transcribed - the
registrar cannot link without them, and inventing a signature for a handler the ABI does not establish would fabricate the call contract.

Next increment: establish the extent and ABI of 0x0042DCE0/0x0042FEF0 (trivial, three instructions each) and of 0x0042DB00/0x0042FE80, then
transcribe the four handlers and the two registrars. That is the shortest real path into the startup chain.

## Handler extents established, and an ABI discrepancy that blocks them

Measured this round, so the next session starts from facts:

- **0x0042DB00**: 171 instructions, prologue `push ebx; push ebp; push esi; mov 0x14(%esp),%esi; push edi`, body ends at 0x0042DCD8
  with nop padding through 0x0042DCDF, so the next entry is 0x0042DCE0. It reads the first argument as a pointer to two dwords
  (`mov (%esi),%ebp; mov 0x4(%esi),%edi`), calls 0x004EE0E0 and 0x004F45F0, and indexes 0x00A96AE0 with the same `(x & 0xFF) * 0x180 +
  (y & 0xFFFF)` shape as the linker.
- **0x0042DCE0**: 11 bytes, `mov 0x8(%esp),%eax; movl $0x2C0,(%eax); ret`, then nop padding to 0x0042DCF0.
- **0x0042FE80** and **0x0042FEF0** are the same pair for the second registrar; 0x0042FEF0 writes `0x2F0` instead of `0x2C0`.

**The blocker**: `engine/include/igi/retail/core/task_types.hpp` declares `using FrameHandler = void (*)(void*);` - one argument - but
the call site that invokes a linked handler passes two:

```
0x4012D8: mov 0x14(%esp),%edx
0x4012DC: push %edx
0x4012DD: push %esi
0x4012DE: call *%eax
```

and 0x0042DCE0 reads its second stack argument (`mov 0x8(%esp),%eax`) and writes through it. So at least some handlers are
`void (*)(void* record, void* arg)`, while the tree's type says one parameter. Transcribing handlers against the one-argument type would
silently drop the second argument and fabricate the contract, so the type and the registrar signatures have to be settled from retail first
(which handlers read a second argument, and whether the second argument is a record's field pointer or a frame value). That is the first
task of the next increment; the handler bodies are small enough to land in one pass once the ABI is decided.
