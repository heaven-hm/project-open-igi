# QVM core dependency table (argument accessors → interpreter core)

Artifact: retail Project IGI `igi.exe`,
SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, image base `0x00400000`.
All sizes/blocks/edges/instruction counts below are radare2 6.2.0 `af`/`pd` measurements on that
image (`af @ <VA>; afij @ <VA>` per function), cross-checked against the linear disassembly
`build/retail-dis.txt` and the raw bytes. Static only: no build of these files, no runtime capture,
no manifest/CMake/database edits.

## 1. What landed in this pass

| Retail VA | File | ABI (established from the bytes) | r2 CFG |
| --- | --- | --- | --- |
| `0x004BD010` | `engine/src/retail/script/native_4bd010.cpp` | cdecl, `long double(void* argument_frame, int32_t index)`, real returned in ST0 via `flds` | 41 B, 1 block, 0 edges, 14 instrs |
| `0x004BCAB0` | `engine/src/retail/script/native_4bcab0.cpp` | cdecl, `int32_t(void* argument_frame, int32_t index)`, integer returned in EAX | 41 B, 1 block, 0 edges, 14 instrs |
| `0x004BD040` | `engine/src/retail/script/native_4bd040.cpp` | cdecl, `char*(void* argument_frame, int32_t index, void* destination, uint32_t count)`, destination returned in EAX | 38 B, 1 block, 0 edges, 16 instrs |

All three are single-block wrappers around the same untranscribed routine `0x004BCAE0`; they differ
only in the selector they push (1 = integer, 2 = real, 3 = string) and in how they consume the
4-byte stack slot that `0x004BCAE0` fills:

- `0x004BD010`: `push $0x2`, size 4, then `flds 0x14(%esp)` → `long double` in ST0. The retail never
  converts the single-precision slot: `flds` loads it into ST0, so the transcription keeps the
  `long double` return and the `static_cast<long double>(float)` is the `flds` itself.
- `0x004BCAB0`: `push $0x1`, size 4, then `mov 0x14(%esp),%eax` → `int32_t`.
- `0x004BD040`: `push $0x3`, size = caller's count, clears `destination[0]` (`movb $0x0,(%esi)` at
  `0x004BD057`) and returns the destination pointer (`mov %esi,%eax` at `0x004BD062`).

`0x004BCAE0` is declared with `NATIVE_DECL` in each of the three files (never defined there) with the
argument read order its own body proves: `[esp+4]` destination, `[esp+8]` size, `[esp+0xC]` frame,
`[esp+0x10]` index, `[esp+0x14]` selector. radare2 reports `nargs=4` for `0x004BCAE0`, which
undercounts: every selector path reads `[esp+0x14]` after the call cleanup, so it takes **five**
stack arguments.

## 2. `0x004BCAE0` itself (the next single blocker, not transcribed)

| VA | bytes | blocks | edges | instrs | callees (with engine status) |
| --- | --- | --- | --- | --- | --- |
| `0x004BCAE0` | 274 | 15 | 22 | 100 | `0x004BCFD0` Qvm_ExecuteInstructions (engine: `qvm_stack_natives.cpp`), `0x004AF7B0` Debug_Error (engine), `0x004BCC00` (missing), `0x004BCEC0` (missing), `0x004A5500` (missing) |

Structure recovered from the bytes (needed before it can be transcribed faithfully):

- saves the interpreter's current-node/limit fields, looks up `frame+0x10[ index ]` into
  `interpreter+0x14`, runs `Qvm_ExecuteInstructions`, then restores the saved fields;
- dispatches on the selector: 1 = integer branch (writes a dword through the destination pointer),
  2 = real branch (`fstps` into the destination), 3 = string branch (`0x004BCC00` produces the value,
  `0x004A5500` copies at most `size-1` bytes, then a NUL is stored at `destination[size-1]`);
- unknown selectors report the byte-identical retail string `"Unknown data type."` (`0x00547AB4`)
  through `Debug_Error` and then spin (`jmp $`), i.e. that path never returns;
- it passes **pointers into its own incoming argument words** (`[esp+0x0C]`, `[esp+0x10]`) to
  `0x004BCEC0` as scratch slots, so its callees' signatures have to be pinned before it can be
  transcribed without inventing an argument layout. That is the reason it is not in this pass.

## 3. The three shared QVM state words

The CMake comment (lines ~431-438) says "three shared QVM state words". The words are
**`0x00A965A0`, `0x00A96598`, `0x00A965A8`**; a fourth word `0x00A965A4` belongs to
`qvm_execute.cpp` itself (`g_a965a4`).

