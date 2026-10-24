# The 0x00402930 startup chain: what blocks it, measured

0x00402930 is the project-owned fail-closed boundary: it is the retail startup init sequence, 155 ordered
init/open/load calls ending in a tail jump to 0x004CEF30, and the candidate cannot run past it. Until the chain is
transcribed the candidate executes no game native at all, so no paired retail/candidate runtime comparison can succeed
and no native can be promoted to VERIFIED on both-sides evidence.

Chain calls: 155 (154 distinct). In catalog: 22. Missing: 133.

Landability is measured per native's own body, not by distance in the call graph: a chain entry is landable when every
callee and every code-address immediate its body uses is already transcribed (AGENTS.md section 21.7).

## Landable now with no new dependency: 0 (after the correction below)


## One to four missing dependencies (the next wave): 21

- 0x488620: 56 instructions, 201 bytes; missing calls ['0x4f0ff0']; missing code immediates ['0x4886f0', '0x488770', '0x488780']
- 0x415070: 30 instructions, 109 bytes; missing calls []; missing code immediates ['0x4150e0', '0x415380', '0x415450']
- 0x486900: 46 instructions, 170 bytes; missing calls []; missing code immediates ['0x4869b0', '0x486cd0', '0x486d70', '0x486f30']
- 0x485A40: 36 instructions, 132 bytes; missing calls []; missing code immediates ['0x485b20', '0x485b40', '0x486010', '0x486020']
- 0x4851B0: 31 instructions, 110 bytes; missing calls []; missing code immediates ['0x424280', '0x485220']
- 0x484F30: 160 instructions, 527 bytes; missing calls ['0x485170', '0x4b8a80']; missing code immediates []
- 0x47E700: 34 instructions, 128 bytes; missing calls ['0x416930', '0x4169d0']; missing code immediates ['0x47e790', '0x47e820']
- 0x47CF20: 10 instructions, 40 bytes; missing calls ['0x4f1cd0', '0x4f1db0']; missing code immediates []
- 0x413DF0: 99 instructions, 407 bytes; missing calls ['0x4f1cd0', '0x4f1db0']; missing code immediates ['0x413f90']
- 0x472A70: 27 instructions, 103 bytes; missing calls ['0x460c10', '0x477bf0']; missing code immediates ['0x472ae0', '0x472ca0']
- 0x46DF10: 22 instructions, 80 bytes; missing calls ['0x4f45f0']; missing code immediates ['0x46df70']
- 0x46AB60: 34 instructions, 125 bytes; missing calls []; missing code immediates ['0x46abe0', '0x46ac30', '0x46ae20', '0x46ae30']
- 0x468E20: 31 instructions, 110 bytes; missing calls []; missing code immediates ['0x468e90', '0x469110']
- 0x4688B0: 31 instructions, 110 bytes; missing calls []; missing code immediates ['0x468920', '0x468bc0']
- 0x4533B0: 56 instructions, 182 bytes; missing calls []; missing code immediates ['0x453470', '0x453480', '0x453490']
- 0x449D30: 69 instructions, 274 bytes; missing calls ['0x4b4720', '0x4b4770']; missing code immediates ['0x449e50', '0x44a1a0']
- 0x438390: 42 instructions, 162 bytes; missing calls ['0x4169d0']; missing code immediates ['0x438440', '0x4387b0', '0x46fb50']
- 0x436B10: 102 instructions, 321 bytes; missing calls []; missing code immediates ['0x436c60', '0x436d10', '0x4374c0', '0x4374d0']
- 0x425C80: 78 instructions, 262 bytes; missing calls []; missing code immediates ['0x425d90', '0x425de0', '0x426590', '0x4265a0']
- 0x424B90: 33 instructions, 120 bytes; missing calls []; missing code immediates ['0x424c10', '0x424c40', '0x424ca0']

## Everything else: 111 natives, each needing five or more dependencies.

## Consequence for the work order

Working the chain in retail order is correct but not sufficient: most entries reference handler addresses and helper
natives that are themselves untranscribed, so the chain must be filled by dependency waves - a native whose body only
uses already-landed callees and already-landed code addresses first, then the natives that reference it. The
one-per-round habit therefore has to be one *wave* per round, not one chain index per round, or the boundary never
clears.

## Correction to the first pass (same day)

The first pass counted 0x00453550 as landable because all of its *calls* and all of its code immediates resolved. It is not
landable: its last instruction is `jmp 0x00453600`, a tail jump into a routine that has no catalog record at all. A tail
target is a code-address dependency exactly like a call, so the rule has to include it.

