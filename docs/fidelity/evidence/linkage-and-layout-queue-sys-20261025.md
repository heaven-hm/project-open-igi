# Linkage conventions in `engine/src/retail` (measured 2026-10-25)

Status: measurement, with a correction to an earlier claim in this same file. The correction is
the important part.

## Correction first

An earlier version of this note said four files "violate" the C-linkage convention and implied the
goal was zero mangled natives. That was wrong, and measuring properly showed why:

```
grep -rln "^namespace igi::retail" engine/src/retail --include=*.cpp | wc -l   ->  80
```

**Eighty** files under `engine/src/retail` open `namespace igi::retail`, and most are in the
build. C++-mangled linkage is not a four-file defect here; it is the majority convention for
non-`native_*` files. Only the `native_*.cpp` files consistently use `extern "C"`/`NATIVE_DECL`.

So "normalise everything to C linkage" would be an eighty-file refactor, which is neither
proportionate nor required. That claim is withdrawn.

## The rule that actually matters

Definition and declaration must agree. That is what caused the real incident today:

- `native_4f1cd0.cpp` defined `Sys_InitGlobalListHead_4F1CD0` inside the namespace, exporting
  `__ZN3igi6retail3sys29Sys_InitGlobalListHead_4F1CD0EPv`. `chain-worker` resolved that mangled
  name correctly; `handler-abi-worker` searched for a C name, did not find one, and concluded the
  VA was unimplemented - and nearly wrote a second implementation of it.
- Nothing was broken by the mangling itself. What was broken was that the *same VA* looked
  implemented to one agent and missing to another.

## Convention adopted from here

| file kind | linkage | caller form |
| --- | --- | --- |
| `native_*.cpp` (one native per file, the form new work uses) | `extern "C"` / `NATIVE_DECL` | unqualified name |
| container files that group several routines (the existing majority) | `namespace igi::retail::<domain>` as they are | qualified name |

This keeps the manifest's `_`-prefixed `nm` form valid for every native file without touching
eighty files, and it makes "is this VA implemented?" answerable by looking for the C name in the
`native_*` set.

## Consequences applied

- `native_4f1cd0.cpp`, `native_4f1d60.cpp`: normalised to `extern "C"`, verified by me.
- `native_4af8f0.cpp`: normalised; its **two callers must be updated in the same change** or they
  reference a symbol that no longer exists. Measured right now:
  `core/native_4f90b0.cpp` and `core/native_4f9df0.cpp` still emit
  `U __ZN3igi6retail3sys25Sys_InsertListTail_4AF8F0EPvS2_`, while the definition exports
  `_Sys_InsertListTail_4AF8F0`. This is the one open inconsistency in the tree and it is being
  closed; nothing links until it is.
- `native_4af960.cpp` (`0x004AF960`): a `native_*` file with **seven** callers in container files
  across `core/`, `script/`, `terrain/`, `resource/` and `mem/`. Queued as one deliberate change,
  because it spans subsystems and the current state is internally consistent.
- `ilff_navigation.cpp` and `resource_lookup.cpp`: container files under the adopted convention;
  they keep their namespace. Two separate structural questions stay recorded:
  `resource_lookup.cpp` holds three retail natives (`0x004AD73D`, `0x004B5AF0`, `0x004B5B30`) in
  one file, and `ilff_navigation.cpp` defines an anonymous-namespace helper `read_i32` while
  claiming no retail VA - either a transcription needing VAs and provenance, or port-only helper
  code that does not belong in `engine/`.

## The check to use

`nm --defined-only` on the compiled object, or `nm -u` on the caller. A name search finds
declarations; only the object shows what a file actually exports and references.
