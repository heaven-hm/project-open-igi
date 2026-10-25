# Recon: retail 0x0042ACC0 (guard 0x00402930 callee #124, call site 0x00402B97)

Status: RECON ONLY. No implementation file written. This note is the durable
record so the work survives context loss; it is not a transcription.

## Extent and structure
- VA 0x0042ACC0 - 0x0042B025 = **870 bytes**. Verified two ways: `afi @ 0x0042ACC0`
  reports `size 870`, and `0x0042ACC0 + 870 = 0x0042B026`, i.e. the `ret` at
  0x0042B025 is the final byte of the routine.
- Entry instruction: `sub esp, 0x8e0`. Frame 0x8E0.
- CFG: **1 block, 0 edges** (`afi`: edges 0). No branch of any kind in the body.
- 246 instructions.

## Trap found during recon, recorded so it is not repeated
`pd 260 @ 0x0042ACC0` prints **two** `ret` instructions, at 0x0042B025 (line 246)
and 0x0042B03C (line 260). Only the first is this routine's. The second belongs
to the following function. Bounding a routine by "the first ret in a pd dump" is
correct here only because the dump was bounded; an unbounded `pd` walks into the
next function and will silently absorb it. The extent must be checked against
`afi`'s size as well.

## Call census (decoded from `call` instructions, not from immediates)
| target | sites |
|---|---|
| 0x00401400 | 15 |
| 0x004F1A80 | 10 |
| 0x004F0F50 | 6 |
| 0x004ECDB0 | 2 |
| 0x0042A2C0 | 2 |
| 0x00401530 | 2 |
| 0x004FB700 0x004F4820 0x004F4800 0x004F4660 0x004F4600 0x004F45F0 0x004F4140 0x004F4130 0x004F4120 0x004F4110 0x004F40E0 0x004F40D0 0x004F1A70 0x004F1A60 0x004F1950 0x004F1000 0x004F0FF0 0x004F0FC0 0x004F0FA0 0x004E6610 0x004CEA20 0x004CEA10 0x004C5800 0x00416890 0x00401900 | 1 each |

Distinct targets: 30. `0x0042A2C0` is a **call inside the same band as this
routine** and is itself untranscribed as far as this recon checked.

## Observed head of the body (first instructions, verbatim)
```
0x0042acc0  sub esp, 0x8e0
0x0042acc6  push 0
0x0042acc8  push 0
0x0042acca  call 0x4f45f0
0x0042accf  push eax
0x0042acd0  push 0x8e0
0x0042acd5  push str.Train            ; 0x53bd1c ; "Train"
0x0042acda  call 0x401900             ; TaskType_Register
0x0042acdf  push 0x429f80             ; handler, untranscribed
0x0042ace4  push 0
0x0042ace6  push eax
0x0042ace7  mov word [0x57bcb8], ax    ; id word
0x0042aced  call 0x401400
0x0042acf2  mov ax, word [0x57bcb8]
```
So this is a registrar for class **"Train"** (string 0x0053BD1C), object size
0x8E0 (equal to the frame), id word at 0x0057BCB8 (16-bit), first handler
0x00429F80 untranscribed, linked through 0x00401400.

## Open items before transcription
1. `0x0057BCB8` ownership must be checked against the tree BEFORE writing
   (file-7 lesson: a second definition links silently and the getter reads an
   empty copy). Not yet checked.
2. `0x0042A2C0` (2 call sites) and `0x00429F80` are untranscribed; handlers go in
   as cast integers per the standing ruling.
3. The body has 10 `0x004F1A80` appends and 15 `0x00401400` links; the append
   argument order and any pre-pushed arg6 must be derived per site, not assumed
   from the sibling registrars (file-8 lesson: adjacent-byte offsets and a
   reversed lea ordering appear in this band).
4. `build/recon/42acc0.dis` holds the full 246-instruction listing.

## Why no implementation file yet
The routine is 246 instructions and 870 bytes. Writing it requires reading the
whole body; a partial transcription is worse than none, and the standing rule is
to stop when the middle of a body is unread rather than guess. Recon is captured
so the next writer starts from evidence rather than from scratch.

## Slot census for the blocked teardown 0x0042BC00 (measured 2026-10-25)

`0x0042ACC0` is the **"Train"** registrar (`push str.Train` = 0x0053BD1C, size 0x8E0
at 0x0042ACD0, parent getter `0x004F45F0`, handler `0x00429F80`, class entry
`0x0042A100`). It **does** own both globals the teardown `0x0042BC00` reads, so
analyst's expectation was correct on both:

| slot | width | evidence | hits in body |
|---|---|---|---|
| `0x0057BCB8` | **uint16_t** | written once at `0x0042ACE7` `mov word [0x57bcb8],ax`; read 16-bit (`66`-prefixed) throughout | 16 |
| `0x0057BCBC` | **uint32_t** | `dword` accesses | 11 |