With that correction the chain has **zero** natives that can be landed without a new transcription. Every one of the 133
missing entries needs at least one more native first, and the blockers are callback and handler bodies (for example
0x00453600) that are not themselves chain entries, so they never appear in a "walk the chain in order" work list.

The next measurement therefore has to be the dependency closure: repeatedly take the natives whose body's calls, code
immediates *and* tail targets are all transcribed, and land that frontier. That is the only ordering that shrinks the
closure, and it is what the following rounds do.

## Dependency closure, measured (round 217)

The frontier was recomputed against real function extents rather than by scanning to the first return: radare2's analysed
function list gives each entry's size, so a body is the instructions inside [entry, entry+size).

- Chain calls: 155 (154 distinct). Missing from the catalog: 133.
- Missing natives reachable from the chain by calls, jumps, tail jumps and code immediates: **1708**.
- Of those, **180** are ready now (every dependency already transcribed), and that frontier is where landing has to start.

A caution discovered while selecting the batch: some call targets are *inner entries* of an existing function rather than
distinct routines (for example 0x004A892E and 0x004A8C89 reference -0x90(%ebp) without setting ebp, so they are mid-function
code that the decoder splits out). Transcribing those as separate natives would duplicate code that already exists inside
another body. Each frontier address therefore has to be checked for a real prologue and a boundary before it is landed, and
four such inner entries were rejected during this round's selection for that reason.

Natives landed from the frontier this round: 0x004AD36E, 0x004C0230, 0x004C0240, 0x004F1C90 - all measured 1 block / 0
edges at score 1.000.

## CRT-range natives in the closure (round 220 finding)

Some readiness blockers are not game code and must not be transcribed as natives. 0x004A54A0, ranked second in the closure by
in-degree, is the MSVC stack-probe helper: it compares with 0x1000, walks the stack down page by page touching each guard page,
then restores esp from eax and finishes with `push eax; ret` to jump to the real return address. That is compiler plumbing with
no C++ body that preserves it, and the standing rule for this work is that pre-WinMain CRT/compiler territory is outside the
reconstruction boundary (retail WinMain at 0x0048F530 is the boundary).

Consequence for the work order: the closure must be classified, not only measured. A closure member is either (a) a game native to
transcribe, (b) a CRT/runtime helper that the compiler supplies in the candidate, or (c) an inner entry of an already transcribed
function. Only (a) counts against readiness; listing 0x004A54A0 as "ready" was noise, and treating it as a required transcription
would have been a fabricated native.

## Frontier exhaustion (round 225)

After the getter and small-native waves, the ready set has run dry of landable leaves. Filtering the remaining ready
natives for bodies that touch no globals, make no call and no tail jump leaves exactly one member - 0x004AC8CC, which is a
three-stos 12-byte zero fill, i.e. compiler-generated struct clearing, not game code.

Every other ready native now falls into one of two classes:

1. it reads or writes a static object whose extent and owner are not yet established (0x5BDDB0, 0x5C10F0, 0x5C8A4C and
   family), so landing it would require inventing storage; or
2. it is a CRT/compiler helper (stack probe, 12-byte clear, string primitives) that the candidate toolchain supplies and
   that must not be transcribed as a native.

That is the real conclusion of the leaf-native strategy: it cannot continue without the extent-discovery work first. The
next increment is therefore data ownership, not another transcription: enumerate every reference to one shared object,
prove its extent from the widest access, record writers and readers, and only then land the natives that touch it.

## Extent discovery started: 0x005BDDB0 (round 226)

Method: an address-literal reference is only the tip. 0x005BDDB0 has exactly one direct reference in the image, the materialisation
at 0x0046B4D0 (`mov $0x005BDDB0,%eax`), so the width cannot be read off a load. The writer in the following routine is what proves
it: 0x0046B4E9 stores a four-byte value at `0x38(%eax)` where eax comes from the address helper 0x0046B040, so the block is at
least 0x3C bytes. That lower bound is what the object is declared with - not a guessed size and not a claim about the whole object.

0x0046B4D0 itself is landable now: 6 bytes, returns the block address, no memory access, CFG 1.000, retail observed 256/256 entries
and returns in a level-1 startup window. It is the first native landed under the extent-discovery approach rather than under the
leaf-native one.

Still open for the same block: the routines that call 0x0046B040 and then touch other offsets; those need the same treatment before
anything larger than a 0x3C-byte object can be declared. 0x005C10F0 and 0x005C8A4C remain undiscovered - the latter is filled by a
`rep movsl` at 0x004869A6 whose count is set outside the visible window, so its extent needs the caller's copy length, not the
address literal.
