# Data-slot ownership: private copies of shared retail storage (2026-10-25)

Status: one confirmed behavioural divergence in accepted code, being fixed. The rule below is the
generalisation.

## The confirmed case

Retail `0x00413DF0` allocates seven event ids and stores them into seven consecutive bytes at
`0x005391AC..0x005391B2`:

```
0x00413DF9   mov byte [0x5391ac], al      ; ...and the same at +1..+6
```

Seven getters read those bytes back, one byte each, verified in the image:

| native | reads | owns in the tree before the fix |
| --- | --- | --- |
| `0x00413B20` | `0x005391AE` | private `g_5391ae = 0xFF` |
| `0x00413B30` | `0x005391AF` | private `g_5391af = 0xFF` |
| `0x00413B40` | `0x005391B0` | private `g_5391b0 = 0xFF` |
| `0x00413B50` | `0x005391B1` | reads `g_5391ac[5]` (correct, new) |
| `0x00413B60` | `0x005391B2` | reads `g_5391ac[6]` (correct, new) |

The first three were accepted earlier, each declaring its own byte initialised to the image value
`0xFF`. Because the registrar writes `g_5391ac[2..4]` and never those private objects, those three
natives **return `0xFF` forever instead of the event id the registrar stores**. That is a
behavioural divergence in live code, not a style difference: statically each file looks right, and
only the fact that two translation units modelled one retail address shows up when both are linked
and run.

## The rule

A retail address - code or data - has exactly one owner in the tree. A native that reads a slot it
does not own declares it `extern` with the owner's exact type and reads it by name. A private copy
initialised to the image value is only correct when the image value is genuinely the whole story,
which it never is for a slot that retail writes elsewhere.

This is the same failure as the duplicate `0x004AF8F0` implementation, one level down: there it was
two implementations of one function, here it is two models of one variable. Both are invisible to a
symbol-collision check, because `g_5391ae` and `g_5391ac[2]` are different symbols naming the same
byte.

## What to check, mechanically

For every `.data`/`.bss` object a native declares, search the retail image for other writers of that
address. If a writer exists in another native, the object must have one owner and the readers must
reference it. The candidate that declares the private copy compiles cleanly and passes structural
checks; nothing but this search catches it.
