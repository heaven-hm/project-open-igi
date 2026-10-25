# Owner ruling: adopt IGI-RE signatures and bodies (recorded 2026-10-25)

**Owner directive, verbatim:** *"use their signaure and code also exactly but just convert each one into
c++ equivalent"*

## What this changes

`https://github.com/slickdaddyy/IGI-RE` is a decompilation of Project I.G.I. into readable C:
**5,376 of 5,376 functions rewritten**, every one with a real name, every global a typed C
variable at its original address, and **90,078 differential tests in 425 files**.

Their README states the standard they worked to: *"It is a **functional** decompilation: a rewrite
does not have to compile to the same bytes, it has to behave exactly like the original: same
return value, same memory writes, same calls in the same order."*

The owner has directed that their signatures **and bodies** be used, converted to C++ equivalents.
**This is a formal change to the body-provenance standard.** §10.13.1 required every body to be
traceable to retail *instructions*; that requirement is superseded for bodies sourced this way, and
the provenance block must say so plainly rather than implying an instruction-level transcription.

**What does NOT change:** one native per VA, one file per native, the five-line provenance block,
domain folders, byte-identical strings, no invented helpers, no dead code, testing outside
`engine/`, and the status ladder. A body that came from IGI-RE is still not `RUNTIME_TESTED`.

## The join (measured, not assumed)

| source | provides | count |
|---|---|---|
| `symbols/names.tsv` | VA -> name | 7068 |
| IGI-RE `src/**/*.c` | name -> C body | 5977 definitions |
| IGI-RE `ORIG(va, ret, name, args)` | VA -> return type + name + **full argument list** | 8305 entries / 2206 VAs |
| IGI-RE `src/addr.c` `AT(name, addr)` | global -> original address | 2130 |

Verified on the live frontier: **64 of 64** frontier symbols resolve to a VA + name + body.
The 11 apparent name conflicts in the ORIG map are trailing-underscore weak-symbol variants
(`List_AddTail` / `List_AddTail_`), not disagreements.

## Provenance block for adopted bodies (required form)

```
// Native: <name>; retail VA 0x...
// Artifact: retail igi.exe SHA-256 <hash>; image base 0x00400000.
// Source: body and signature adopted from IGI-RE <commit> <path>; converted C -> C++.
//         IGI-RE is a functional decompilation - equivalent behaviour, NOT byte-identical code.
// Behavior/CFG: <what the body does; retail instruction-level CFG where measured>.
// Status/limits: TRANSCRIBED; BUILD_VERIFIED; STRUCTURALLY_VERIFIED against <what>;
//         runtime UNOBSERVED. <limits>
```

## Honesty requirement

An adopted body is a **second-hand** reconstruction. Its correctness rests on IGI-RE's 90,078
differential tests, not on our own instruction-level reading of `igi.exe`. Every record must say
that. Do not let "it compiles and matches IGI-RE" read as "verified against retail".