| VA | meaning (evidence) | retail writers / readers | current owner |
| --- | --- | --- | --- |
| `0x00A965A0` | global-state / stop-parsing gate written by `Qvm_SetGlobalState`; the source-block loop in `Qvm_Execute` tests it together with the abort flag | write `0x004B8A14` (Qvm_SetGlobalState `0x004B8A10`), read `0x004B8645` | `qvm_native_loader.cpp:96` (`g_a965a0`), STRUCTURALLY_VERIFIED |
| `0x00A96598` | "error reporting enabled" dword | write `0x004B8974` (3-instruction setter `0x004B8970`) and `0x004B89D4`; read `0x004B7E10` (Qvm_ReportError) and `0x004B898D` | `qvm_native_loader.cpp:29` (`g_a96598`, volatile) |
| `0x00A965A8` | "an error was reported" flag | write `0x004B7E1D` (Qvm_ReportError), `0x004B85C7`/`0x004B86BB` (Qvm_Execute), `0x004B9313` (**0x004B9120**), `0x004B987D` (**0x004B9660**), `0x004BBD83`; read `0x004B8699` (Qvm_Execute) and `0x004BF361` (Qvm_ParseBlock `0x004BF1F0`) | `qvm_native_loader.cpp:30` (`g_a965a8`, volatile) |
| `0x00A965A4` (not one of the three) | execution-abort flag | write `0x004B85C1`, `0x004B8602`, `0x004B86CE` (Qvm_Execute), `0x004B8C24` (BreakScript `0x004B8C10`); test `0x004B864D` | `qvm_execute.cpp:27` (`g_a965a4`) |

