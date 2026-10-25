# The PE section table, and why an address alone cannot classify a global

Recorded 2026-10-25 after chain-worker reported that `0x0053C1DC` "looks like
`.rdata` but is not". Confirmed here by parsing the section table directly, and the
finding is larger than the original report.

## The table

Parsed from the retail image's PE header (image base `0x00400000`, 4 sections):

| section | VA | VirtualSize | virtual end | raw size |
|---|---|---|---|---|
| `.text`  | `0x00401000` | `0x131510` | `0x00532510` | `0x132000` |
| `.rdata` | `0x00533000` | `0x0028F6` | `0x005358F6` | `0x003000` |
| `.data`  | `0x00536000` | `0x6FC558` | `0x00C32558` | `0x01A000` |
| `.rsrc`  | `0x00C33000` | `0x001E40` | `0x00C34E40` | `0x002000` |

## Two traps

**`VirtualSize`, not raw size, bounds a section.** `.rdata` is only `0x28F6` bytes
of virtual space ending at `0x005358F6`, but its raw size is `0x3000`, so
file-aligned tooling reports it as ending at `0x00536000`. Radare2's `iS` prints the
file-aligned size. Any classification that trusts that output treats the
`0x005358F6`-`0x00536000` gap as readable `.rdata` when it is not mapped at all.

**There is no `.bss` section in this image.** The four sections are `.text`,
`.rdata`, `.data`, `.rsrc`. `.data` has virtual size `0x6FC558` against a raw size
of `0x01A000`, so its tail is zero-filled by the loader. A zero-initialised global
is therefore `.data` with a zero initialiser, and `B` versus `D` is a
symbol-classification convention, not a fact about which section holds the object.

## Consequences for the rules in use

The storage-class rule stays as stated -- **the image's initial value fixes the
initialiser** -- but "the image value is zero, so it is `.bss`" should read "so it
is `.data` with a zero initialiser". Nothing about the port's behaviour changes;
what changes is that a wrong sentence was being written into provenance blocks.

The address-range rule is the one that was actually wrong. **Most of the
`0x0053xxxx` range is `.data`**, not `.rdata`:

- `0x0053C1DC` -> `.data` (chain-worker's case; image value `0x0181`)
- `0x00536144` -> `.data` (the `"IGI"` title string passed to `SetAppTitle`)
- `0x005383D8`, `0x005383DA` -> `.data`
- `0x005690B0`, `0x005690B8`, `0x005690C8` -> `.data`
- `0x0057BD38` -> `.data`

`.rdata` covers only `0x00533000`-`0x005358F6`. Anything at or above `0x00536000`
is `.data`, and anything between `0x005358F6` and `0x00536000` is unmapped padding.

## What this cost, and what it caught

chain-worker found it while classifying `0x0053C1DC` for `Task_DefinePlaceExplosiveTBA`
and recorded both the section derivation and the image value, on the grounds that
either alone would have been enough to get it wrong. That is the right instinct: the
section decides writability, the image value decides the initialiser, and they are
independent facts.

Following it into the three byte getters at `0x00408320`/`0x00408330`/`0x00408340`
exposed a real fidelity bug that had nothing to do with sections.
`render/native_00407d60.cpp` owns `g_538518`, `g_538519` and `g_53851a` as their
writer, and all three were declared `= 0U`. **Their image values are all `0xFF`.**
The initialiser decides the value a reader sees before the writer runs, so `0` versus
`0xFF` is observable: a byte getter read before `0x00407D60` executes returned 0 in
the port and 0xFF in retail. All three are now `0xFFU`.
