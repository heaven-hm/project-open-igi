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
