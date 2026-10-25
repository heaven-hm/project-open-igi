# Static-initializer audit: tree definitions vs the image

## The defect class

A global in file-backed `.data` has its **initial value in the image bytes**. A port definition
written `std::uint16_t g_54e720;` initialises it to **0**, while retail loads **`0x181`** from the
file. Any getter that returns the slot then returns a different value than retail, before anything
writes it.

This was found by chain-worker on `native_4f1e00/4f1e10/4f1e20.cpp` (image `0xFFFFFFFF`, port `0`),
and then generalised by a scan.

## The scan

Read every `g_<VA>` definition in `engine/src/retail/**/*.cpp`, take the declared type's **width**,
read that many bytes from the image at the VA, and compare with the C++ initialiser (`0` when
absent). Restricted to file-backed `.data` (`0x00500000 <= VA < 0x00550000`); BSS is excluded
because `RetailImage.va_to_off()` returns `None` there and a BSS read is not a static initialiser.

**A first version of this scan read 8 bytes regardless of type and reported 160 disagreements.**
Almost all were false: a `uint16_t` holding `0x181` compared against `0x18101810181` is the scan
reading adjacent data. Reading the declared width leaves **66 real sites**. The count was wrong by
2.4x in the direction that looks like diligence.

## The result

**66 sites across 64 files**, all of the same shape:

| type | port had | image holds |
|---|---|---|
| `uint16_t` | `0` | `0x181` (385) |
| `uint8_t` / `uint32_t` | `0` | `0xFF` |
| `int32_t` (`texture_loaders.cpp`) | `0`, `1` | `0xFFFFFFFF` |

Corrected to the image value at every site, with `U` suffix. Re-scan reports **0 remaining**.

## What this does and does not establish

- **Establishes:** the port's static initial value now matches the image at all 66 sites.
- **Does not establish:** that any of them was **observably** wrong at runtime. A slot read only
  after being written produces the same behaviour either way. Observability is **per site** and has
  not been measured. `0x181` and `0xFF` are sentinel values, so the sites where a getter can run
  before its setter are the ones that matter, and those need runtime evidence.

This is a **behavioural** class, not a link class: every one of these files linked cleanly while
carrying the wrong initial value.

## Rule

**A file-backed global's initialiser is the image's bytes at its VA, read at the declared type's
width.** Do not leave it implicit. And when a scan reports a disagreement count, check the width it
read before believing the number.