**Linkage defect (finding, not a transcription gap).** The three shared words *are* ported, but they
are defined inside the anonymous namespace of `engine/src/retail/script/qvm_native_loader.cpp`
(`namespace igi::retail::script { namespace { std::int32_t g_a965a0 = -1; ... } }`, lines 18-32), so
they have internal linkage. `engine/src/retail/script/qvm_execute.cpp` declares them at global scope
(`extern std::int32_t g_a965a0; extern volatile std::int32_t g_a96598; extern volatile ... g_a965a8;`)
and therefore cannot link against them even if every callee existed. The CMake rationale ("three
shared QVM state words are not ported yet") is inaccurate: the missing piece is external linkage
(or a shared header with `extern "C"` declarations), which is a one-site change in
`qvm_native_loader.cpp` that the integrator owns. This is why `0x004B9120` and `0x004B9660` cannot
be transcribed in isolation: both write `0x00A965A8` on their error paths.

## 4. The ten callees named in the CMake comment

The comment calls them "eight retail callees" but lists ten VA: eight belong to `Qvm_Execute`
(`0x004B85B0`), two to `Qvm_GetFloatArg` (`0x004B8A50`). Engine status is what matters for the link;
manifest status alone does not create an engine definition.

| VA | role per `qvm_execute.cpp` | bytes | blocks | edges | instrs | direct callees | leaf? |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x004BD080` | Qvm_RunProgram (precompiled-program runner, drives `0x004BCFD0`) | 346 | 20 | 17 | 107 | `0x004A53B3` sprintf, `0x004BC960` (307 B, missing), `0x004BCFD0` (engine), 1 indirect | no |
| `0x004C0200` | Qvm_SourceBufferInit (script-source buffer over the resource) | 42 | 1 | 0 | 13 | none | **yes** |
| `0x004BF1C0` | Qvm_SourceReaderInit | 29 | 1 | 0 | 10 | `0x004BEF60` (91 B, 1 block, 34 instrs, missing) | near-leaf |
| `0x004BFC70` | Qvm_ReadNextBlock | 1087 | 76 | 105 | 329 | `0x004AF960` (engine), `0x004B0D10` (engine), `0x004BF1E0` (engine), `0x004C0170` (engine), `0x004C01D0` (42 B, missing), `0x00517260`/`0x00517510`/`0x00517570` (manifest VERIFIED but **script candidates only**, no engine file), 1 indirect | no |
| `0x004BF1F0` | Qvm_ParseBlock | 409 | 23 | 28 | 152 | `0x004AF8F0` (engine), `0x004B0C60` (engine), `0x004B7E10` (engine), `0x004B9CD0` (**1612 B, 106 blocks, 540 instrs**, missing), `0x004BF390`, `0x004BF3A0`, `0x004BF580` (missing), `0x004BFC70`, `0x004BF8F0`; reads `0x00A965A8` | no |
| `0x004BFA80` | Qvm_ReleaseBlockTree | 319 | 30 | 41 | 130 | Debug_Error (engine), **`0x004B9120`** (the integer evaluator), self-recursive, 1 indirect | no |
| `0x004BF8F0` | Qvm_ReleaseBlock | 363 | 24 | 28 | 142 | Debug_Error, `0x004AF960`, `0x004B0D10` (both engine), `0x004B9BE0` (191 B, 19 blocks, 71 instrs, missing), self-recursive, 1 indirect | no |
| `0x004BEFC0` | Qvm_ReleaseSourceReader | 26 | 1 | 0 | 9 | `0x004B0D10` (engine) | near-leaf |
| `0x004BD010` | Qvm_GetFloatArg's real reader | 41 | 1 | 0 | 14 | `0x004BCAE0` | **done this pass** |
| `0x004B9660` | Qvm_GetFloatArg's expression-node evaluator | 1265 | 75 | 75 | 447 | see §5 | no |

Cheapest order for the eight Qvm_Execute callees: `0x004C0200` (leaf, 13 instrs) →
`0x004BEFC0` (9 instrs, one existing callee) → `0x004BF1C0` (needs only `0x004BEF60`, 34 instrs) →
`0x004BF8F0` (needs `0x004B9BE0`) → `0x004BF1F0` (needs `0x004B9CD0`, the largest single item in the
whole closure at 540 instructions) → `0x004BFA80` (waits for `0x004B9120`) → `0x004BF1C0`-dependent
`0x004BFC70` (needs `0x004C01D0`, `0x004BC960`, and the three terrain-named pointers) →
`0x004BD080` (needs `0x004BC960`, and `sprintf`'s engine definition should be confirmed).

## 5. The evaluator cluster (`0x004B9120`, `0x004B9660`, `0x004B8CE0`) — one wave, not three

Not transcribed in this pass. Measured facts that justify treating them together:

| VA | bytes | blocks | edges | instrs | r2 nargs | dispatch evidence |
| --- | --- | --- | --- | --- | --- | --- |
| `0x004B9120` Qvm_EvaluateIntegerNode | 1188 | 65 | 52 | 484 | 3 | node type tag = signed byte at `+0x01`, range 0..0x1D; jump table `0x004B95C4`, sub-dispatch table `0x004B963C` |
| `0x004B9660` Qvm_EvaluateArgumentValue | 1265 | 75 | 75 | 447 | 6 | same tag range; byte index table `0x004B9BA4`, jump table `0x004B9B54`, sub-dispatch `0x004B9BC4`; null node returns the double constant 0.0 at `0x005333B0` |
| `0x004B8CE0` Qvm_ResolveSymbolBlock | 778 | 41 | 47 | 262 | 9 | tag range 0..0x19; byte index table `0x004B9000`, jump table `0x004B8FEC`, second-level table `0x004B901C`; null node stores the 2-byte retail constant `0x0020` at `0x005474D8` ("space, NUL") and returns 0 |

Why they are one wave:

1. They call each other and a shared formatter: `0x004B9040` (211 B, 11 blocks, 70 instrs, 2 KiB stack
   frame) calls `0x004B8CE0`, `0x004B9120`, `0x004B9660` and `0x004A5500`; `0x004B9120`/`0x004B9660`
   call `0x004B9040`; `0x004B9660` calls `0x004B9120`; all three are self-recursive. There is no
   acyclic order inside the cluster.
2. Each performs one **indirect call through `node+0x10`** (`call *0x10(%eax)`; `0x004B91BB`,
   `0x004B9707`, `0x004B8DEF`) — a virtual method on a value-node subtype object. The target lives in
   runtime-initialised tables, so the transcription must keep the indirect call and declare the
   callee as a function pointer taken from the node; the object layout beyond the fields read here is
   not established.
3. `0x004B9120` (write `0x004B9313`) and `0x004B9660` (write `0x004B987D`) store `0x00A965A8`, which
   today has internal linkage (see §3). Until that word is exported, these two cannot be transcribed
   without duplicating retail storage.
4. Additional missing callees: `0x004BABB0` (114 B, 7 blocks, 35 instrs) → `0x004AF910` (engine),
   `0x004AF930` (37 B, missing), `0x004BAB80` (26 B, 1 block, 9 instrs, missing), `0x004BAC30`
   (98 B, 3 blocks, 33 instrs → `0x004B0C60` engine); `0x004BAB80` → `0x004C0620` (61 B, missing);
   `0x004A5500` (254 B, 35 blocks, 109 instrs, **leaf**); `0x005169B0` (291 B, 36 blocks,
   123 instrs) — the catalogue names it `Terrain_RenderQuadtree_4`, which cannot be right: `0x004B8CE0`
   calls it with the same `(source, destination)` shape it uses for `0x004A5500`, and no engine
   definition exists (manifest record points at a script candidate).

Byte-identical retail strings these bodies need (read from the image, all in `.data`):
`0x005473C4` `"Syntax error in string expression, line %d"`,
`0x005473F0` `"Syntax error in assignment, line %d"`,
`0x00547414` `"Expression: Cannot assign to constant, line %d"`,
`0x00547444` `"Internal script error. Invalid assignment, line %d"`,
`0x00547478` `"Unknown identifier '%s' in expression, line %d"`,
`0x005474A8` `"Numeric member in string expression. Line %d."`,
`0x005474D8` `" "` (the two-byte 0x0020 constant written for a null node),
`0x00547AB4` `"Unknown data type."`.

## 6. VM step layer needed by `0x004BCAE0` (second-order closure)

| VA | bytes | blocks | edges | instrs | callees | notes |
| --- | --- | --- | --- | --- | --- | --- |
| `0x004A5500` | 254 | 35 | 56 | 109 | none | **leaf**; bounded byte copy used by the string path |
| `0x004BCC00` | 178 | 9 | 11 | 68 | `0x004BCCC0`, `0x004BCD90` | |
| `0x004BCEC0` | 260 | 12 | 14 | 103 | `0x004BCCC0`, `0x004BCD90`, 1 indirect | |
| `0x004BCCC0` | 208 | 24 | — | 79 | Debug_Error only | near-leaf, error path only |
| `0x004BCD90` | 165 | 7 | — | 69 | `0x004BAB80`, `0x004BCC00`, `0x004BCCC0`, `0x004BCE40` | |
| `0x004BCE40` | 116 | 1 | 0 | 51 | none measured | single block |

## 7. Ordering recommendation and expected link status

Statuses below are symbol-level predictions for the `igi` link, not build results (no build was run).

| Wave | VAs added | New undefined symbols after the wave |
| --- | --- | --- |
| tree today (six handler files + the three accessor files) | — | `0x4BCAB0`, `0x4BD010`, `0x4BD040`, `0x4B9120`, `0x4B9660`, `0x4B8CE0` (6, matches the Lead's integration run) |
| **this pass** (`native_4bd010`, `native_4bcab0`, `native_4bd040`) | `0x4BD010`, `0x4BCAB0`, `0x4BD040` | `0x4BCAE0`, `0x4B9120`, `0x4B9660`, `0x4B8CE0` (4) |
| A | `0x4BCAE0` | `0x4BCC00`, `0x4BCEC0`, `0x4A5500`, `0x4B9120`, `0x4B9660`, `0x4B8CE0` (6 again, but now all in the VM step layer) |
| B | `0x4A5500` (leaf) + `0x4BCC00` + `0x4BCEC0` | `0x4BCCC0`, `0x4BCD90`, `0x4B9120`, `0x4B9660`, `0x4B8CE0` (5) |
| C | `0x4BCCC0` + `0x4BCD90` | `0x4BCE40`, `0x4B9120`, `0x4B9660`, `0x4B8CE0` (4) |
| D | `0x4BCE40` | `0x4B9120`, `0x4B9660`, `0x4B8CE0` (3) — the integer/real/symbol accessor path is then self-contained |
| E | evaluator cluster: `0x4B9040`, `0x4BABB0`, `0x4BAB80`, `0x4BAC30`, `0x4AF930`, `0x4C0620`, `0x005169B0`, plus one evaluator at a time (`0x004B8CE0` 262 instrs, `0x004B9660` 447, `0x004B9120` 484) and the `0x00A965A8` external-linkage change | per evaluator order: `0x4B8CE0` first leaves `0x4B9120`/`0x4B9660`; after all three and the two remaining helpers the accessor branch links green |
| F | Qvm_Execute closure (§4, `0x4B9CD0` is the long pole) then `qvm_execute.cpp` can leave the OBJECT target | `qvm_execute.cpp` then also needs the `0x00A965A0`/`0x00A96598`/`0x00A965A8` linkage fix; the handler/humanplayer branches inherit that |

Practical consequence for the handler set: the six `0x0042D*`/`0x0042F*` handler files need only the
three accessors' symbols, and the accessors' symbols need the whole chain in W — there is no wave
before E that makes the handler branch link. If the goal is a green build sooner, the existing
`OBJECT`-target pattern (`igi_qvm_execute_object`, `igi_humanplayer_defines_object`) is the
sanctioned way to keep the handler files compile-checked without linking them.

## 8. What this record does not claim

- No build/link/run of any file in this pass; the link-status table is a symbol-closure prediction.
- The selector semantics (1 integer, 2 real, 3 string) are established by the three retail dispatch
  branches inside `0x004BCAE0`; the meaning of the frame fields (`+0x04` flag, `+0x08` table,
  `+0x0C` interpreter pointer, `+0x14/+0x18/+0x1C` saved interpreter fields) is not interpreted here.
- The roles attached to the ten callees in §4 come from `qvm_execute.cpp`'s existing comments and
  were only used as labels; the authoritative evidence for each is its own disassembly, which this
  pass measured but did not transcribe.
- Manifest "VERIFIED"/"STRUCTURALLY_VERIFIED" entries whose implementation path is a
  `scripts/GameNatives/...` or catalogue file are **not** engine definitions and do not remove a link
  dependency; the table marks them as script candidates.

## 9. Corrections appended after Lead review (2026-10-25)

1. **The closure is eleven retail VAs, not ten.** `0x004BCAE0` was never in the ten-VA list in the
   `CMakeLists.txt` comment; it is the unlisted eleventh member of the accessor→evaluator closure.
   The three accessors (`0x004BD010`, `0x004BCAB0`, `0x004BD040`) all call it, and it calls
   `0x004BCFD0`, so it belongs to the closure even though the CMake comment omits it.
2. **`0x005169B0` stays unnamed.** The catalogue name `Terrain_RenderQuadtree_4` is not accepted as
   evidence for its behaviour: nothing in this pass derives its arguments, return value or side
   effects. Record it as an unnamed VA whose behaviour still needs derivation. The earlier
   `(source, destination)` shape comparison with `0x004A5500` is **superseded**: `0x004A5500` is the
   statically linked MSVC `strncpy` (its third argument is a count and it returns the destination; see
   `docs/fidelity/evidence/qvm-4a5500-strncpy.md`), so the two VAs cannot be compared on that shape.
   New evidence: `0x004B8CE0` contains a direct `call 0x5169B0` at `0x004B8Fxx` range, so the routine
   is reachable from the string-expression renderer; that call site is the entry point for deriving it.
3. **Wave A result (six transcription units + one CRT identification).** Files added:
   `native_4bce40.cpp` (`0x004BCE40`), `native_4bccc0.cpp` (`0x004BCCC0`), `native_4bcc00.cpp`
   (`0x004BCC00`), `native_4bcd90.cpp` (`0x004BCD90`), `native_4bcec0.cpp` (`0x004BCEC0`),
   `native_4bcae0.cpp` (`0x004BCAE0`). `0x004A5500` deliberately has **no** implementation unit: it is
   the linked MSVC `strncpy`, handled exactly like `0x004A53B3` `sprintf` in
   `engine/src/retail/error/error_show.cpp` (identification + binding to the CRT symbol).
4. **QVM state pointer naming inversion (finding, not fixed here).**
   `engine/include/igi/retail/script/qvm_native_state.hpp` names `+0x00` `code_base` and `+0x14`
   `instruction_offset`, but `0x004BCFD0` computes the opcode byte as `*(+0x14 + *(uint32*)+0x00)`
   (`0x004BCFE3`: `mov 0x14(%esi),%eax`; `0x004BCFE6`: `mov (%esi),%ecx`; `0x004BCFEC`:
   `mov (%eax,%ecx,1),%dl`), and `0x004BCAE0` stores a node pointer into `+0x14` and saves/restores it
   around the call. So `+0x14` is the pointer slot and `+0x00` is the offset; the two names are swapped
   relative to the byte use. The address arithmetic is symmetric, so the existing transcription is
   still correct; only the names are misleading. Wave A files therefore read `+0x14` as a pointer and
   document the inversion instead of using the inverted name.

## 10. Decoded dispatch tables needed by the two remaining evaluators

Measured from the retail image; both are read as 32-bit little-endian code addresses.

**`0x004B9120` (`Qvm_EvaluateIntegerNode_4B9120`)** - 1188 bytes, 65 blocks, 52 edges, 484
instructions, 3 args, 48-byte frame. Dispatch: `node[+1]` as signed byte, `> 0x1D` -> return 0
(`0x004B9135`/`0x004B9138`), jump table **`0x004B95C4`**, 30 entries:

| node[1] | target | node[1] | target | node[1] | target |
| --- | --- | --- | --- | --- | --- |
| 0 | `0x004B9160` | 10 | `0x004B9363` | 20 | `0x004B9277` |
| 1 | `0x004B928E` | 11 | `0x004B9382` | 21 | `0x004B91E5` (return 0) |
| 2 | `0x004B92AD` | 12 | `0x004B93A3` | 22 | `0x004B91E5` |
| 3 | `0x004B923D` | 13 | `0x004B9263` | 23 | `0x004B91E5` |
| 4 | `0x004B924F` | 14 | `0x004B945C` | 24 | `0x004B9145` |
| 5 | `0x004B92CC` | 15 | `0x004B9482` | 25 | `0x004B9501` |
| 6 | `0x004B92EC` | 16 | `0x004B93C4` | 26 | `0x004B91E5` |
| 7 | `0x004B91E5` | 17 | `0x004B93EA` | 27 | `0x004B94A8` |
| 8 | `0x004B9325` | 18 | `0x004B9410` | 28 | `0x004B94CE` |
| 9 | `0x004B9344` | 19 | `0x004B9436` | 29 | `0x004B9209` |

Second dispatch inside the same function: `node[0]` signed byte minus 1, `> 5` -> error path
`0x004B8E79`-equivalent `0x004B91E5`, jump table **`0x004B963C`**, 6 entries: 1 -> `0x004B9170`,
2 -> `0x004B91E5`, 3 -> `0x004B9179`, 4 -> `0x004B918A`, 5 -> `0x004B918A`, 6 -> `0x004B91F7`.
Callees: itself (45 recursion sites), `0x004BABB0` (2), `0x004B7E10` `Qvm_ReportError` (3),
`0x004B9040` (1), `0x004AF7B0` `Debug_Error` (1). Indirect call through `node->[0x10]` at
`0x004B91BB`.

**`0x004B8CE0` (`Qvm_RenderNodeText_4B8CE0`)** - 778 bytes, 41 blocks, 47 edges, 262 instructions,
9 args reported by radare2, 2104-byte frame. Arguments from the prologue and the `0x004B9040` call
site: `(node, char* buffer, uint32 buffer_size, uint32 limit)`; a null node writes the two bytes
`20 00` from `0x005474D8` into the buffer and returns 0 (`0x004B8CF4`-`0x004B8D05`). Dispatch on
`node[+1]` signed byte, `> 0x19` -> error path `0x004B8FBD`, then a **26-byte index table at
`0x004B9000`** `[0,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,2,3]` into jump table
**`0x004B8FEC`** (5 entries): slot 0 -> `0x004B8D5E`, 1 -> `0x004B8E96`, 2 -> `0x004B8D2D`,
3 -> `0x004B8EFC`, 4 -> `0x004B8FBD`. Second dispatch: `node[0]` signed byte minus 1, `> 5` ->
`0x004B8E79`, jump table **`0x004B901C`** (6 entries): 1 -> `0x004B8D72`, 2 -> `0x004B8DA0`,
3 -> `0x004B8D72`, 4 -> `0x004B8DB3`, 5 -> `0x004B8DB3`, 6 -> `0x004B8E36`.
Callees: itself (6), `0x004B7E10` `Qvm_ReportError` (5), `0x004BABB0` (2), `0x004A5500` MSVC
`strncpy` (2), `0x005169B0` (1), `0x004AF7B0` `Debug_Error` (1), `0x004B9040` (1); indirect call
through `node->[0x10]` at `0x004B8DEF`.
Byte-identical literals it passes to `Qvm_ReportError`, read from the image:

| VA | bytes | text |
| --- | --- | --- |
| `0x005473C4` | `53 79 ... 25 64 00` | `Syntax error in string expression, line %d` |
| `0x005473F0` | | `Syntax error in assignment, line %d` |
| `0x00547414` | | `Expression: Cannot assign to constant, line %d` |
| `0x00547444` | | `Internal script error. Invalid assignment, line %d` |
| `0x00547478` | | `Unknown identifier '%s' in expression, line %d` |
| `0x005474A8` | | `Numeric member in string expression. Line %d.` |
| `0x005474D8` | `20 00` | two-byte space + NUL pair |
| `0x00547AB4` | `55 6e ... 2e 00` | `Unknown data type.` |