And `0x0042BC00` reads exactly those two:
```
0x0042bc00  66a1b8bc5700   mov ax, word [0x57bcb8]
0x0042bc06  50             push eax
0x0042bc07  e8145efdff     call 0x00401A20   ; TaskType_Clear
0x0042bc0c  8b0dbcbc5700   mov ecx, dword [0x57bcbc]
0x0042bc12  51             push ecx
0x0042bc13  e8185c0900     call 0x004C1830   ; QTaskList_Delete
0x0042bc18  83c408         add esp, 8
0x0042bc1b  c3             ret
```
**No `0x0181` reset** - and `0x0042BC00` is above `0x00424790`, so analyst's Law 2
(no reset above MenuWindow) predicts exactly this. Second independent confirmation.

**LINKAGE DECISION, so analyst can declare them now:** when `native_42acc0.cpp` lands
it will define `g_57bcb8` (`std::uint16_t`) and `g_57bcbc` (`std::uint32_t`) with
**C linkage**, matching the ten-of-eleven owners in the band. Declare them
`extern "C"`.

### A correction I made to myself while measuring this
My first check for `0x57BCBC` piped a grep through `head -12` and printed only
`57BCB8` lines; I read that truncated output as "0x57BCBC does not appear in this
body" and stated it as a contradiction of analyst's expectation. It appears 11 times.
**A truncated output read as a complete one is the same failure as a count of zero
from an unverified pattern** - the sixth instrument fault of this shape this session.

## COMPLETE STRUCTURAL ANALYSIS (measured 2026-10-25, transcription NOT yet written)

### Identity
Class **"Train"**, string `0x0053BD1C`, class size `0x8E0` (`push 0x8E0` at 0x0042ACD0),
parent getter `0x004F45F0` (called at 0x0042ACCA), class-alloc `0x00401900` at
0x0042ACDA. Extent 0x0042ACC0-0x0042B025 = 870 bytes, 246 instructions, 1 block,
0 edges, frame `0x8E0`, final cleanup `add esp, 0x8f8` at 0x0042B01F, `ret` at
0x0042B025. No stack probe.

### Call census (decoded from `call` instructions, not from immediates)
| target | sites | role |
|---|---|---|
| `0x00401400` | **15** | the handler-registration call |
| `0x004F1A80` | 10 | event registration |
| `0x004F0F50` | 6 | event registration |
| `0x00401530` | 2 | second registration call |
| `0x004F45F0` | 1 | parent class getter |
| `0x004F4140 0x004F4130 0x004F4120 0x004F4110 0x004F40E0 0x004F40D0` | 1 each | |
| `0x004F1A70 0x004F1A60 0x004F1950` | 1 each | |
| `0x004F1000 0x004F0FF0 0x004F0FC0 0x004F0FA0` | 1 each | |
| `0x004E6610 0x004CEA20 0x004CEA10 0x004C5800` | 1 each | |
| `0x00416890` | 1 | |
| `0x00401900` | 1 | class allocation |
**24 distinct targets over 53 call sites** - counted, not copied. `0x0042A2C0` is NOT
among them: 0 hits in the decoded disassembly.

**TWO CORRECTIONS, both mine, both in the recon note above.**
1. The earlier section of this note says "Distinct targets: 30". The decoded census
   gives **24**. The 30 was written from a glance, not a count.
2. That same section says `0x0042A2C0` "is a call inside the same band as this routine
   and is itself untranscribed". **It is not called by this routine at all** - 0 hits.
   I carried an unverified line forward into this analysis, and the only reason it did
   not become a phantom dependency is that the second pass counted.

Both are the same failure: a number and a name written from the shape of the earlier
note rather than measured from the bytes. The first pass was the guess; the second pass
was the count.

### The 17 handler immediates pushed (`push 0x4xxxxx`)
```
0x429F80 0x42A100 0x42A150 0x42AC20 0x42B030 0x42B040 0x42B220 0x42B240
0x42B280 0x42B2C0 0x42B2F0 0x42B500 0x42B700 0x42B930 0x42BB60 0x42BBA0
0x4EFF20
```
15 go to `0x00401400`, 2 to `0x00401530` (`0x42A100` at 0x0042ACF8 with `push 2`, and
`0x42B500` at 0x0042AD41 with `push 7`).

### Two globals this file MUST define (analyst's 0x0042BC00 declares them extern)
- `0x0057BCB8` **`std::uint16_t`** - written ONCE at `0x0042ACE7`
  `mov word [0x57bcb8],ax`, then read 16-bit throughout (17 accesses).
- `0x0057BCBC` **`std::uint32_t`** - `mov dword [0x57bcbc],eax` at `0x0042AE91`
  (11 accesses).
**LINKAGE: C linkage** (matches ten of eleven owners in the band). Analyst declares
both `extern "C"`.

### The body's shape
The routine reads `g_57bcb8` into ax/cx/dx repeatedly and passes it as the FIRST
argument of each registration: `push <handler>; push <literal>; push <id>` then
`call 0x00401400`. The pattern repeats 15 times with different handlers and literals.
**It is highly patterned but the literals and handler order must each be read from
retail, not copied from a sibling** - the band has no canonical append shape (four
distinct `lea` orderings were measured across the other registrars).

### Why this note exists rather than a file
The transcription is 246 instructions. Writing it at the end of a context budget,
from a pattern rather than from the instructions one at a time, is precisely how an
invented constant gets in - and this file's whole purpose is to define two globals
another native depends on. The analysis above is complete and durable; the
transcription needs a fresh budget.
