# Guard-callee dependency chains, measured 2026-10-25

The startup guard `0x00402930` is 402 instructions / 314 call sites / **308 distinct
direct targets**, of which **179 have an implementation file and 129 do not.**
This note records the chains for the smallest missing ones, so the next writer starts
from measurements rather than from a `pd` window.

**Method warning that applies to every entry below.** Extents here are bounded by the
`ret` and the alignment nops after it, cross-checked against the byte stream - never by
"the first `ret` in a `pd` dump" and never by a dump length chosen for convenience. I
reported the guard itself as 156 instructions when it is 402 because I ran `pd 156` and
believed the window. **A `pd` bound is not an extent.**

---

## `0x00407D60` - "material defines" - 31 instructions, `ret` at 0x00407DD4

Extent `0x00407D60`-`0x00407DD4`; 11 nops `0x00407DD5`-`0x00407DDF`; the next routine
starts at `0x00407DE0` (which is also handler #1 below).

**Body, all 31 instructions read:**
1. Table clear: `mov eax,0x569474` / loop `mov byte [eax],0; add eax,0x268;
   cmp eax,0x56E174; jl` - clears the `0x569474` table, stride `0x268`, to `0x56E174`.
   **`0x569474` is already owned by `render/native_00407d60.cpp`** - check whether that
   file is this routine under a different name before writing a second definition.
2. Three `call 0x004017C0` with `push 0`, each storing `al` into `0x538518`,
   `0x538519`, `0x53851A`. **`0x538518` is owned by `core/native_408340.cpp` and
   `0x538519` by `core/native_438390.cpp`; `0x53851A` is UNOWNED.**
3. Three name registrations through `0x004B8890`, each `push -1; push 0;
   push <handler>; push <name>`:
   `"SetActiveMaterial"` (0x538544) -> 0x00407DE0
   `"DefineQMaterial"` (0x538534) -> 0x00407E30
   `"DefineGameMaterial"` (0x538520) -> 0x00407F50
4. `add esp, 0x3c` (15 pushes x 4) and `ret`.

**BLOCKED on five untranscribed files:** `0x004017C0`, `0x004B8890`, `0x00407DE0`,
`0x00407E30`, `0x00407F50`. **Do not add it to the build until they exist.**

---

## `0x004B8890` - shared name-registration forwarder - 14 instructions, `ret` at 0x004B88B2

Extent `0x004B8890`-`0x004B88B2`; 13 nops `0x004B88B3`-`0x004B88BF`; the next routine
`0x004B88C0` is a sibling with a different constant set.

**Referenced by 5 transcribed files.** It is a seven-argument forwarder:
`mov eax,[esp+0x10] / mov ecx,[esp+0xc] / mov edx,[esp+8] / push 0 / push 0 / push 1 /
push eax / mov eax,[esp+0x14] / push ecx / push edx / push eax / call 0x004BAAC0 /
add esp,0x1c / ret`. Seven pushes = `0x1c` bytes, consistent.
**Blocked on `0x004BAAC0`.** Note the second `mov eax,[esp+0x14]` is re-read *after*
four pushes, so it addresses the original `[esp+0x18]` - the caller's 7th argument.
That re-read is the kind of detail a rewrite silently "fixes".

---

## `0x004017C0` - event allocation - blocked, and it has a fatal path

`mov eax,0x20` / `mov ecx,0x54F9F2` / loop: `cmp byte [ecx],0` / `je 0x004017EA` /
`add ecx,6` / `inc eax` / `cmp ecx,0x54FF26` / `jl`. On exhaustion it pushes
`str.Unable_to_allocate_event` (`0x005361A8`, `"Unable to allocate event"`) and calls
`0x004AF7B0`, then `jmp 0x004017E8` - **an infinite self-jump, i.e. a fatal hang, not a
return.** Preserve it; do not "fix" it into a return or an error code.
**Blocked on `0x004AF7B0`, which is itself one of the guard's 129 missing callees.**

---

## `0x004BAAC0` - the allocation worker - 36 instructions, `ret` at line 36 of its extent

Blocks `0x004B8890`, the shared forwarder referenced by five transcribed files.

**Body, head read (36 instructions to the `ret`):**
```
push esi / push edi
push 1 / call 0x004B0D60
push 4 / push 0x1c / call 0x004B0C60      ; allocate 0x1c = 28 bytes
mov esi,eax
mov ecx,7 / xor eax,eax / mov edi,esi / rep stosd   ; zero 7 dwords = 28 bytes
mov eax,[esp+0x18]
mov ecx,dword [0xA43E88]
push esi / push eax / push ecx
call 0x004C0560
mov edx,[esp+0x28] / mov ecx,[esp+0x30]
mov [esi+0x14],eax / mov eax,[esp+0x2c] / mov [esi+0x10],edx / mov edx,[esp+0x34]
...
```
**The `rep stosd` count 7 and the allocation size `0x1c` agree exactly** (7 x 4 = 28 =
0x1c), which is a useful cross-check that the struct is 28 bytes and that the zeroing
covers all of it. A port that allocates 0x1c but zeroes 6 dwords leaves 4 bytes of
allocator garbage in a struct that is then filled from arguments - and nothing would
catch it.

**Blocked on `0x004B0D60`, `0x004B0C60`, `0x004C0560`** (transcribed status recorded in
the run that produced this note) **and on the global `0xA43E88`,** which is read before
the `0x004C0560` call.

**Note the argument offsets shift across the body** (`[esp+0x18]`, then `[esp+0x28]`,
`[esp+0x2c]`, `[esp+0x30]`, `[esp+0x34]` after three pushes). **Read each offset against
the push state at that instruction**, not against the function entry - the same trap as
`0x004B8890`'s re-read.
