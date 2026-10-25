# `build/retail-dis.txt` desync: 23 windows, 424 bytes

Recorded 2026-10-25. `build/retail-dis.txt` is a GNU objdump linear disassembly of retail `igi.exe`. After an indirect jump through an inline jump
table it keeps decoding straight into the table's data bytes and only realigns when the linear stream happens to resynchronise. Any native whose body
covers one of these ranges must be transcribed from radare2, never from the listing.

## Method and its limits

- 406,645 listing instructions parsed; 124 `jmp *0xTABLE(,%reg,4)` sites found; the 142 functions enclosing them analysed with `r2 af` + `pdfj`.
- A window is a maximal run of listing instructions whose address is **strictly inside** `[entry, entry+size)` of an r2-recovered function but which r2
  does not report at that address. An earlier pass attributed up to 0x400 bytes past each function's end to that function and produced 455 windows /
  81,653 bytes; that was the pass's bug and the number was retracted.
- Ground truth exists only inside those 142 table-dispatch functions. Outside them this method does not contradict the listing - it simply does not
  measure it. "Not in the list" means unmeasured, not clean.

## The windows

| Range | Bytes |
| --- | --- |
| `0x00482881-0x00482882` | 1 |
| `0x0048B6E5-0x0048B6E7` | 2 |
| `0x0048FFA1-0x0048FFA6` | 5 |
| `0x00496251-0x00496254` | 3 |
| `0x00496AB1-0x00496AB4` | 3 |
| `0x004A7E63-0x004A7E64` | 1 |
| `0x004AA80B-0x004AA80C` | 1 |
| `0x004AA813-0x004AA8B0` | 157 |
| `0x004AA8F6-0x004AA90D` | 23 |
| `0x004AA90F-0x004AA910` | 1 |
| `0x004AA91B-0x004AA91C` | 1 |
| `0x004ACDCB-0x004ACDCC` | 1 |
| `0x004ACDD3-0x004ACE78` | 165 |
| `0x004ACEB6-0x004ACECD` | 23 |
| `0x004ACECF-0x004ACED0` | 1 |
| `0x004ACEDB-0x004ACEDC` | 1 |
| `0x004B5D85-0x004B5D8B` | 6 |
| `0x004BA341-0x004BA344` | 3 |
| `0x004BA383-0x004BA38C` | 9 |
| `0x004BC771-0x004BC774` | 3 |
| `0x004C5902-0x004C5905` | 3 |
| `0x004F9DF2-0x004F9DFB` | 9 |
| `0x00517512-0x00517514` | 2 |

## Mechanism, confirmed on the largest window

At `0x004AA80C` the listing contains `ff 24 8d 8c a8 4a 00` (`jmp *0x4aa88c(,%ecx,4)`) and then continues at `0x004AA813` inside the jump-table data:
`90 20 a8 4a 00 4c a8` decodes as `nop; and %ch,-0x57b3ffb6(%eax); dec %edx; add %dh,-0x58(%eax); ...` until realignment at `0x004AA8B0`. The paired
`0x004ACDD3-0x004ACE78` window has the same shape.

`0x004F9DF2-0x004F9DFB` is the sanity case: it is the registrar body that first forced this lane onto radare2.

## Consequences

- `0x004A54A0` (`__chkstk`) is called from eight sites that `r2 /r` finds and this listing renders not at all - the same defect class seen from the
  callee side. See `compiler-emitted-symbols.md`.
- Any transcription covering a window above needs its provenance to name radare2 as the source and to say why the listing was not used.
