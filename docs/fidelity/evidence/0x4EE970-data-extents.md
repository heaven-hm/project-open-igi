# 0x004EE970 — retail static-data extents and the 84 registered handlers

**Status:** evidence only. No candidate source, manifest entry or engine file was changed to produce this document; every claim below is traceable to retail instructions in the image named here.

| | |
| --- | --- |
| Image | `igi.exe` (retail Project IGI 1) |
| SHA-256 | `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01` |
| Image base | `0x00400000`, Win32 x86 PE |
| `.text` | `0x00401000`–`0x00533000` (size `0x132000`) |
| Function under study | `0x004EE970`, size `0x0E7D` (3709 bytes, `0x4EE970`–`0x4EF7EC`) |
| Tools | radare2 6.2.0 (`pD`, `aaa`, `axtj`, `aoj`); byte/opcode fields below are the retail encodings read from the same image |
| Companion artifact | `docs/fidelity/evidence/0x004EE970-full-listing.txt` (committed `pD 3709 @ 0x4EE970`) |

**Reading order.** Part A (§1–§6) answers the original question: the exact extent, owner, field map and clipping risk of every retail static range this function touches. Part B (§7) is the appended handler map — entry, size, CFG, stack-argument ABI and registration site of all 84 code pointers the function pushes — and it is the current priority item. If you only need one thing from this file, read §7.

**Terminology.** "Touched" = referenced by a direct absolute memory operand or a push immediate inside `0x4EE970`. "Extent" = the byte range proven contiguous by the proving instructions, not merely the bytes this function happens to touch: the point is to catch ranges that are part of the same storage but are written/read by *other* owners. "Owner" = the retail function whose instructions read/write the range, with its transcription status in this checkout.

---

## 1. Method and ground rules

- Function body verified from `pD 3709 @ 0x004EE970` of the retail image (the committed listing). No decompiler pseudocode is used as evidence in this document; every extent proof below is an instruction whose bytes decode to an access in the claimed range.
- Cross-references come from `aaa` + `axtj @ <address>` on the same image. In this binary all static-data accesses are absolute 32-bit immediates inside the instruction, so `axtj` yields the complete reader/writer set including write/read permission (`-w-`/`r--`) and the exact opcode text. `aaa` also invents a few giant bogus functions; therefore containment in a "function" is never used as proof, only instruction bytes and loop bounds.
- Completeness of the touched set was checked by decoding **every** absolute value in the listing: 127 distinct absolute references, of which 84 are code pointers (§7), 1 is the address of the `"RGB"` string (`0x0054DF10`, data), and 42 are the data addresses in §2. No other static range is touched directly; indirect effects through calls are explicitly out of scope and listed as such.
- Rules honoured: no absolute retail VA becomes a candidate constant (candidate is `DYNAMIC_BASE`); storage is described as candidate-relative objects/offsets; no invented semantics; unknown extents stay `UNKNOWN` with the instruction that would settle them; no `engine/`, listing, CMake, manifest or git-state changes.

## 2. Region inventory

Eight distinct static runs are touched. "Touched bytes" is what `0x4EE970` itself accesses; "proven extent" is the smallest contiguous span that the proving instructions show belongs to the same storage (including bytes owned by other functions).

| ID | Touched bytes (by `0x4EE970`) | Range | Proven extent | Content | Owner(s) | Risk |
| --- | --- | --- | --- | --- | --- | --- |
| R1 | 10 (`0x0054DD0C`–`0x0054DD15`) | `0x0054DD0C`–`0x0054DD16` | 11 (incl. flag byte `0x0054DD16`) | five class words (`uint16`) + script-state byte | `0x4EE970` (words); flag byte `0x4F0E50`/`0x4F0780` | **WOULD-CLIP** if only 5×`u16` |
| R2 | 16 | `0x0054DDA4`–`0x0054DDB3` | 16 | script callback ids: 2 bytes, 2-byte layout hole, 3 dwords | `0x4EE970` (initial), `0x4F0A60` (reset to `0xFF`) | **WOULD-MISALIGN** if hole removed |
| R3 | 13 dwords | `0x00A70C68`–`0x00A70CA7` | `0x40` (16 dwords) | type handles + script-context fields | `0x4EE970` (13), `0x4F0E50` (+0x0C, +0x30), `0x4F0CE0` (+0x2C) | base **UNKNOWN**; **WOULD-CLIP** at 15 dwords |
| R4 | `0x0BC4` | `0x00A70CA8`–`0x00A7186B` | `0x0BC4` (251 × 3 dwords) | QHash bucket table | `0x4EE970` (init), `0x4F0A60` (teardown) | **WOULD-CLIP** if not 0x2F1 dwords |
| R5 | 9 dwords | `0x00A7186C`–`0x00A7188F` | `0x24` | nine type handles | `0x4EE970` (all) | **WOULD-CLIP** at 8 dwords |
| R6 | `0x4000` | `0x00A71890`–`0x00A7588F` | `0x4000` (4096 dwords) | global dword array | `0x4EE970` (clear), `0x4F0A60` (release) | **WOULD-CLIP** if < 0x1000 dwords or byte-typed |
| R7 | 4 dwords | `0x00A75890`–`0x00A758A3` | `0x14` (incl. hole at `0x00A7589C`) | four type handles + one unproven dword | `0x4EE970` | **WOULD-CLIP/MISALIGN** if hole removed |
| R8 | 10 bytes | `0x00A758A4`–`0x00A758C7` | `0x24` (incl. holes at `0x00A758BF`) | task/parse state fields | `0x4EE970` (4 stores); handler natives (see field map) | **WOULD-CLIP** if holes removed |

Adjacent storage that `0x4EE970` does **not** touch, but which bounds R8: a 16-byte flag array at `0x00A758C8`–`0x00A758D7` and the 64-entry parameter table `0x00A758E0`–`0x00A76BE3` (stride `0x4C`, `0x1300` bytes). See §4.

---

## 3. Region-by-region evidence

### R1 — `0x0054DD0C`, five class words and the script-state flag

Writers in `0x004EE970` (all `mov word [abs],ax`; the immediates are 16-bit and the ids are consumed as 16-bit class words at the registration calls):

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EE999` | `66a30cdd5400` | `mov word [0x54dd0c], ax` |
| `0x004EE9B6` | `66a30edd5400` | `mov word [0x54dd0e], ax` |
| `0x004EE9D3` | `66a310dd5400` | `mov word [0x54dd10], ax` |
| `0x004EE9F0` | `66a312dd5400` | `mov word [0x54dd12], ax` |
| `0x004EEA0B` | `66a314dd5400` | `mov word [0x54dd14], ax` |

The same five words are read back by the registration blocks (Level / Static / Dynamic / Idle / CameraBase, `0x4EEA47`–`0x4EEB9F`):

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EEA47` | `66a10cdd5400` | `mov ax, word [0x54dd0c]` |
| `0x004EEA5A` | `668b0d0cdd5400` | `mov cx, word [0x54dd0c]` |
| `0x004EEA6D` | `668b150cdd5400` | `mov dx, word [0x54dd0c]` |

Extent inside this function: `0x0054DD0C`–`0x0054DD15` (10 bytes). The storage does not end there: byte `0x0054DD16` is a live script-state flag with owners outside this function:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F0909` | `c60516dd540000` | `mov byte [0x54dd16], 0` |
| `0x004F098D` | `c60516dd540001` | `mov byte [0x54dd16], 1` |
| `0x004F0E65` | `880d16dd5400` | `mov byte [0x54dd16], cl` |
| `0x004F0780` | `a016dd5400` | `mov al, byte [0x54dd16]` |

So the object is at least 11 bytes: five 16-bit ids plus one 8-bit flag at +0x0A. The flag byte is set to `0`/`1` inside `0x4F0780` (Task_New) and latched from an argument by `0x4F0E50` (ScriptInit); `0x4F0780` also reads it on entry, so it is a recursion/re-entrancy guard for level-script parsing.

**Field map (base `0x0054DD0C`):**

| Offset | Address | Type | Meaning (from registration use) | Writers | Readers |
| --- | --- | --- | --- | --- | --- |
| +0x00 | `0x0054DD0C` | `uint16` | Level class id, passed to `0x401530`/`0x401400` | `0x4EE970` | `0x4EE970` (3 sites), `0x4F0A60` (`0x4F0C4B`), `0x4F0CB0` |
| +0x02 | `0x0054DD0E` | `uint16` | Static class id | `0x4EE970` | `0x4EE970` (5 sites), `0x4F0A60` (`0x4F0BF7`) |
| +0x04 | `0x0054DD10` | `uint16` | Dynamic class id | `0x4EE970` | `0x4EE970` (3 sites), `0x4F0A60` (`0x4F0C22`) |
| +0x06 | `0x0054DD12` | `uint16` | Idle class id | `0x4EE970` | `0x4EE970` (3 sites), `0x4F0A60` (`0x4F0C32`) |
| +0x08 | `0x0054DD14` | `uint16` | CameraBase class id | `0x4EE970` | `0x4EE970` (3 sites), `0x4F0A60` (`0x4F0C3E`) |
| +0x0A | `0x0054DD16` | `uint8` | script-state / re-entrancy flag | `0x4F0E50`, `0x4F0780` | `0x4F0780` |

**Proven extent:** `[0x0054DD0C, 0x0054DD17)` = 11 bytes, highest byte accessed `0x0054DD16`.
**UNKNOWN:** object start and end outside the accessed span. `0x0054DD0A` (the word before) and `0x0054DD17`+ have **zero** code references in the whole image, so neither the base nor the true end can be proven; only the accessed span is.
**Owner / transcription status:** `0x4EE970` — partial native (`engine/src/retail/script/native_4ee970.cpp`, unwired, not in `native-verification.json`); globals declared in `engine/src/retail/script/native_4ee970_globals.cpp` as `g_54dd0c[5]` (clips the flag, see §5). Flag byte: `0x4F0E50` = `ScriptInit` (manifest `VERIFIED`, but the manifest implementation path points outside this checkout — cannot be inspected here); `0x4F0780` = Task_New, **no native**.
**Clipping risk:** declaring `uint16_t[5]` (10 bytes) makes `0x0054DD16` fall onto the next declared object → **WOULD-CLIP**. Declare 5×`uint16` + `uint8` (+ 1 pad = 12 bytes), or model as offsets 0x00–0x0A of the enclosing area.

### R2 — `0x0054DDA4`, script callback ids

The values are returned live by calls, not constants — `0x4017C0` (byte-size ids) and `0x401810` (dword-size ids): `call 0x4017C0` twice (byte ids, at `0x4EEA11` and `0x4EEA1C`) and `call 0x401810` three times (dword ids, at `0x4EEA27`, `0x4EEA32`, `0x4EEA3D`), each result stored immediately:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EEA17` | `a2a4dd5400` | `mov byte [0x54dda4], al` |
| `0x004EEA22` | `a2a5dd5400` | `mov byte [0x54dda5], al` |
| `0x004EEA2D` | `a3a8dd5400` | `mov dword [0x54dda8], eax` |
| `0x004EEA38` | `a3acdd5400` | `mov dword [0x54ddac], eax` |
| `0x004EEA42` | `a3b0dd5400` | `mov dword [0x54ddb0], eax` |

Group re-initialisation by `0x4F0A60` proves the five fields are one object and fixes the type layout: bytes at +0x00/+0x01, dwords at +0x04/+0x08/+0x0C, with a deliberate 2-byte hole at +0x02 (no instruction in the image references `0x0054DDA6`/`0x0054DDA7`):

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F0BFE` | `b8ff000000` | `mov eax, 0xff` |
| `0x004F0C04` | `a2a4dd5400` | `mov byte [0x54dda4], al` |
| `0x004F0C09` | `a2a5dd5400` | `mov byte [0x54dda5], al` |
| `0x004F0C0E` | `a3a8dd5400` | `mov dword [0x54dda8], eax` |
| `0x004F0C13` | `a3acdd5400` | `mov dword [0x54ddac], eax` |
| `0x004F0C18` | `a3b0dd5400` | `mov dword [0x54ddb0], eax` |

**Field map (base `0x0054DDA4`):**

| Offset | Address | Type | Meaning | Writers | Readers |
| --- | --- | --- | --- | --- | --- |
| +0x00 | `0x0054DDA4` | `uint8` | id from `0x4017C0`; reset value `0xFF` | `0x4EE970`, `0x4F0A60` | `0x4F0A60` (`0x4F0BBD`), `0x4F11F0` |
| +0x01 | `0x0054DDA5` | `uint8` | id from `0x4017C0`; reset value `0xFF` | `0x4EE970`, `0x4F0A60` | `0x4F0A60` (`0x4F0BC9`), `0x4F1200` |
| +0x02 | `0x0054DDA6` | 2-byte pad | unreferenced layout hole | — | — |
| +0x04 | `0x0054DDA8` | `uint32` | id from `0x401810`; reset value `0xFFFFFFFF` | `0x4EE970`, `0x4F0A60` | `0x4F0A60` (`0x4F0BD4`), `0x4F1210` |
| +0x08 | `0x0054DDAC` | `uint32` | id from `0x401810`; reset value `0xFFFFFFFF` | `0x4EE970`, `0x4F0A60` | `0x4F0A60` (`0x4F0BE0`), `0x4F1220` |
| +0x0C | `0x0054DDB0` | `uint32` | id from `0x401810`; reset value `0xFFFFFFFF` | `0x4EE970`, `0x4F0A60` | `0x4F0A60` (`0x4F0BEC`), `0x4F1230` |

**Proven extent:** `[0x0054DDA4, 0x0054DDB4)` = 16 bytes; highest byte accessed `0x0054DDB3`.
**UNKNOWN:** base/end beyond the touched span — `0x0054DDA0` and `0x0054DDB4` have zero code references, so the enclosing object boundary is unprovable. The accessed span and the internal layout are proven.
**Owners:** `0x4EE970` (partial native), `0x4F0A60` (**no native**; candidate file `scripts/GameNatives/script/004F0A60-Script_ParseEnumInt32.cpp` is misnamed for this body — the retail function at `0x4F0A60` is the hash/type/array teardown+reset described in §4), readers `0x4F11F0`/`0x4F1200` / `0x4F1210`/`0x4F1220`/`0x4F1230` all **no native** (candidate files only).
**Clipping risk:** packing the fields without the 2-byte hole moves the three dwords by 2 bytes → **WOULD-MISALIGN**. Keep +0x04/+0x08/+0x0C.

### R3 — `0x00A70C68`, the type-handle and script-context run (`0x40` bytes, 16 dwords)

Thirteen of the sixteen dwords are initialised by `0x004EE970`; every store is a 5-byte `A3 + VA` moffs `mov dword [addr],eax` (proof of the encoding: `0x4EED09` = `a3 68 0c a7 00`, and the same encoding holds for all thirteen — see the field map). All thirteen are the return values of `0x4F19C0(name, -1)` type-registration calls (`type name` strings are pushed as `push str.<name>` immediates just before each `0x4F19C0` call).

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EED09` | `a3680ca700` | `mov dword [0xa70c68], eax` |
| `0x004EEDB4` | `a36c0ca700` | `mov dword [0xa70c6c], eax` |
| `0x004EF78D` | `a3700ca700` | `mov dword [0xa70c70], eax` |
| `0x004EF060` | `a3780ca700` | `mov dword [0xa70c78], eax` |
| `0x004EEFB5` | `a37c0ca700` | `mov dword [0xa70c7c], eax` |
| `0x004EF630` | `a3800ca700` | `mov dword [0xa70c80], eax` |
| `0x004EF739` | `a3840ca700` | `mov dword [0xa70c84], eax` |
| `0x004EF767` | `a3880ca700` | `mov dword [0xa70c88], eax` |
| `0x004EF6E2` | `a38c0ca700` | `mov dword [0xa70c8c], eax` |
| `0x004EF4DA` | `a3900ca700` | `mov dword [0xa70c90], eax` |
| `0x004EF10B` | `a39c0ca700` | `mov dword [0xa70c9c], eax` |
| `0x004EF585` | `a3a00ca700` | `mov dword [0xa70ca0], eax` |
| `0x004EF749` | `a3a40ca700` | `mov dword [0xa70ca4], eax` |

Boundary proof at the top: the next dword at `0x00A70CA8` is the base of the QHash table initialised at `0x4EF7C5` (§R4), so this run ends exactly at `0x00A70CA8`.

Boundary proof at the bottom: **there is none.** `0x00A70C60` and `0x00A70C64` are live globals of other functions — `0x4EE410` writes `0x00A70C60` (`mov dword [0xa70c60],eax` at `0x4EE4BD`) and zeroes `0x00A70C64` (`0x4EE505`), `0x4EE6B0` reads/writes `0x00A70C64` (`0x4EE6E2`, `0x4EE70B`, `0x4EE7B0`, `0x4EE803`, `0x4EE80D`); and `0x4EE88B` pushes `0x00A70C48` as an argument (`push 0xa70c48`). `0x00A70C40`/`0x00A70C44`/`0x00A70C4C`–`0x00A70C5C` have no references. Therefore `0x00A70C68` is a **field offset inside a larger static area**, not a provable object base.

**Field map (base `0x00A70C68`):**

| Offset | Address | Type | Meaning | Writer in `0x4EE970` | Other writers / readers |
| --- | --- | --- | --- | --- | --- |
| +0x00 | `0x00A70C68` | `uint32` | Real32x3 type handle | `0x4EED09` | in-fn `0x4EED18`/`33`/`4B`/`65`/`80`/`98`, `0x4EF70D`; `0x4F0A60` (`0x4F0B3A`), `0x4F0F60` |
| +0x04 | `0x00A70C6C` | `uint32` | Real32x9 type handle | `0x4EEDB4` | in-fn `0x4EEDC3`…`0x4EEE43`; `0x4F0A60` (`0x4F0B45`), `0x4F0F70` |
| +0x08 | `0x00A70C70` | `uint32` | DropDownCombo type handle | `0x4EF78D` | `0x4F0A60` (`0x4F0A8B`), `0x4F0F00` |
| +0x0C | `0x00A70C74` | `uint32` | script file-name pointer | `—` | written by `0x4F0E6B` (ScriptInit) and `0x4F07C1` (inside `0x4F0780`); read by `0x4F0794`, `0x4F08F9` |
| +0x10 | `0x00A70C78` | `uint32` | Int16 type handle | `0x4EF060` | in-fn `0x4EF06F`…`0x4EF0EF`; `0x4F0A60` (`0x4F0B60`), `0x4F0FB0` |
| +0x14 | `0x00A70C7C` | `uint32` | Int32 type handle | `0x4EEFB5` | in-fn `0x4EEFC4`…`0x4EF044`; `0x4F0A60` (`0x4F0B54`), `0x4F0FA0` |
| +0x18 | `0x00A70C80` | `uint32` | GraphEnvelope type handle | `0x4EF630` | in-fn `0x4EF640`…`0x4EF6F2`; `0x4F0A60` (`0x4F0AF4`) |
| +0x1C | `0x00A70C84` | `uint32` | Angle type handle | `0x4EF739` | `0x4F0A60` (`0x4F0AB9`), `0x4F0F20` |
| +0x20 | `0x00A70C88` | `uint32` | ObjectPos type handle | `0x4EF767` | `0x4F0A60` (`0x4F0AAE`), `0x4F1010` |
| +0x24 | `0x00A70C8C` | `uint32` | ScriptFile type handle | `0x4EF6E2` | `0x4F0A60` (`0x4F0A96`), `0x4F0F10` |
| +0x28 | `0x00A70C90` | `uint32` | bool8 type handle | `0x4EF4DA` | in-fn `0x4EF4EA`…`0x4EF75E`; `0x4F0A60` (`0x4F0BB1`), `0x4F1000` |
| +0x2C | `0x00A70C94` | `uint32` | currently-executing level-script handle | `—` | written by `0x4F0CE4` (inside `0x4F0CE0`); read by `0x4F0CD0` |
| +0x30 | `0x00A70C98` | `uint32` | script line number | `—` | written by `0x4F0E5C` (ScriptInit); read by `0x4F078F`, `0x4F08FF` |
| +0x34 | `0x00A70C9C` | `uint32` | VarString type handle | `0x4EF10B` | in-fn `0x4EF11A`…`0x4EF777`; `0x4F0A60` (`0x4F0B6B`), `0x4F0FC0` |
| +0x38 | `0x00A70CA0` | `uint32` | Graph type handle | `0x4EF585` | in-fn `0x4EF595`…`0x4EF614`; `0x4F0A60` (`0x4F0AFF`), `0x4F1020` |
| +0x3C | `0x00A70CA4` | `uint32` | Degrees type handle | `0x4EF749` | `0x4F0A60` (`0x4F0AC5`), `0x4F0F30` |

**Proven extent:** `[0x00A70C68, 0x00A70CA8)` = `0x40` bytes; highest byte accessed `0x00A70CA7`.
**UNKNOWN:** the base. The run is contiguous and the top boundary is exact, but the bottom is not the object base (see above). The instruction that would settle it is any access to `0x00A70C64` interpreted as a structure member — none exists; `0x00A70C64` is accessed as a standalone global by `0x4EE410`/`0x4EE6B0`, so the "object" is either a compiler-layout struct spanning `0x00A70C48`–`0x00A70CA8` or unrelated globals that happen to be adjacent. **Do not declare a standalone object starting at `0x00A70C68`.**
**Owners:** `0x4EE970` (partial native), `0x4F0E50` ScriptInit (manifest `VERIFIED`, external path) for +0x0C/+0x30, `0x4F0CE0` (**no native**, candidate `scripts/GameNatives/script/004F0CD0-Script_ExecuteLevelScript.cpp`) for +0x2C.
**Clipping risk:** 15 dwords (`0x3C`) clips the Degrees handle at `0x00A70CA4` → **WOULD-CLIP**; a standalone declaration at `0x00A70C68` misstates base → **UNKNOWN**. Model as offsets within the containing area.

### R4 — `0x00A70CA8`, the QHash bucket table (251 × 3 dwords = `0x0BC4` bytes)

Init loop in `0x004EE970`:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EF7C5` | `b8a80ca700` | `mov eax, 0xa70ca8` |
| `0x004EF7CA` | `8d4804` | `lea ecx, [eax + 4]` |
| `0x004EF7CD` | `8908` | `mov dword [eax], ecx` |
| `0x004EF7CF` | `894008` | `mov dword [eax + 8], eax` |
| `0x004EF7D2` | `83c00c` | `add eax, 0xc` |
| `0x004EF7D5` | `8919` | `mov dword [ecx], ebx` |
| `0x004EF7D7` | `3d6c18a700` | `cmp eax, 0xa7186c` |
| `0x004EF7DC` | `7cec` | `jl 0x4ef7ca` |

`add eax,0xc` with `cmp eax,0xa7186c` proves exactly `(0xA7186C-0xA70CA8)/0xC = 0xBC4/0xC = 251` iterations; each bucket is 3 dwords: `+0x00` receives the address of its own `+0x04` slot (link-pointer slot), `+0x04` receives `NULL` (first node), `+0x08` receives the bucket address again (node back-pointer).

Independent second proof, teardown loop in `0x4F0A60` — same base, same stride, same bound, different function:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F0A62` | `bfa80ca700` | `mov edi, 0xa70ca8` |
| `0x004F0A80` | `83c70c` | `add edi, 0xc` |
| `0x004F0A83` | `81ff6c18a700` | `cmp edi, 0xa7186c` |
| `0x004F0A89` | `7cdc` | `jl 0x4f0a67` |

Indexing proof: `0x4F11A0` (transcribed as `engine/src/retail/script/native_4f11a0.cpp`, `STRUCTURALLY_VERIFIED`) computes `hash % 251` in `0x4EE960`, multiplies by 3 (`lea eax,[eax+eax*2]`) and reads `[eax*4+0xa70ca8]`, i.e. bucket stride 12 bytes over 251 buckets. `engine/src/retail/script/hash_table_globals.cpp` already declares `g_hash_buckets_a70ca8[0x2F1]` = 251×3 dwords = `0xBC4` bytes, which matches exactly.

**Proven extent:** `[0x00A70CA8, 0x00A7186C)` = `0x0BC4` bytes; both bounds proven twice, by two independent functions.
**Owners:** `0x4EE970` (partial native), `0x4F0A60` (**no native**) for teardown; readers `0x4F11A0` (transcribed native, structural) and `0x4F0C90` (per-node release, manifest `STRUCTURALLY_VERIFIED`).
**Clipping risk:** **NONE** if declared as `uint32_t[0x2F1]`; a `251 × uint32` (1 dword per bucket) declaration is 3× too short → **WOULD-CLIP**.

### R5 — `0x00A7186C`, nine type handles (`0x24` bytes)

All nine written by `0x004EE970`, each `A3 + VA` moffs store to the addresses above; the value stored is the handle returned by the immediately preceding `0x4F19C0(name, -1)` call. The type names, in store order, map 1:1 to the fields:

| Offset | Address | Type stored | Store instruction |
| --- | --- | --- | --- |
| +0x00 | `0x00A7186C` | String256 | `0x4EF30C` `a3 6c 18 a7 00` |
| +0x04 | `0x00A71870` | Real32 | `0x4EEC5E` `a3 70 18 a7 00` |
| +0x08 | `0x00A71874` | Real64 | `0x4EEE5F` `a3 74 18 a7 00` |
| +0x0C | `0x00A71878` | Quat32 | `0x4EEBB3` `a3 78 18 a7 00` |
| +0x10 | `0x00A7187C` | PitchGraph | `0x4EF708` `a3 7c 18 a7 00` |
| +0x14 | `0x00A71880` | `"RGB"` (second registration over Real32x3) | `0x4EF723` `a3 80 18 a7 00` |
| +0x18 | `0x00A71884` | String16 | `0x4EF1B6` `a3 84 18 a7 00` |
| +0x1C | `0x00A71888` | String32 | `0x4EF261` `a3 88 18 a7 00` |
| +0x20 | `0x00A7188C` | LevelExpression | `0x4EF3B7` `a3 8c 18 a7 00` |

**Proven extent:** `[0x00A7186C, 0x00A71890)` = `0x24` bytes; last field ends exactly at the R6 array base `0x00A71890`, so the top boundary is exact. Bottom boundary exact too — R4's bucket table ends at `0x00A7186C`.
**Owners:** `0x4EE970` (all nine, partial native). Readers: `0x4EE970` itself (registration code re-reads each handle), `0x4F0A60` (sites `0x4F0B0B`, `0x4F0B2E`, `0x4F0B8E`, `0x4F0B9A`, `0x4F0AD1`, `0x4F0ADC`, `0x4F0B77`, `0x4F0B83`, `0x4F0B17`), and type getters `0x4F0F40`/`0x4F0F50`/`0x4F0F80`/`0x4F0FD0`/`0x4F0FE0`/`0x4F0FF0` (no engine natives).
**Clipping risk:** 8 dwords clips the LevelExpression handle → **WOULD-CLIP**.

### R6 — `0x00A71890`, the 4096-dword global array (`0x4000` bytes)

Clear loop in `0x004EE970`:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EE974` | `b900100000` | `mov ecx, 0x1000` |
| `0x004EE979` | `33c0` | `xor eax, eax` |
| `0x004EE97B` | `bf9018a700` | `mov edi, 0xa71890` |
| `0x004EE982` | `f3ab` | `rep stosd dword es:[edi], eax` |

`mov ecx,0x1000` + `rep stosd` proves exactly `0x1000` dwords = `0x4000` bytes cleared from `0x00A71890`, ending at the first byte *not* cleared, `0x00A75890`.

Independent bounds proof, per-entry release loop in `0x4F0A60` (dword stride, each non-null entry passed to `0x401B20`, then the slot itself zeroed):

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F0C5B` | `be9018a700` | `mov esi, 0xa71890` |
| `0x004F0C60` | `8b06` | `mov eax, dword [esi]` |
| `0x004F0C62` | `85c0` | `test eax, eax` |
| `0x004F0C64` | `740f` | `je 0x4f0c75` |
| `0x004F0C66` | `50` | `push eax` |
| `0x004F0C67` | `e8b40ef1ff` | `call 0x401b20` |
| `0x004F0C6C` | `83c404` | `add esp, 4` |
| `0x004F0C6F` | `c70600000000` | `mov dword [esi], 0` |
| `0x004F0C78` | `81fe9058a700` | `cmp esi, 0xa75890` |
| `0x004F0C7E` | `7ce0` | `jl 0x4f0c60` |

Third proof, scan loop in `0x4F1050`:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F1050` | `56` | `push esi` |
| `0x004F1051` | `8b742408` | `mov esi, dword [esp + 8]` |
| `0x004F1057` | `b99018a700` | `mov ecx, 0xa71890` |
| `0x004F105C` | `8b11` | `mov edx, dword [ecx]` |
| `0x004F106A` | `81f99058a700` | `cmp ecx, 0xa75890` |

Element type/indexing proof — dword elements at `base + index*4`:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F1095` | `8b04b59018a700` | `mov eax, dword [esi*4 + 0xa71890]` |
| `0x004F10CB` | `8904b59018a700` | `mov dword [esi*4 + 0xa71890], eax` |
| `0x004F10F5` | `c704b59018a70000000000` | `mov dword [esi*4 + 0xa71890], 0` |

Base-address uses: `0x4EE97B` (this function), `0x4F0C5B` (`0x4F0A60`), `0x4F0D42` (`0x4F0D40`), `0x4F0EA3` (`0x4F0E90`), `0x4F1057` (`0x4F1050`). Indexed uses: `0x4F103F` (`0x4F1030`), `0x4F1095` (`0x4F1090`), `0x4F10E5` (`0x4F10E0`). Writers: `0x4F10CB` (`0x4F1090`), `0x4F10F5` (`0x4F10E0`).

**Proven extent:** `[0x00A71890, 0x00A75890)` = `0x4000` bytes; highest byte touched `0x00A7588F`; both bounds proven three times.
**Owners:** `0x4EE970` (clear, partial native), `0x4F0A60` (**no native**), `0x4F1090`/`0x4F10E0` (**no native**, candidate `scripts/GameNatives/script/004F1090-*`, `004F10E0-*`); readers `0x4F1030`/`0x4F1050` (**no native**).
**Clipping risk:** **NONE** for `uint32_t[0x1000]`; `< 0x1000` dwords or a byte-typed declaration (`uint8_t[0x4000]`) breaks the `*4` indexing → **WOULD-CLIP**.

### R7 — `0x00A75890`, four more type handles plus an unproven dword (`0x14` bytes)

| Offset | Address | Content | Store instruction in `0x4EE970` |
| --- | --- | --- | --- |
| +0x00 | `0x00A75890` | Real64x3 type handle | `0x4EEF0A` `a3 90 58 a7 00` |
| +0x04 | `0x00A75894` | QTaskNote type handle | `0x4EF462` `a3 94 58 a7 00` |
| +0x08 | `0x00A75898` | PushButton type handle | `0x4EF77D` `a3 98 58 a7 00` |
| +0x0C | `0x00A7589C` | **UNKNOWN** — zero references in the whole image | — |
| +0x10 | `0x00A758A0` | Bitfield type handle | `0x4EF6F8` `a3 a0 58 a7 00` |

**Proven extent:** `[0x00A75890, 0x00A758A4)` = `0x14` bytes; the lower bound is exact (R6 array ends there), the upper bound is `0x00A758A4` because the next touched dword is R8's first field. The 4-byte hole at +0x0C is real: `axtj @ 0x00A7589C` returns nothing, and `0x00A75898+4 = 0x00A7589C`, `0x00A758A0-4 = 0x00A7589C`, so the gap is a genuine layout hole, not an unanalysed access.
**UNKNOWN:** what the +0x0C dword is (never accessed anywhere in the image; possibly never used by retail code). The instruction that would settle it does not exist in the image — record as unreferenced storage.
**Owners:** `0x4EE970` (4 stores; partial native). Readers: `0x4EE970` itself (registration code), `0x4F0A60` (sites `0x4F0B22` for +0x00, `0x4F0C78` for +0x00 as the array-end comparand, `0x4F0BA6` for +0x04, `0x4F0AA2` for +0x08, `0x4F0AE8` for +0x10), `0x4F0D5F`/`0x4F0EC0`/`0x4F0F90`/`0x4F106A` (all for +0x00), `0x4F0EF0` (for +0x08).
**Clipping risk:** declaring 4 contiguous dwords puts Bitfield at +0x0C → **WOULD-CLIP/MISALIGN**. Keep the hole.

### R8 — `0x00A758A4`, task/parse state fields (`0x24` bytes)

Field map. "Handler" owners below are the handler natives registered by `0x4EE970` (§7); they are the real owners of this state, while `0x4EE970` itself only initialises the last few bytes.

| Offset | Address | Type | Meaning | Writers | Readers |
| --- | --- | --- | --- | --- | --- |
| +0x00 | `0x00A758A4` | ptr | current task object | `0x4F0600` (`0x4F0604`), `0x4F0610` (zero) | `0x4F0580`, `0x4F0D40`, `0x4F0E90`, `0x4F1080`, `0x4F17C0` |
| +0x04 | `0x00A758A8` | ptr | task parameter block | `0x4F0780` (`0x4F085B`), `0x4F0E50` (zero) | `0x4F0780` (`0x4F0852`), `0x4F0CF0` (`0x4F0D2F`) |
| +0x08 | `0x00A758AC` | ptr | dynamic-object state | `0x4F06E0` (`0x4F06E4`), `0x4F06F0` (zero) | `0x4F0EE0` |
| +0x0C | `0x00A758B0` | ptr | static-object state | `0x4F0620` (`0x4F062D`), `0x4F0640` (zero) | `0x4F0620`, `0x4F0640`, `0x4F0ED0` |
| +0x10 | `0x00A758B4` | ptr | idle-object state | `0x4F0700` (`0x4F0704`), `0x4F0710` (zero) | **none** (write-only) |
| +0x14 | `0x00A758B8` | ptr | camera-base state | `0x4F0720` (`0x4F0724`), `0x4F0730` (zero) | **none** (write-only) |
| +0x18 | `0x00A758BC` | `uint8` | script-file dirty/flag byte | `0x4F0E50` (`0x4F0E71`), `0x4F0780` (zero) | `0x4F0780` (`0x4F07FB`) |
| +0x19 | `0x00A758BD` | `uint8` | parse-mode suppress flag | `0x4EE970` (`0x4EF7B9`) | `0x4F0660`, `0x4F1160` |
| +0x1A | `0x00A758BE` | `uint8` | parse-mode re-entrancy flag | `0x4EE970` (`0x4EF7BF`) | `0x4F1160` |
| +0x1B | `0x00A758BF` | 1-byte pad | unreferenced layout hole | — | — |
| +0x1C | `0x00A758C0` | ptr | NULL-initialised slot, no reader found | `0x4EE970` (`0x4EF7DE`) | **none** (write-only) |
| +0x20 | `0x00A758C4` | ptr | NULL-initialised slot, no reader found | `0x4EE970` (`0x4EF7E4`) | **none** (write-only) |

Stores in `0x004EE970` itself ("tail" initialisation, immediately before the function's `ret` at `0x4EF7EC`):

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EF7B9` | `881dbd58a700` | `mov byte [0xa758bd], bl` |
| `0x004EF7BF` | `881dbe58a700` | `mov byte [0xa758be], bl` |
| `0x004EF7DE` | `891dc058a700` | `mov dword [0xa758c0], ebx` |
| `0x004EF7E4` | `891dc458a700` | `mov dword [0xa758c4], ebx` |
| `0x004EF7EC` | `c3` | `ret` |

`0x4EE970` only clears the parse-mode flags (+0x19/+0x1A) and NULLs two dwords (+0x1C/+0x20); the earlier fields are initialised by the handler natives that this same function registers.

**Proven extent:** `[0x00A758A4, 0x00A758C8)` = `0x24` bytes; highest byte accessed `0x00A758C7`. The bypass/dirty-byte hole at +0x1B is a genuine unreferenced byte.
**UNKNOWN:** (a) the roles of +0x1C/+0x20 — write-only, never read by retail code; (b) whether +0x00…+0x0C and +0x10/+0x14 are one struct or several; (c) the type of +0x00/+0x04 — stored values are pointers (written from handler arguments, `0x4F0604` stores `[esp+4]` directly), but the pointed-to type is not established here.
**Owners / transcription status:** `0x4EE970` partial; all handler owners `0x4F0600`, `0x4F0610`, `0x4F0620`, `0x4F0640`, `0x4F0660`, `0x4F06E0`, `0x4F06F0`, `0x4F0700`, `0x4F0710`, `0x4F0720`, `0x4F0730`, `0x4F0780` have **no engine native** (candidate files only); `0x4F0E50` ScriptInit manifest `VERIFIED` (external path); `0x4F1160` `Script_GetParseMode` `STRUCTURALLY_VERIFIED` (reads +0x19/+0x1A). Readers `0x4F0580`, `0x4F0CF0`, `0x4F0D40`, `0x4F0ED0`, `0x4F0EE0`, `0x4F0E90`, `0x4F1080`, `0x4F17C0` — no engine natives (candidates in `scripts/GameNatives`).
**Clipping risk:** dropping the +0x1B hole or the write-only dwords loses byte-accurate layout/observable writes → **WOULD-CLIP**.

---

## 4. The enclosing storage: what is proven and what is not

The eight runs are contiguous from `0x00A70C68` to `0x00A758C8`, and their bounding edges are each proven by at least one independent function:

| Boundary | Proof |
| --- | --- |
| `0x00A70CA8` (start of QHash table) | `0x4EF7C5` init loop base; `0x4F0A62` teardown loop base |
| `0x00A7186C` (end of QHash table / start of nine handles) | loop bound at `0x4EF7D7`; loop bound at `0x4F0A83` |
| `0x00A71890` (start of array) | `0x4EE974`/`0x4EE97B` clear base; `0x4F0C5B`/`0x4F1057` loop base |
| `0x00A75890` (end of array / start of four handles) | `0x4EE982` `rep stosd` end; `0x4F0C78`/`0x4F106A` loop bound |
| `0x00A758C8` (start of 16-byte flag array) | `0x4F1B22` `mov cl,[eax+0xa758c8]` with `cmp eax,0x10` (0x4F1B2D) and `jl` → 16 byte entries; `0x4F1840` zeroes the four dwords at `0x4F1843`, `0x4F184D`, `0x4F1857`, `0x4F185F` |
| `0x00A758E0` (start of 64-entry parameter table) | `0x4F1848` `mov ecx,0x4c0` + `0x4F1852` `mov edi,0xa758e0` + `rep stosd` → `0x1300` bytes, ending `0x00A76BE4` |

Flag array and parameter table instructions (not touched by `0x4EE970`, listed because they bound R8):

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F1B22` | `8a88c858a700` | `mov cl, byte [eax + 0xa758c8]` |
| `0x004F1B28` | `84c9` | `test cl, cl` |
| `0x004F1B2A` | `7415` | `je 0x4f1b41` |
| `0x004F1B2D` | `83f810` | `cmp eax, 0x10` |

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F1840` | `33c0` | `xor eax, eax` |
| `0x004F1843` | `a3c858a700` | `mov dword [0xa758c8], eax` |
| `0x004F1848` | `b9c0040000` | `mov ecx, 0x4c0` |
| `0x004F184D` | `a3cc58a700` | `mov dword [0xa758cc], eax` |
| `0x004F1852` | `bfe058a700` | `mov edi, 0xa758e0` |
| `0x004F1857` | `a3d058a700` | `mov dword [0xa758d0], eax` |
| `0x004F185C` | `f3ab` | `rep stosd dword es:[edi], eax` |
| `0x004F185F` | `a3d458a700` | `mov dword [0xa758d4], eax` |

What is **not** proven:

1. **Base of the whole area.** `0x00A70C48` is pushed as an argument by `0x4EE88B`, and `0x00A70C60`/`0x00A70C64` are used by `0x4EE410`/`0x4EE6B0`. So the strongest defensible statement is: the area begins at or below `0x00A70C48`; `0x00A70C68` is not an object base.
2. **Base of R1/R2** (`0x0054DD0C`, `0x0054DDA4`): the bytes immediately below each (`0x0054DD0A`, `0x0054DDA0`) have zero code references, so a lower bound cannot be derived from this image.
3. **Next object after R8** (`0x00A758C8`): it is a separate 16-byte array by access pattern, but nothing proves it is a different *object* than R8 — they are adjacent fields at worst.
4. **The 64-entry parameter table at `0x00A758E0`**: proven extent only (`0x1300` bytes). Its contents are per-(type,kind) handler pointers indexed `type*0x4C + kind*4` (§7.3), i.e. `0x4C` = 76 bytes = 19 dwords per type; 64 entries × 0x4C = 0x1300.

Practical modelling consequence: declare the **inner blocks** (`0xA70CA8` table, `0xA7186C` handles, `0xA71890` array, `0xA75890` handles) as candidate-relative objects with the proven sizes, and treat R3/R8 as offsets of a containing script-context area rather than standalone objects.

---

## 5. Clipping-risk summary

| Region | Last byte actually accessed | Correct declaration | Declaration that would clip |
| --- | --- | --- | --- |
| R1 | `0x0054DD16` | 5×`uint16` + `uint8` (+pad) | `uint16_t[5]` → clips flag byte |
| R2 | `0x0054DDB3` | `uint8[2]`, pad[2], `uint32[3]` | packed 5 fields, no hole → dword misalignment |
| R3 | `0x00A70CA7` | 16 dwords as offsets of containing area | 15 dwords → clips Degrees; standalone base → wrong layout |
| R4 | `0x00A7186B` | `uint32_t[0x2F1]` | `uint32_t[0xFB]` or `uint32_t[251]` → 3× short |
| R5 | `0x00A7188F` | `uint32_t[9]` | `uint32_t[8]` → clips LevelExpression |
| R6 | `0x00A7588F` | `uint32_t[0x1000]` | `<0x1000` dwords, or `uint8_t[0x4000]` → breaks `index*4` |
| R7 | `0x00A758A3` | 5 dwords incl. hole at +0x0C | 4 contiguous dwords → Bitfield moves to +0x0C |
| R8 | `0x00A758C7` | 0x24 bytes with holes + write-only dwords | dropping +0x1B hole or +0x1C/+0x20 loses observables |

`NONE` cases: R4 and R6 only, and only with the exact declarations above. Everything else has a concrete clip if modelled naively.

---

## 6. Negative evidence (what is *not* touched)

- Absolute references inside `0x004EE970`: 127 distinct values = 84 code pointers (§7) + 1 string address (`0x0054DF10`, `"RGB"`) + 42 data addresses, and those 42 fall exactly into R1–R8. There is no ninth data range.
- Addresses with **zero** code references in the whole image (checked with `axtj`): `0x0054DD0A`, `0x0054DDA0`, `0x0054DDA6`, `0x0054DDA7`, `0x0054DDB4`, `0x00A70C40`, `0x00A70C44`, `0x00A70C4C`–`0x00A70C5C`, `0x00A71860`, `0x00A71864`, `0x00A71868`, `0x00A75888`, `0x00A7588C`, `0x00A7589C`, `0x00A758BF`, `0x00A758D8`.
- `0x0054DF10` is **not** a handler: the listing's `push 0x54df10` at `0x4EF713` pushes the address of the NUL-terminated string `"RGB"` in `.data`. It is the only absolute push in the function that is not code, and it is why a naive "count pushes" gives 85.
- `0x00A70CA6` has zero references, confirming nothing is stored at `0x00A70CA5`-`0x00A70CA7` (R3 dwords are 4-byte aligned).

---

## 7. APPENDED: the 84 registered code pointers — entry, size, CFG, ABI

### 7.1 Extraction and the 83 → 84 correction

**Rule:** every `push imm32` inside `0x004EE970` whose immediate satisfies `0x00401000 ≤ imm < 0x00533000` is a code pointer. The body contains 139 `push` instructions whose operand is an absolute immediate; decoding those immediates yields **84 distinct code addresses** in `.text`, plus exactly one pushed data address (`0x0054DF10`, the `"RGB"` string) and the `-1` sentinel. The 84 code addresses are the complete handler set: no pushed code pointer is left unregistered, and no handler is un-pushed (checked set equality both ways).

**The handoff list of 83 is missing exactly one: `0x0051BAA0`.** Body: `8b 44 24 08` (`mov eax,[esp+8]`) / `c7 00 0c 00 00 00` (`mov dword [eax],0xC`) / `c3` (`ret`), size `0x0B`, registered once by `0x4F1B90` as `Real32x3.kindE9`, reads `[esp+8]`, writes `0x0000000C` through the pointer in that slot. It is a clean 3-instruction entry in `.text`, and it is the only handler the handoff missed.

**What is *not* a handler:** `0x0054DF10` — see §6; it is the `"RGB"` string address, not code.

**Registration multiplicity** (from the push/registration pairs in the listing):

- 74 handlers are registered exactly once.
- `0x4EFA10` — 12 registrations (kind `E6` getter shared by 11 types + `bool8.kindE9`).
- `0x4D3710` — 8 registrations (Static.method1, Idle.method1, VarString.kindEB, Graph.kindEA/EB/E7/E8/E6); it is a no-op `ret` handler.
- `0x4EFB00`, `0x4F0380`, `0x4F03D0`, `0x4F04D0` — 4 each (shared size/clear handlers across string/float vector types).
- `0x4F0430` — 3; `0x4EFA90`, `0x4EFAA0`, `0x427D70` — 2 each.

### 7.2 Are they real function entries?

All 84 pass the entry test; none is interior code, a thunk, or a data blob:

- **All 84 end in a plain `C3 ret`** (`last bytes c3`), and none ends in `C2 imm16`. There is no `stdcall`/`thiscall` callee-cleanup anywhere in the set.
- **No handler starts with `jmp`** (no thunks/aliases), and no two handlers overlap.
- **Entry padding:** 82 entries are preceded by NOP padding after the previous function's `ret` (`PAD_AFTER_RET`). Two are adjacent to the previous function's `ret`:
  - `0x004D3710` — a genuine 1-byte function `c3` (`ret`), registered 8×, and additionally called directly from 13 places in `.text` (e.g. `0x482784 e8 87 0f 05 00 call 0x4d3710`). It is the deliberate "no operation" callback for handlers that need none.
  - `0x004F0600` — starts exactly at `0x004F0580 + 0x80`; the previous function's last instruction is its `ret` at `0x004F05FF`, so no padding exists but the boundary is exact.
- **Size range:** 1 byte (`0x4D3710`) to `0x2DB` bytes (`0x4F0780`). CFG shape: 59 handlers are single-basic-block, 15 have 3 blocks, 2 have 5, 1 has 8, 4 have 11, 2 have 12, and `0x4F0780` has 30 (44 edges).
- **Disassembly source:** each size/end/CFG came from `agfj`/`afi` on `aaa`-analysed image and was cross-checked against the next function's entry padding; for the two adjacent entries the check is the `ret` byte of the predecessor.

### 7.3 Callback ABI and where the pointers live

Three registration channels are used, each with different storage:

**Channel 1 — type-attribute callbacks** (66 handlers are registered only here; `0x4D3710` also appears here, after its 8 class-table registrations). These handlers are registered through `0x4F1B60`/`0x4F1B70`/`0x4F1B80`/`0x4F1B90`/`0x4F1BA0`/`0x4F1BB0` (kind getters) + `0x4F1BC0`. The getters return a one-byte kind tag read from static bytes: `0x4F1B60`←`0x0054E0EA`, `0x4F1B70`←`0x0054E0E6`, `0x4F1B80`←`0x0054E0EB`, `0x4F1B90`←`0x0054E0E9`, `0x4F1BA0`←`0x0054E0E7`, `0x4F1BB0`←`0x0054E0E8`. `0x4F1BC0(type, kind, handler)` computes `19*type + (int8)kind` and stores `handler` at `[that*4 + 0xA758E8]`. The dispatcher `0x4F1AF0(type, kind, value, arg0)` computes the identical index and then calls the stored pointer:

   | VA | bytes | instruction |
| --- | --- | --- |
| `0x004F1AF0` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x004F1AF4` | `8d0cc0` | `lea ecx, [eax + eax*8]` |
| `0x004F1AF7` | `8d1448` | `lea edx, [eax + ecx*2]` |
| `0x004F1AFA` | `8b4c2410` | `mov ecx, dword [esp + 0x10]` |
| `0x004F1AFE` | `0fbe44240c` | `movsx eax, byte [esp + 0xc]` |
| `0x004F1B03` | `03d0` | `add edx, eax` |
| `0x004F1B05` | `8b442404` | `mov eax, dword [esp + 4]` |
| `0x004F1B09` | `51` | `push ecx` |
| `0x004F1B0A` | `50` | `push eax` |
| `0x004F1B0B` | `ff1495e858a700` | `call dword [edx*4 + 0xa758e8]` |

   The two `push`es immediately before the indirect call establish the callback ABI: `fn(arg0, value)` — exactly **two cdecl arguments**, the dispatcher's first argument and its third. Therefore every attribute handler registered this way receives `[esp+4]` = caller's arg0 and `[esp+8]` = value; handlers that only read `[esp+8]` ignore the caller's arg0. Proof of the store side (the two `lea`s build `19*type`, `movsx` sign-extends the kind byte's low 8 bits, the handler is the 3rd argument):

   | VA | bytes | instruction |
| --- | --- | --- |
| `0x004F1BC1` | `8b5c240c` | `mov ebx, dword [esp + 0xc]` |
| `0x004F1BC6` | `8b6c240c` | `mov ebp, dword [esp + 0xc]` |
| `0x004F1BCA` | `0fbed3` | `movsx edx, bl` |
| `0x004F1BCD` | `8d44ed00` | `lea eax, [ebp + ebp*8]` |
| `0x004F1BD8` | `8d4c4500` | `lea ecx, [ebp + eax*2]` |
| `0x004F1BE0` | `03ca` | `add ecx, edx` |
| `0x004F1BE4` | `89048de858a700` | `mov dword [ecx*4 + 0xa758e8], eax` |

**Channel 2 — class methods and class properties** (16 handlers). Registered through `0x401530(classWord, id, handler)` / `0x401400(classWord, state, handler)` with the class words of R1. The two `0x487E20`/`0x487D90` registrations take their `state` byte from `0x4CEA20`/`0x4CEA10` instead of an immediate. The handler entry code reads at most two stack slots. **UNKNOWN:** the invocation site of these two tables was not located in this pass; it is not needed to fix the callback signature (all these handlers are cdecl with 1–2 args), but it *is* needed if the call sites are to be transcribed.
**Channel 3 — QVM task functions** (2 handlers). `0x4B8890(name, fn, 0, -1)` registers `0x4F0740` (`"Task_DeclareParameters"`) and `0x4F0780` (`"Task_New"`) as QVM native task functions; their ABI is the QVM invocation convention, and `0x4F0780` reads three incoming slots (`[esp+4]`, `[esp+8]`, `[esp+0xC]`).

**Observed incoming-argument reads** (computed by CFG dataflow over each handler's complete body, including both branch directions and loops; entry-relative slot offsets, `[esp+4]` = first argument; `0x4EF8C0` uses a frame pointer, `[ebp+8]`/`[ebp+0xC]`, normalised here to `[esp+4]`/`[esp+8]`):

| slots read | handlers | meaning |
| --- | --- | --- |
| `[esp+0x4]`, `[esp+0x8]` | 55 | arg0 + arg1 |
| `[esp+0x8]` | 13 | arg1 only (arg0 ignored) |
| `[esp+0x4]` | 9 | arg0 only |
| — | 6 | zero-argument callback |
| `[esp+0x4]`, `[esp+0x8]`, `[esp+0xc]` | 1 | arg0 + arg1 + arg2 |

Notes:

- No handler writes to an incoming argument slot; every write is to its own stack frame or to static/heap memory. (Checked for all 84; the earlier suspicion about `0x4EFD50` was a linear-scan artifact — its `[esp+0x38]` access is a *read* of arg0 after `sub esp,0x34`.)
- All handlers return in `EAX`; `0x4F1AF0` passes that value back to its caller. Whether individual call sites consume the value is not part of this pass.
- No 4th argument is ever read, so the effective maximum arity is 3 (`0x4F0780`), and only `0x4F0780` + one shared converter take 3.

### 7.4 The 84-handler table

Columns: `VA` (retail entry), `size`/`end` from CFG recovery, `bb`/`edges`/`insn` from `agfj`, `arg slots read` (entry-relative, as above), `registration` (derived from the listing's registration sequences: type + kind tag for attribute handlers, class + method/property slot for class handlers), `entry` = first 4 retail bytes (for boundary verification).

| # | VA | size | end | bb | edges | insn | arg slots read | registration | entry bytes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | `0x00427d70` | 11 | `0x00427d7b` | 1 | 0 | 3 | `[esp+0x8]` | Int32.kindE9, Real32.kindE9 | `8b442408` |
| 2 | `0x00427f80` | 11 | `0x00427f8b` | 1 | 0 | 3 | `[esp+0x8]` | Real64.kindE9 | `8b442408` |
| 3 | `0x00487d90` | 129 | `0x00487e11` | 11 | 15 | 45 | `[esp+0x4]`, `[esp+0x8]` | Static.prop(0x4CEA10 state) | `8b442404` |
| 4 | `0x00487e20` | 129 | `0x00487ea1` | 11 | 15 | 45 | `[esp+0x4]`, `[esp+0x8]` | Static.prop(0x4CEA20 state) | `8b442404` |
| 5 | `0x00488700` | 104 | `0x00488768` | 11 | 15 | 36 | `[esp+0x4]` | CameraBase.method1 | `8b442404` |
| 6 | `0x004d3710` | 1 | `0x004d3711` | 1 | 0 | 1 | — | Static.method1, Idle.method1, VarString.kindEB, Graph.kindEA, Graph.kindEB, Graph.kindE7, Graph.kindE8, Graph.kindE6 | `c3` |
| 7 | `0x004ef7f0` | 53 | `0x004ef825` | 1 | 0 | 24 | `[esp+0x4]`, `[esp+0x8]` | QTaskNote.kindE7 | `8b442404` |
| 8 | `0x004ef830` | 21 | `0x004ef845` | 1 | 0 | 8 | `[esp+0x4]`, `[esp+0x8]` | QTaskNote.kindE8 | `8b442408` |
| 9 | `0x004ef850` | 11 | `0x004ef85b` | 1 | 0 | 3 | `[esp+0x8]` | QTaskNote.kindE9 | `8b442408` |
| 10 | `0x004ef860` | 66 | `0x004ef8a2` | 3 | 3 | 31 | `[esp+0x4]`, `[esp+0x8]` | GraphEnvelope.kindEA | `53` |
| 11 | `0x004ef8b0` | 11 | `0x004ef8bb` | 1 | 0 | 3 | `[esp+0x8]` | GraphEnvelope.kindE6 | `8b442408` |
| 12 | `0x004ef8c0` | 196 | `0x004ef984` | 8 | 11 | 78 | `[esp+0x4]`, `[esp+0x8]` | GraphEnvelope.kindEB | `55` |
| 13 | `0x004ef990` | 22 | `0x004ef9a6` | 1 | 0 | 10 | `[esp+0x4]`, `[esp+0x8]` | GraphEnvelope.kindE7 | `8b442408` |
| 14 | `0x004ef9b0` | 22 | `0x004ef9c6` | 1 | 0 | 10 | `[esp+0x4]`, `[esp+0x8]` | GraphEnvelope.kindE8 | `8b442408` |
| 15 | `0x004ef9d0` | 11 | `0x004ef9db` | 1 | 0 | 3 | `[esp+0x8]` | GraphEnvelope.kindE9 | `8b442408` |
| 16 | `0x004ef9e0` | 41 | `0x004efa09` | 1 | 0 | 18 | `[esp+0x4]`, `[esp+0x8]` | Real32.kindEA | `56` |
| 17 | `0x004efa10` | 11 | `0x004efa1b` | 1 | 0 | 3 | `[esp+0x8]` | Int16.kindE6, Int32.kindE6, LevelExpression.kindE6, QTaskNote.kindE6, Real32.kindE6, Real64.kindE6, String16.kindE6, String256.kindE6, String32.kindE6, VarString.kindE6, bool8.kindE6, bool8.kindE9 | `8b442408` |
| 18 | `0x004efa20` | 100 | `0x004efa84` | 3 | 4 | 43 | `[esp+0x4]`, `[esp+0x8]` | Real32.kindEB | `8b442408` |
| 19 | `0x004efa90` | 15 | `0x004efa9f` | 1 | 0 | 6 | `[esp+0x4]`, `[esp+0x8]` | Int32.kindE7, Real32.kindE7 | `8b442408` |
| 20 | `0x004efaa0` | 15 | `0x004efaaf` | 1 | 0 | 6 | `[esp+0x4]`, `[esp+0x8]` | Int32.kindE8, Real32.kindE8 | `8b442408` |
| 21 | `0x004efab0` | 79 | `0x004efaff` | 1 | 0 | 34 | `[esp+0x4]`, `[esp+0x8]` | Real32x3.kindEA | `53` |
| 22 | `0x004efb00` | 11 | `0x004efb0b` | 1 | 0 | 3 | `[esp+0x8]` | Quat32.kindE6, Real32x3.kindE6, Real32x9.kindE6, Real64x3.kindE6 | `8b442408` |
| 23 | `0x004efb10` | 66 | `0x004efb52` | 3 | 4 | 23 | `[esp+0x4]`, `[esp+0x8]` | Real32x3.kindEB | `8b442408` |
| 24 | `0x004efb60` | 27 | `0x004efb7b` | 1 | 0 | 10 | `[esp+0x4]`, `[esp+0x8]` | Real32x3.kindE7 | `8b4c2408` |
| 25 | `0x004efb80` | 27 | `0x004efb9b` | 1 | 0 | 10 | `[esp+0x4]`, `[esp+0x8]` | Real32x3.kindE8 | `8b442408` |
| 26 | `0x004efba0` | 108 | `0x004efc0c` | 1 | 0 | 42 | `[esp+0x4]`, `[esp+0x8]` | Real32x9.kindEA | `83ec08` |
| 27 | `0x004efc10` | 120 | `0x004efc88` | 3 | 4 | 41 | `[esp+0x4]`, `[esp+0x8]` | Real32x9.kindEB | `8b442408` |
| 28 | `0x004efc90` | 22 | `0x004efca6` | 1 | 0 | 10 | `[esp+0x4]`, `[esp+0x8]` | Real32x9.kindE7 | `8b442408` |
| 29 | `0x004efcb0` | 22 | `0x004efcc6` | 1 | 0 | 10 | `[esp+0x4]`, `[esp+0x8]` | Real32x9.kindE8 | `8b442408` |
| 30 | `0x004efcd0` | 11 | `0x004efcdb` | 1 | 0 | 3 | `[esp+0x8]` | Real32x9.kindE9 | `8b442408` |
| 31 | `0x004efce0` | 108 | `0x004efd4c` | 1 | 0 | 42 | `[esp+0x4]`, `[esp+0x8]` | Quat32.kindEA | `83ec08` |
| 32 | `0x004efd50` | 399 | `0x004efedf` | 12 | 17 | 133 | `[esp+0x4]`, `[esp+0x8]` | Quat32.kindEB | `8b442408` |
| 33 | `0x004efee0` | 22 | `0x004efef6` | 1 | 0 | 10 | `[esp+0x4]`, `[esp+0x8]` | Quat32.kindE7 | `8b442408` |
| 34 | `0x004eff00` | 22 | `0x004eff16` | 1 | 0 | 10 | `[esp+0x4]`, `[esp+0x8]` | Quat32.kindE8 | `8b442408` |
| 35 | `0x004eff20` | 11 | `0x004eff2b` | 1 | 0 | 3 | `[esp+0x8]` | Quat32.kindE9 | `8b442408` |
| 36 | `0x004eff30` | 41 | `0x004eff59` | 1 | 0 | 18 | `[esp+0x4]`, `[esp+0x8]` | Real64.kindEA | `56` |
| 37 | `0x004eff60` | 46 | `0x004eff8e` | 3 | 4 | 18 | `[esp+0x4]`, `[esp+0x8]` | Real64.kindEB | `8b442408` |
| 38 | `0x004eff90` | 21 | `0x004effa5` | 1 | 0 | 8 | `[esp+0x4]`, `[esp+0x8]` | Real64.kindE7 | `8b442408` |
| 39 | `0x004effb0` | 21 | `0x004effc5` | 1 | 0 | 8 | `[esp+0x4]`, `[esp+0x8]` | Real64.kindE8 | `8b442408` |
| 40 | `0x004effd0` | 79 | `0x004f001f` | 1 | 0 | 34 | `[esp+0x4]`, `[esp+0x8]` | Real64x3.kindEA | `53` |
| 41 | `0x004f0020` | 62 | `0x004f005e` | 3 | 4 | 26 | `[esp+0x4]`, `[esp+0x8]` | Real64x3.kindEB | `8b442408` |
| 42 | `0x004f0060` | 22 | `0x004f0076` | 1 | 0 | 10 | `[esp+0x4]`, `[esp+0x8]` | Real64x3.kindE7 | `8b442408` |
| 43 | `0x004f0080` | 22 | `0x004f0096` | 1 | 0 | 10 | `[esp+0x4]`, `[esp+0x8]` | Real64x3.kindE8 | `8b442408` |
| 44 | `0x004f00a0` | 11 | `0x004f00ab` | 1 | 0 | 3 | `[esp+0x8]` | Real64x3.kindE9 | `8b442408` |
| 45 | `0x004f00b0` | 41 | `0x004f00d9` | 1 | 0 | 18 | `[esp+0x4]`, `[esp+0x8]` | Int32.kindEA | `56` |
| 46 | `0x004f00e0` | 42 | `0x004f010a` | 3 | 4 | 16 | `[esp+0x4]`, `[esp+0x8]` | Int32.kindEB | `8b442408` |
| 47 | `0x004f0110` | 42 | `0x004f013a` | 1 | 0 | 18 | `[esp+0x4]`, `[esp+0x8]` | Int16.kindEA | `56` |
| 48 | `0x004f0140` | 43 | `0x004f016b` | 3 | 4 | 16 | `[esp+0x4]`, `[esp+0x8]` | Int16.kindEB | `8b442408` |
| 49 | `0x004f0170` | 17 | `0x004f0181` | 1 | 0 | 6 | `[esp+0x4]`, `[esp+0x8]` | Int16.kindE7 | `8b442408` |
| 50 | `0x004f0190` | 17 | `0x004f01a1` | 1 | 0 | 6 | `[esp+0x4]`, `[esp+0x8]` | Int16.kindE8 | `8b442408` |
| 51 | `0x004f01b0` | 11 | `0x004f01bb` | 1 | 0 | 3 | `[esp+0x8]` | Int16.kindE9 | `8b442408` |
| 52 | `0x004f01c0` | 167 | `0x004f0267` | 5 | 6 | 64 | `[esp+0x4]`, `[esp+0x8]` | VarString.kindEA | `81ec0004` |
| 53 | `0x004f0270` | 59 | `0x004f02ab` | 3 | 2 | 28 | `[esp+0x4]`, `[esp+0x8]` | VarString.kindE7 | `8b442404` |
| 54 | `0x004f02b0` | 107 | `0x004f031b` | 5 | 6 | 48 | `[esp+0x4]`, `[esp+0x8]` | VarString.kindE8 | `53` |
| 55 | `0x004f0320` | 40 | `0x004f0348` | 3 | 2 | 17 | `[esp+0x4]`, `[esp+0x8]` | VarString.kindE9 | `8b442404` |
| 56 | `0x004f0350` | 42 | `0x004f037a` | 1 | 0 | 19 | `[esp+0x4]`, `[esp+0x8]` | String16.kindEA | `56` |
| 57 | `0x004f0380` | 22 | `0x004f0396` | 1 | 0 | 10 | `[esp+0x4]`, `[esp+0x8]` | LevelExpression.kindE9, String16.kindE9, String256.kindE9, String32.kindE9 | `57` |
| 58 | `0x004f03a0` | 42 | `0x004f03ca` | 1 | 0 | 19 | `[esp+0x4]`, `[esp+0x8]` | String32.kindEA | `56` |
| 59 | `0x004f03d0` | 42 | `0x004f03fa` | 1 | 0 | 20 | `[esp+0x4]`, `[esp+0x8]` | LevelExpression.kindE8, String16.kindE8, String256.kindE8, String32.kindE8 | `8b442408` |
| 60 | `0x004f0400` | 45 | `0x004f042d` | 1 | 0 | 19 | `[esp+0x4]`, `[esp+0x8]` | String256.kindEA | `56` |
| 61 | `0x004f0430` | 40 | `0x004f0458` | 3 | 4 | 15 | `[esp+0x4]`, `[esp+0x8]` | String16.kindEB, String256.kindEB, String32.kindEB | `8b442408` |
| 62 | `0x004f0460` | 57 | `0x004f0499` | 1 | 0 | 28 | `[esp+0x4]`, `[esp+0x8]` | LevelExpression.kindEA | `56` |
| 63 | `0x004f04a0` | 42 | `0x004f04ca` | 3 | 4 | 16 | `[esp+0x4]`, `[esp+0x8]` | LevelExpression.kindEB | `8b442408` |
| 64 | `0x004f04d0` | 42 | `0x004f04fa` | 1 | 0 | 20 | `[esp+0x4]`, `[esp+0x8]` | LevelExpression.kindE7, String16.kindE7, String256.kindE7, String32.kindE7 | `56` |
| 65 | `0x004f0500` | 41 | `0x004f0529` | 1 | 0 | 18 | `[esp+0x4]`, `[esp+0x8]` | bool8.kindEA | `56` |
| 66 | `0x004f0530` | 43 | `0x004f055b` | 3 | 4 | 16 | `[esp+0x4]`, `[esp+0x8]` | bool8.kindEB | `8b442408` |
| 67 | `0x004f0560` | 15 | `0x004f056f` | 1 | 0 | 6 | `[esp+0x4]`, `[esp+0x8]` | bool8.kindE7 | `8b442408` |
| 68 | `0x004f0570` | 15 | `0x004f057f` | 1 | 0 | 6 | `[esp+0x4]`, `[esp+0x8]` | bool8.kindE8 | `8b442408` |
| 69 | `0x004f0580` | 128 | `0x004f0600` | 11 | 15 | 43 | `[esp+0x4]` | Level.method1 | `8b442404` |
| 70 | `0x004f0600` | 10 | `0x004f060a` | 1 | 0 | 3 | `[esp+0x4]` | Level.prop(0) | `8b442404` |
| 71 | `0x004f0610` | 11 | `0x004f061b` | 1 | 0 | 2 | — | Level.method2 | `c705a458` |
| 72 | `0x004f0620` | 19 | `0x004f0633` | 3 | 3 | 6 | `[esp+0x4]` | Static.prop(0) | `a1b058a7` |
| 73 | `0x004f0640` | 20 | `0x004f0654` | 3 | 3 | 5 | — | Static.method2 | `a1b058a7` |
| 74 | `0x004f0660` | 113 | `0x004f06d1` | 12 | 17 | 39 | `[esp+0x4]` | Dynamic.method1 | `a0bd58a7` |
| 75 | `0x004f06e0` | 10 | `0x004f06ea` | 1 | 0 | 3 | `[esp+0x4]` | Dynamic.prop(0) | `8b442404` |
| 76 | `0x004f06f0` | 11 | `0x004f06fb` | 1 | 0 | 2 | — | Dynamic.method2 | `c705ac58` |
| 77 | `0x004f0700` | 10 | `0x004f070a` | 1 | 0 | 3 | `[esp+0x4]` | Idle.prop(0) | `8b442404` |
| 78 | `0x004f0710` | 11 | `0x004f071b` | 1 | 0 | 2 | — | Idle.method2 | `c705b458` |
| 79 | `0x004f0720` | 10 | `0x004f072a` | 1 | 0 | 3 | `[esp+0x4]` | CameraBase.prop(0) | `8b442404` |
| 80 | `0x004f0730` | 11 | `0x004f073b` | 1 | 0 | 2 | — | CameraBase.method2 | `c705b858` |
| 81 | `0x004f0740` | 60 | `0x004f077c` | 1 | 0 | 16 | `[esp+0x4]` | QVM:Task_DeclareParameters | `83ec18` |
| 82 | `0x004f0780` | 731 | `0x004f0a5b` | 30 | 44 | 228 | `[esp+0x4]`, `[esp+0x8]`, `[esp+0xc]` | QVM:Task_New | `a016dd54` |
| 83 | `0x004f64e0` | 11 | `0x004f64eb` | 1 | 0 | 3 | `[esp+0x8]` | Graph.kindE9 | `8b442408` |
| 84 | `0x0051baa0` | 11 | `0x0051baab` | 1 | 0 | 3 | `[esp+0x8]` | Real32x3.kindE9 | `8b442408` |

### 7.5 Family split and the largest handlers

- **76 handlers** lie in the contiguous code block `0x004EF7F0`–`0x004F0790` that immediately follows the initialiser (whose `ret` is at `0x4EF7EC`); they are the per-type/per-class callbacks registered by that initialiser. 39 of them are ≤ 30 bytes.
- **8 outliers:** `0x427D70`, `0x427F80` (getter pairs reused from the enum parser), `0x487D90`, `0x487E20` (Static class property handlers, 129 bytes each, shared), `0x488700` (CameraBase.method1, 104 bytes), `0x4D3710` (1-byte no-op), `0x4F64E0` (Graph.kindE9 getter), `0x51BAA0` (Real32x3.kindE9 setter, the one missing from the 83 list).
- **Largest:** `0x4F0780` Task_New (`0x2DB`/731 B, 30 bb, 44 edges, 228 insn), `0x4EFD50` Quat32 kindEB (`0x18F`, 12 bb, 133 insn), `0x4EF8C0` GraphEnvelope kindEB (`0xC4`, 8 bb, 78 insn), `0x4F01C0` VarString kindEA (`0xA7`, 5 bb, 64 insn), `0x487D90`/`0x487E20` (`0x81` each, 11 bb, 45 insn), `0x4F0580` Level.method1 (`0x80`, 11 bb, 43 insn), `0x4EFC10` Real32x9 kindEB (`0x78`), `0x4F0660` Dynamic.method1 (`0x71`), `0x4EFBA0` Real32x9 kindEA (`0x6C`).

### 7.6 Callees of the larger handlers (transcription dependencies)

Call targets reported by CFG recovery for the handler bodies (dependency planning only — not behavioural proof); each target needs a candidate native before the caller can be transcribed faithfully:

| Handler | Bytes | Insns | Calls |
| --- | --- | --- | --- |
| `0x004F0780` | 731 | 228 | `0x004b8ad0`, `0x00401a70`, `0x004013b0`, `0x004012a0`, `0x004f1030`, `0x004f1090`, `0x004f1a60`, `0x004f1b60`, `0x004f1af0`, `0x004f1210`, `0x004f1230`, `0x004b8930`, `0x004b8b00` |
| `0x004EFD50` | 399 | 133 | `0x004a53b3` |
| `0x004EF8C0` | 196 | 78 | `0x004a53b3` |
| `0x004F01C0` | 167 | 64 | `0x004b0d10`, `0x004b8a80`, `0x004b0c60` |
| `0x004F0580` | 128 | 43 | — |
| `0x004F0660` | 113 | 39 | — |
| `0x00487D90` | 129 | 45 | `0x004cea10` |
| `0x00487E20` | 129 | 45 | `0x004cea20` |
| `0x00488700` | 104 | 36 | — |
| `0x004EFA20` | 100 | 43 | `0x004a53b3` |
| `0x004EFB10` | 66 | 23 | `0x004a53b3` |
| `0x004F0270` | 59 | 28 | — |
| `0x004F02B0` | 107 | 48 | `0x004b0d10`, `0x004b0c60` |
| `0x004F0320` | 40 | 17 | — |
| `0x004EFCE0` | 108 | 42 | — |
| `0x004F04A0` | 42 | 16 | `0x004a53b3` |

### 7.7 What the Lead needs from this section

1. **84, not 83.** Add `0x0051BAA0` (Real32x3.kindE9, 11 bytes, reads `[esp+8]`, writes 0xC through it).
2. **All 84 are clean `ret`-terminated cdecl entries** with 0–3 stack arguments: 55 read `[esp+4]`+`[esp+8]`; 13 read only `[esp+8]`; 9 read only `[esp+4]`; 6 read none; 1 reads three. No `ret imm16`, no thunks, no interior-code pointers.
3. **Each handler push immediate must become a candidate symbol reference**, never a retail absolute constant (candidate is `DYNAMIC_BASE`). The registration/table natives (`0x401530`, `0x401400`, `0x4F1B60`–`0x4F1BB0`, `0x4F1BC0`, `0x4F1AF0`, `0x4B8890`, `0x4F19C0`) must exist as candidate natives with their retail signatures before the initialiser can be transcribed; the stored values are candidate function pointers.
4. **Suggested order:** the six zero-argument clear/no-op handlers (`0x4D3710`, `0x4F0610`, `0x4F06F0`, `0x4F0710`, `0x4F0730`, `0x4F0640`) and the 13 `[esp+8]`-only getters/setters first (3 instructions each), then the 9 `[esp+4]`-only writers, then the 55 two-argument converters; leave `0x4F0780` (731 B, depends on 13 natives incl. the QVM) and `0x4EFD50`/`0x4EF8C0` for last.
5. **UNKNOWNs in this section:** the invocation sites for the class-method/property tables (`0x401530`/`0x401400`) were not located; the exact semantics of every handler body (only entry/size/CFG/ABI is claimed here, not behavior); and whether an individual call site consumes each handler's `EAX` result.

---


---

## 8. APPENDED — transcription sheet: the six zero-argument handlers

The 84-handler map is §7. This section is the first half of the transcription sheet requested by the Lead: the **six handlers whose bodies read no incoming stack argument**. The thirteen `[esp+8]`-only handlers follow in §9. Sheet only — no `engine/` file, manifest entry or listing was changed to produce it.

### 8.0 Refinements to §7.3 (must be read before relying on §7.3 wording)

Preparing this sheet turned up four details that §7.3 states too loosely or omits. They do not change the 84-handler table or the `fn(arg0, value)` conclusion; they fix how the registration machinery must be transcribed.

**(a) The six "kind getters" take no arguments.** Each is exactly two instructions — `mov al, byte [<slot>]; ret` — and reads one byte from static data; e.g. `0x4F1B90` = `a0 e9 e0 54 00` / `c3` (`mov al, byte [0x0054E0E9]; ret`). The `push <handler>` that precedes the getter call in the listing is **not** the getter's argument: no `add esp` follows the getter call, so the pushed handler stays on the stack and becomes the **third** argument of `0x4F1BC0`. Exact sequence (bytes from the committed listing):

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EECCA` | `68707d4200` | `push 0x427d70` (handler — stays for `0x4F1BC0`) |
| `0x004EECCF` | `e8bc2e0000` | `call 0x4F1B90` (no arguments; returns kind in AL) |
| `0x004EECD4` | `50` | `push eax` (kind low byte = arg1) |
| `0x004EECD5` | `a17018a700` | `mov eax, dword [0xa71870]` (Real32 type handle) |
| `0x004EECDA` | `50` | `push eax` (type = arg0) |
| `0x004EECDB` | `e8e02e0000` | `call 0x4F1BC0` (gets type, kind, handler) |
| `0x004EECE0` | `83c40c` | `add esp, 0xc` (cleans all three) |

So `0x4F1BC0(type, kind, handler)` is right, but the kind getter is a **no-parameter native** and the handler push must be preserved on the stack between the two calls. Transcribing the getter as `uint32 get(uintptr_t)` would be wrong; transcribing it as a zero-argument call whose result is the kind byte is required.

**(b) The six kind slots are assigned at run time, not in the file.** The bytes at `0x0054E0E6`, `E7`, `E8`, `E9`, `EA`, `EB` are all `0xFF` in the image (verified in the PE raw data: `0x0054E0E6` = `ff ff ff ff ff ff`). `0x4F1840` assigns them by calling `0x4F1B20` six times:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F187D` | `e89e020000` | `call 0x4F1B20` |
| `0x004F1882` | `a2ebe05400` | `mov byte [0x54e0eb], al` |
| `0x004F1887` | `e894020000` | `call 0x4F1B20` |
| `0x004F188C` | `a2eae05400` | `mov byte [0x54e0ea], al` |
| `0x004F1891` | `e88a020000` | `call 0x4F1B20` |
| `0x004F1896` | `a2e7e05400` | `mov byte [0x54e0e7], al` |
| `0x004F189B` | `e880020000` | `call 0x4F1B20` |
| `0x004F18A0` | `a2e8e05400` | `mov byte [0x54e0e8], al` |
| `0x004F18A5` | `e876020000` | `call 0x4F1B20` |
| `0x004F18AA` | `a2e9e05400` | `mov byte [0x54e0e9], al` |
| `0x004F18AF` | `e86c020000` | `call 0x4F1B20` |
| `0x004F18B4` | `a2e6e05400` | `mov byte [0x54e0e6], al` |

Allocation order: slot `EB`, then `EA`, `E7`, `E8`, `E9`, `E6`. `0x4F1B20` is the manifest `STRUCTURALLY_VERIFIED` helper; `0x4F1840` also zeroes the 16-byte flag array at `0xA758C8` and the `0x1300`-byte parameter table at `0xA758E0` (see §4). **Consequence:** the "kindE6"/"kindE9" labels in §7.4 are slot identities (the address byte), not kind values. The kind value stored in a slot is whatever `0x4F1B20` returned when `0x4F1840` ran; it must never be hardcoded. The registration index `19*type + (int8)kind` therefore also cannot be enumerated statically — transcribe `0x4F1840`/`0x4F1B20` and read the bytes, exactly as retail does.

**(c) The dispatcher propagates the callback's EAX, but none of its four callers consumes it.** `0x4F1AF0` ends `call dword [edx*4+0xa758e8]` / `add esp,8` / `ret` (`0x4F1B0B`, `0x4F1B12`, `0x4F1B15`), so it returns the handler's EAX. The four call sites, with the first instruction after each call, show the result is overwritten before any use:

| Call site | First instruction after the call |
| --- | --- |
| `0x4880A4` | `0x4880A9` `8b4324` `mov eax, dword [ebx + 0x24]` |
| `0x4882F8` | `0x4882FD` `8b442430` `mov eax, dword [esp + 0x30]` |
| `0x488333` | `0x488338` `83c410` `add esp, 0x10`, then `0x48833B` `8b4324` `mov eax, ...` |
| `0x4F095A` | `0x4F095F` `8bb424f800..` `mov esi, dword [esp + 0xF8]` |

So a candidate handler's EAX-on-return is currently unobservable at every dispatcher call site. It is still worth matching for the constant-writers (§9), because retail returns `value` there and the match is free; but no landed native should be blocked on it.

**(d) These handlers are shared, global callbacks.** `axtj` on the whole image gives `0x4D3710` 163 references: 148 registration `push` instructions, 13 direct `call`s and 2 pointer stores. Only 8 of those are inside this initialiser; 140 registration pushes and all 13 direct calls are outside it. The other five have exactly one reference each — the registration here. The bodies must therefore not assume any type-specific context, and `0x4D3710` in particular must stay a truly empty callback.

### 8.1 The six handlers

Loud caveat first, because it bounds what "transcription-ready" means here: **five of the six are class-method handlers** registered through `0x401530(classWord, methodId, handler)` with `methodId = 2`; the consumer of those class tables (the call site that invokes class methods) was **not located** in this pass. Their bodies read no stack arguments at all, so any argument list is ignored by construction and the emitted body is exact; but their C++ signature and the use (if any) of their EAX return cannot be fixed until `0x401530` and the class-invocation path are transcribed. The sixth, `0x4D3710`, is registered in *both* channels (class method id 1 for Static and Idle, and six attribute kind slots) and is additionally called directly at 13 sites with one pushed argument. Nothing here contradicts the `fn(arg0, value)` dispatcher evidence — the five class methods simply are not reached through it.

| # | VA | size | body | registration (this initialiser) | target global |
| --- | --- | --- | --- | --- | --- |
| 1 | `0x004D3710` | 1 | `ret` only (no-op) | Static.method1, Idle.method1 (`0x401530`, id 1) + attribute slots of VarString (`EB`) and Graph (`EA`,`EB`,`E7`,`E8`,`E6`) | — |
| 2 | `0x004F0610` | 11 | unconditional dword zero-store | Level.method2 (`0x401530`, id 2, class word R1+0x00 `0x0054DD0C`) | `0x00A758A4` (R8+0x00) |
| 3 | `0x004F0640` | 20 | conditional dword zero-store | Static.method2 (`0x401530`, id 2, class word R1+0x02 `0x0054DD0E`) | `0x00A758B0` (R8+0x0C), read then written |
| 4 | `0x004F06F0` | 11 | unconditional dword zero-store | Dynamic.method2 (`0x401530`, id 2, class word R1+0x04 `0x0054DD10`) | `0x00A758AC` (R8+0x08) |
| 5 | `0x004F0710` | 11 | unconditional dword zero-store | Idle.method2 (`0x401530`, id 2, class word R1+0x06 `0x0054DD12`) | `0x00A758B4` (R8+0x10) |
| 6 | `0x004F0730` | 11 | unconditional dword zero-store | CameraBase.method2 (`0x401530`, id 2, class word R1+0x08 `0x0054DD14`) | `0x00A758B8` (R8+0x14) |

Common facts for all six: **no callee calls**, **no string reads**, **no dereference of any argument** (they read no arguments), and none of them returns a value of its own (see each block for the EAX detail). The five globals do not exist in the candidate yet; they are R8 fields (`0x00A758A4`, `0x00A758AC`, `0x00A758B0`, `0x00A758B4`, `0x00A758B8` — §3 R8, currently only reached by untranscribed handlers). They must become new candidate symbols (extern, not file-local); their retail addresses must not appear as constants in candidate code.

### 8.2 `0x004D3710` — no-op callback (class method 1 for Static/Idle + six attribute slots)

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004D3710` | `c3` | `ret` |

**Registrations.** Class channel (2 sites), `0x401530(classWord, 1, 0x4D3710)`:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EEA84` | `66a10edd5400` | `mov ax, word [0x54dd0e]` (Static class word) |
| `0x004EEA8A` | `6810374d00` | `push 0x4d3710` |
| `0x004EEA8F` | `6a01` | `push 1` |
| `0x004EEA91` | `50` | `push eax` |
| `0x004EEA92` | `e8992af1ff` | `call 0x401530` |
| `0x004EEB2D` | `668b1512dd5400` | `mov dx, word [0x54dd12]` (Idle class word) |
| `0x004EEB34` | `6810374d00` | `push 0x4d3710` |
| `0x004EEB39` | `6a01` | `push 1` |
| `0x004EEB3B` | `52` | `push edx` |
| `0x004EEB3C` | `e8ef29f1ff` | `call 0x401530` |

Attribute channel (6 sites), all with the same 6-instruction shape (`push handler` / `call getter` / `push eax` / load type handle / `push` / `call 0x4F1BC0`):

| Site | Getter call | Kind slot | Type handle (field name) | Registration call |
| --- | --- | --- | --- | --- |
| `0x004EF12A` | `0x004EF12F` `0x4F1B80` | `0x0054E0EB` | `0x00A70C9C` (VarString, R3+0x34) | `0x004EF13B` |
| `0x004EF58A` | `0x004EF58F` `0x4F1B60` | `0x0054E0EA` | `0x00A70CA0` (Graph, R3+0x38) | `0x004EF59B` |
| `0x004EF5A3` | `0x004EF5A8` `0x4F1B80` | `0x0054E0EB` | `0x00A70CA0` (Graph) | `0x004EF5B5` |
| `0x004EF5BD` | `0x004EF5C2` `0x4F1BA0` | `0x0054E0E7` | `0x00A70CA0` (Graph) | `0x004EF5CF` |
| `0x004EF5D7` | `0x004EF5DC` `0x4F1BB0` | `0x0054E0E8` | `0x00A70CA0` (Graph) | `0x004EF5E8` |
| `0x004EF60A` | `0x004EF60F` `0x4F1B70` | `0x0054E0E6` | `0x00A70CA0` (Graph) | `0x004EF61C` |

**Exact C++ behaviour:** nothing. The body performs no access, so the emitted native must have an empty body:

```cpp
NATIVE_DECL void <no-op callback>(void) { }
```

**Accesses:** none — no argument read, no global read/write, no callee, no constant.

**-O3 / codegen risks (all three matter here):**
1. **Do not give it a return value.** Retail leaves EAX exactly as the caller had it. `return 0;` would make the dispatcher return 0 instead of `arg0`; an empty body plus `ret` keeps EAX unchanged, which is what retail does. (Currently unobservable per §8.0(c), but this handler is also called directly, so preserving it is free.)
2. **Empty-body folding (linker ICF).** Because the body is a bare `ret`, an identical-code-folding linker may merge this native with another empty stub, collapsing a retail VA identity that the registration tables must reference. Verify the emitted symbol address is the one stored by the registration natives; do not let the build silently alias it.
3. **Arity.** Retail tolerates three call shapes: 1 pushed argument at the 13 direct call sites (`push esi; call 0x4d3710; add esp,4` — e.g. `0x00482783`/`0x00482784`), 2 arguments through the attribute table (`fn(arg0, value)`), and whatever the class system passes. A cdecl empty function with no parameters ignores all of them; do not add parameters that force the caller to push something, and do not clean the stack inside it (retail is caller-clean).

### 8.3 `0x004F0610` — Level class method 2: zero `0x00A758A4`

**Registration —** `0x401530(classWord = *0x0054DD0C, 2, 0x004F0610)`:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EEA6D` | `668b150cdd5400` | `mov dx, word [0x54dd0c]` |
| `0x004EEA74` | `83c440` | `add esp, 0x40` (cleanup of the Level block's earlier pushes) |
| `0x004EEA77` | `6810064f00` | `push 0x4f0610` |
| `0x004EEA7C` | `6a02` | `push 2` |
| `0x004EEA7E` | `52` | `push edx` |
| `0x004EEA7F` | `e8ac2af1ff` | `call 0x401530` |

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F0610` | `c705a458a70000000000` | `mov dword [0xa758a4], 0` |
| `0x004F061A` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004F0610: *(uint32_t*)0xA758A4 = 0   (R8 +0x00, Level object slot)
NATIVE_DECL void <Level class method 2>(void) { g_a758a4 = 0u; }
```

**Accesses:**

| Retail address | Width | Access | Candidate form |
| --- | --- | --- | --- |
| `0x00A758A4` | 4 bytes | write only, value `0` | extern symbol for R8 +0x00, store `0u` |

**Constants / strings / callees:** none. Only the constant `0`.

**-O3 loss risks:**
1. **Store to a global nothing reads yet.** R8 +0x00 is currently written by this handler and by Level's property handler `0x4F0600` (untranscribed), and read by `0x4F0580`, `0x4F0D40`, `0x4F0E90`, `0x4F1080`, `0x4F17C0` (all untranscribed). Because no candidate code reads it today, the store is removable under LTO/dead-store analysis if the symbol is file-local or provably unread. Declare it as an external symbol in the shared globals unit and do not let it become `static`; then a plain store survives -O3.
2. Nothing else: the function touches no argument, calls nothing, and needs no return value (retail leaves EAX untouched).

### 8.4 `0x004F0640` — Static class method 2: conditionally zero `0x00A758B0`

**Registration —** `0x401530(classWord = *0x0054DD0E, 2, 0x004F0640)`:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EEAAA` | `668b150edd5400` | `mov dx, word [0x54dd0e]` |
| `0x004EEAB1` | `6840064f00` | `push 0x4f0640` |
| `0x004EEAB6` | `6a02` | `push 2` |
| `0x004EEAB8` | `52` | `push edx` |
| `0x004EEAB9` | `e8722af1ff` | `call 0x401530` |

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F0640` | `a1b058a700` | `mov eax, dword [0xa758b0]` |
| `0x004F0645` | `85c0` | `test eax, eax` |
| `0x004F0647` | `740a` | `je 0x4f0653` |
| `0x004F0649` | `c705b058a70000000000` | `mov dword [0xa758b0], 0` |
| `0x004F0653` | `c3` | `ret` |

**Exact C++ behaviour (branch-faithful form):**

```cpp
// 0x004F0640: load 0xA758B0; if non-null, store 0; return old value in EAX
NATIVE_DECL void <Static class method 2>(void)
{
    if (g_a758b0 != 0u)
        g_a758b0 = 0u;
}
```

**Accesses:**

| Retail address | Width | Access | Candidate form |
| --- | --- | --- | --- |
| `0x00A758B0` | 4 bytes | read (guard), then write `0` if non-zero | extern symbol for R8 +0x0C; the load is retail and worth keeping |

**Constants / strings / callees:** none. Constant `0` only. Retail writers/readers of `0x00A758B0`: written here and at `0x4F062D` (Static property, `0x4F0620`); read here, at `0x4F0620` and at `0x4F0ED0`. All untranscribed.

**-O3 loss risks (this is the risky one of the six):**
1. **The load can be folded away.** Memory-state-wise the branch is redundant — both paths end with the global equal to 0 — so -O3 may replace the sequence with an unconditional `store 0` (or, with LTO and no readers, delete the store). That is acceptable for memory state, but it loses the retail load, which is the only reason the function reads before writing. If byte-faithful sequencing matters, keep the explicit `if`; the compiler may still fold it, and the memory end-state remains identical either way.
2. **EAX differs between the forms.** Retail returns EAX = old global (0 when the global was already null); the `if` form compiled as a void function leaves EAX unspecified. No caller's use of it is proven (class-invocation path unknown), so this is a documented divergence rather than a blocker — but if the class system is later found to read it, transcribe the return as the old value.
3. **Do not declare the global `static`** (same dead-store risk as §8.3).

### 8.5 `0x004F06F0` — Dynamic class method 2: zero `0x00A758AC`

**Registration —** `0x401530(classWord = *0x0054DD10, 2, 0x004F06F0)`:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EEB19` | `668b0d10dd5400` | `mov cx, word [0x54dd10]` |
| `0x004EEB20` | `68f0064f00` | `push 0x4f06f0` |
| `0x004EEB25` | `6a02` | `push 2` |
| `0x004EEB27` | `51` | `push ecx` |
| `0x004EEB28` | `e8032af1ff` | `call 0x401530` |

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F06F0` | `c705ac58a70000000000` | `mov dword [0xa758ac], 0` |
| `0x004F06FA` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004F06F0: *(uint32_t*)0xA758AC = 0   (R8 +0x08, Dynamic object slot)
NATIVE_DECL void <Dynamic class method 2>(void) { g_a758ac = 0u; }
```

**Accesses:** `0x00A758AC`, 4 bytes, write only, value `0`. Retail also writes this field from Dynamic's property handler `0x4F06E4` (`0x4F06E0`) and reads it at `0x4F0EE0`; both are untranscribed, so in candidate code the field has no reader today. **Constants / strings / callees:** none besides the constant `0`.
**-O3 loss risks:** identical to §8.3 (external-symbol requirement; no argument/return concerns; EAX untouched by retail).

### 8.6 `0x004F0710` — Idle class method 2: zero `0x00A758B4`

**Registration —** `0x401530(classWord = *0x0054DD12, 2, 0x004F0710)`:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EEB53` | `668b0d12dd5400` | `mov cx, word [0x54dd12]` |
| `0x004EEB5D` | `6810074f00` | `push 0x4f0710` |
| `0x004EEB62` | `6a02` | `push 2` |
| `0x004EEB64` | `51` | `push ecx` |
| `0x004EEB65` | `e8c629f1ff` | `call 0x401530` |

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F0710` | `c705b458a70000000000` | `mov dword [0xa758b4], 0` |
| `0x004F071A` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004F0710: *(uint32_t*)0xA758B4 = 0   (R8 +0x10, Idle object slot)
NATIVE_DECL void <Idle class method 2>(void) { g_a758b4 = 0u; }
```

**Accesses:** `0x00A758B4`, 4 bytes, write only, value `0`. Note this field has **no reader anywhere in the image** (§3 R8), so its only observable effect is the store itself — which makes the external-symbol requirement in §8.3 point 1 mandatory, otherwise -O3 can delete the last observable effect of the native.
**Constants / strings / callees:** none besides the constant `0`.

### 8.7 `0x004F0730` — CameraBase class method 2: zero `0x00A758B8`

**Registration —** `0x401530(classWord = *0x0054DD14, 2, 0x004F0730)`:

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EEB90` | `668b0d14dd5400` | `mov cx, word [0x54dd14]` |
| `0x004EEB97` | `6830074f00` | `push 0x4f0730` |
| `0x004EEB9C` | `6a02` | `push 2` |
| `0x004EEB9E` | `51` | `push ecx` |
| `0x004EEB9F` | `e88c29f1ff` | `call 0x401530` |

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F0730` | `c705b858a70000000000` | `mov dword [0xa758b8], 0` |
| `0x004F073A` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004F0730: *(uint32_t*)0xA758B8 = 0   (R8 +0x14, CameraBase object slot)
NATIVE_DECL void <CameraBase class method 2>(void) { g_a758b8 = 0u; }
```

**Accesses:** `0x00A758B8`, 4 bytes, write only, value `0`, also with **no reader anywhere in the image** (same caveat as §8.6).
**Constants / strings / callees:** none besides the constant `0`.

### 8.8 Landing checklist for the six

| Handler | Landing risk | Requires before it can be *called* |
| --- | --- | --- |
| `0x004D3710` | empty body must stay empty; ICF alias check; arity-tolerant | nothing for the body; registration natives `0x401530`/`0x4F1B60`–`0x4F1BC0` for wiring |
| `0x004F0610` | external symbol for `0xA758A4` (dead-store risk) | `0x401530` + class invocation path |
| `0x004F0640` | external symbol for `0xA758B0`; load may fold; EAX = old value divergence | `0x401530` + class invocation path |
| `0x004F06F0` | external symbol for `0xA758AC` | `0x401530` + class invocation path |
| `0x004F0710` | external symbol for `0xA758B4`; field has no readers — store must not be elided | `0x401530` + class invocation path |
| `0x004F0730` | external symbol for `0xA758B8`; field has no readers — store must not be elided | `0x401530` + class invocation path |

None of the six can be exercised until `0x401530` and the class-invocation path are transcribed; the bodies are nevertheless exact and can land now. The thirteen `[esp+8]`-only handlers follow in §9.

---

## 9. APPENDED — transcription sheet: the thirteen `[esp+8]`-only handlers

Second half of the sheet requested by the Lead: the thirteen handlers that read **only** `[esp+8]`, the dispatcher's second callback argument (`value`, i.e. `0x4F1AF0`'s 4th argument). Same rules as §8: sheet only, no `engine/` write, no manifest/listing/git change.

### 9.0 Verdict — all thirteen fit the two-argument model; nothing here threatens the dispatcher evidence

Every one of the thirteen is the same 11-byte, 3-instruction, single-basic-block function:

| # | VA | bytes | instruction |
| --- | --- | --- | --- |
| — | `<VA>` | `8b442408` | `mov eax, dword [esp + 8]` (read `value`, 4 bytes) |
| — | `<VA>+4` | `c700<imm32>` | `mov dword [eax], <imm32>` (write 4 bytes through `value`) |
| — | `<VA>+0xA` | `c3` | `ret` |

Facts that make this group the cleanest in the set:
- **One argument read only.** `[esp+8]` is read once; `[esp+4]` (`arg0`) is never read by any of the thirteen; no third argument is read; no handler writes any incoming stack slot. The dispatcher pushes exactly two arguments (`push value; push arg0` at `0x4F1B09`/`0x4F1B0A`), so the two-argument model is confirmed again, from the callee side.
- **Exactly one dereference, 4 bytes wide, write-only.** No handler reads memory through `value`; there is no byte/word/dword ambiguity to flag. The only memory effects in the whole group are these single dword stores.
- **No global, no string, no callee.** No handler in this group touches static data or calls anything. The only constant is the immediate stored.
- **No direct call sites anywhere in the image; two registration roles.** `axtj` reports zero `call` references to each of the thirteen; they are referenced only by `push` instructions. One set is the attribute registrations inside `0x4EE970` (this sheet's scope, reachable through table `0x00A758E8` / `0x4F1AF0`). The other set is 31 registrations outside the initialiser, and every one of them is a **class-property registration** `0x401400(classWord, stateByte, handler)` whose state byte comes from a helper called with the handler (`0x4C56E0` ×16, `0x416810` ×5, `0x413B30` ×4, `0x4F4110`/`0x4E5360` ×2 each, `0x477030`/`0x4EE0F0` ×1 each): e.g. `0x4EF8B0` at `0x43F9C7` inside the `Fence` declaration function `0x43F900`, `0x51BAA0` at `0x4266EE` inside the `CarDoor` declaration `0x426630`, `0x427D70` at `0x4278EE` inside the `RocketPod` declaration `0x427830`. `0x4EF850` and `0x4EF9D0` have no outside references. So the same bodies are also shared class-property callbacks elsewhere; that channel's invocation ABI is the same open Channel 2 as §8.1, but because these bodies read only `[esp+8]` they ignore everything except their second argument under either channel.
- **EAX on return = `value`.** `mov eax,[esp+8]` leaves the pointer in EAX and the store does not change EAX, so retail returns `value` (the dispatcher passes that through). Per §8.0(c) no dispatch caller consumes it today; matching it is still free by returning `value`.

**LOUD: no handler in this group needs an exception or reinterpretation of the model.** There is no "does not fit" item to act on for the thirteen: every one reads only `[esp+8]`, writes 4 bytes through it, and does nothing else. The only open ABI item left anywhere in this sheet is the class channel — class *method* invocation for the six in §8.1, and class *property* invocation for the outside role described in the third bullet above.

### 9.1 Canonical transcription (one native per VA)

```cpp
// Shape of all thirteen; substitute the VA and the stored constant.
// Native: <retail-derived name>; retail VA 0x00XXXXXX
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;                                        // retail never reads [esp+4]
    *reinterpret_cast<std::uint32_t*>(value) = <CONST>; // 4-byte store, no read-back
    return value;                                      // retail leaves EAX = value
}
```

- ABI: `cdecl`, two 4-byte arguments, caller cleans 8 bytes (`0x4F1B12 add esp, 8`). Keep both parameters even though `arg0` is unused: with a one-parameter form `value` would sit at `[esp+4]` and the emitted frame would no longer match retail's `[esp+8]` read — the frame offsets are themselves part of the evidence.
- `-O3`: the store goes through an opaque, escaped pointer, so it cannot be deleted, reordered with an observable effect, or turned into a no-op. Unlike the six in §8 there is no dead-store, dead-load or folding risk here. The only codegen choice to make is `void` vs returning `value`; `void` loses nothing observable today (§8.0(c)) but returning `value` is exactly retail.
- Do not merge or macro-collapse the thirteen: rule 10.15(13)/10.13(2) requires one native per retail VA, and the retail bodies differ only in the immediate — but they are distinct functions with distinct callers in the table.


### 9.2 The thirteen, with constants and registrations

| # | VA | size | stored 4-byte value | registrations in `0x4EE970` (type + kind slot) |
| --- | --- | --- | --- | --- |
| 1 | `0x00427D70` | 11 | `4` | Real32 (`E9`), Int32 (`E9`) |
| 2 | `0x00427F80` | 11 | `8` | Real64 (`E9`) |
| 3 | `0x004EF850` | 11 | `0x41` | QTaskNote (`E9`) |
| 4 | `0x004EF8B0` | 11 | `0xB` | GraphEnvelope (`E6`) |
| 5 | `0x004EF9D0` | 11 | `0x2C` | GraphEnvelope (`E9`) |
| 6 | `0x004EFA10` | 11 | `1` | Real32, Real64, Int32, Int16, VarString, String16, String32, String256, LevelExpression, QTaskNote, bool8 (`E6`) + bool8 (`E9`) |
| 7 | `0x004EFB00` | 11 | `3` | Quat32, Real32x3, Real32x9, Real64x3 (`E6`) |
| 8 | `0x004EFCD0` | 11 | `0x28` | Real32x9 (`E9`) |
| 9 | `0x004EFF20` | 11 | `0x14` | Quat32 (`E9`) |
| 10 | `0x004F00A0` | 11 | `0x18` | Real64x3 (`E9`) |
| 11 | `0x004F01B0` | 11 | `2` | Int16 (`E9`) |
| 12 | `0x004F64E0` | 11 | `0` | Graph (`E9`) |
| 13 | `0x0051BAA0` | 11 | `0xC` | Real32x3 (`E9`) |

**Verification note for row 4 (`0x004EF8B0`), where a naive parse of the listing mis-assigns the type:** the registration instructions are `push 0x4ef8b0` at `0x004EF6B5`, `call 0x4F1B70` at `0x004EF6BA`, `mov edx, dword [0x00A70C80]` at `0x004EF6BF`, `push eax` at `0x004EF6C5`, `push edx` at `0x004EF6C6`, `call 0x4F1BC0` at `0x004EF6C7`. The type handle is `0x00A70C80` = GraphEnvelope, and the `mov eax, dword [0x00A7186C]` (String256) at `0x004EF6CC` belongs to the **next** registration block. The committed §7.4 row 11 and Appendix A record (`GraphEnvelope.kindE6`) are therefore correct; only this sheet's per-handler table below needs to be read with that order in mind.

**Hypothesis, clearly separated from evidence:** the `E9` immediates look like byte sizes of the registered types (Int16 2, Int32/Real32 4, Real64 8, Quat32 0x14, Real32x3 0xC, Real32x9 0x28, Real64x3 0x18, graph envelope 0x2C, Graph 0). That reading is *not proven* — `E6` does not share it (it stores 1, 3 and 0xB) — so do not name these natives from it. The sheet only needs the immediates.

### 9.3 Per-handler blocks

#### 9.3.1 `0x00427D70` — stores `4` (`4u`); registered for Int32 and Real32

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x00427D70` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x00427D74` | `c70004000000` | `mov dword [eax], 4` |
| `0x00427D7A` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x00427D70: *reinterpret_cast<std::uint32_t*>(value) = 4u;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 4u;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EECCA` | `0x004EECCF` `0x4f1b90` | `E9` | `0x00A71870` Real32 | `0x004EECDB` |
| `0x004EF021` | `0x004EF026` `0x4f1b90` | `E9` | `0x00A70C7C` Int32 | `0x004EF032` |

**Constants / strings / globals:** the only constant is the stored `4`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.2 `0x00427F80` — stores `8` (`8u`); registered for Real64

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x00427F80` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x00427F84` | `c70008000000` | `mov dword [eax], 8` |
| `0x00427F8A` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x00427F80: *reinterpret_cast<std::uint32_t*>(value) = 8u;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 8u;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EEECB` | `0x004EEED0` `0x4f1b90` | `E9` | `0x00A71874` Real64 | `0x004EEEDC` |

**Constants / strings / globals:** the only constant is the stored `8`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.3 `0x004EF850` — stores `0x41` (`0x41u`); registered for QTaskNote

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EF850` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x004EF854` | `c70041000000` | `mov dword [eax], 0x41` |
| `0x004EF85A` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004EF850: *reinterpret_cast<std::uint32_t*>(value) = 0x41u;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 0x41u;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EF49A` | `0x004EF49F` `0x4f1b90` | `E9` | `0x00A75894` QTaskNote | `0x004EF4AC` |

**Constants / strings / globals:** the only constant is the stored `0x41`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.4 `0x004EF8B0` — stores `0xB` (`0xBu`); registered for GraphEnvelope

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EF8B0` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x004EF8B4` | `c7000b000000` | `mov dword [eax], 0xB` |
| `0x004EF8BA` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004EF8B0: *reinterpret_cast<std::uint32_t*>(value) = 0xBu;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 0xBu;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EF6B5` | `0x004EF6BA` `0x4f1b70` | `E6` | `0x00A70C80` GraphEnvelope | `0x004EF6C7` |

**Constants / strings / globals:** the only constant is the stored `0xB`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.5 `0x004EF9D0` — stores `0x2C` (`0x2Cu`); registered for GraphEnvelope

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EF9D0` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x004EF9D4` | `c7002c000000` | `mov dword [eax], 0x2C` |
| `0x004EF9DA` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004EF9D0: *reinterpret_cast<std::uint32_t*>(value) = 0x2Cu;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 0x2Cu;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EF69B` | `0x004EF6A0` `0x4f1b90` | `E9` | `0x00A70C80` GraphEnvelope | `0x004EF6AD` |

**Constants / strings / globals:** the only constant is the stored `0x2C`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.6 `0x004EFA10` — stores `1` (`1u`); registered for twelve registrations

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EFA10` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x004EFA14` | `c70001000000` | `mov dword [eax], 1` |
| `0x004EFA1A` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004EFA10: *reinterpret_cast<std::uint32_t*>(value) = 1u;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 1u;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EECE3` | `0x004EECE8` `0x4f1b70` | `E6` | `0x00A71870` Real32 | `0x004EECF5` |
| `0x004EEEE4` | `0x004EEEE9` `0x4f1b70` | `E6` | `0x00A71874` Real64 | `0x004EEEF6` |
| `0x004EF03A` | `0x004EF03F` `0x4f1b70` | `E6` | `0x00A70C7C` Int32 | `0x004EF04C` |
| `0x004EF0E5` | `0x004EF0EA` `0x4f1b70` | `E6` | `0x00A70C78` Int16 | `0x004EF0F7` |
| `0x004EF190` | `0x004EF195` `0x4f1b70` | `E6` | `0x00A70C9C` VarString | `0x004EF1A2` |
| `0x004EF23B` | `0x004EF240` `0x4f1b70` | `E6` | `0x00A71884` String16 | `0x004EF24D` |
| `0x004EF2E6` | `0x004EF2EB` `0x4f1b70` | `E6` | `0x00A71888` String32 | `0x004EF2F8` |
| `0x004EF391` | `0x004EF396` `0x4f1b70` | `E6` | `0x00A7186C` String256 | `0x004EF3A3` |
| `0x004EF43C` | `0x004EF441` `0x4f1b70` | `E6` | `0x00A7188C` LevelExpression | `0x004EF44E` |
| `0x004EF4B4` | `0x004EF4B9` `0x4f1b70` | `E6` | `0x00A75894` QTaskNote | `0x004EF4C6` |
| `0x004EF545` | `0x004EF54A` `0x4f1b90` | `E9` | `0x00A70C90` bool8 | `0x004EF557` |
| `0x004EF55F` | `0x004EF564` `0x4f1b70` | `E6` | `0x00A70C90` bool8 | `0x004EF571` |

**Constants / strings / globals:** the only constant is the stored `1`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.7 `0x004EFB00` — stores `3` (`3u`); registered for Quat32, Real32x3, Real32x9, Real64x3

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EFB00` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x004EFB04` | `c70003000000` | `mov dword [eax], 3` |
| `0x004EFB0A` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004EFB00: *reinterpret_cast<std::uint32_t*>(value) = 3u;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 3u;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EEC38` | `0x004EEC3D` `0x4f1b70` | `E6` | `0x00A71878` Quat32 | `0x004EEC4A` |
| `0x004EED8E` | `0x004EED93` `0x4f1b70` | `E6` | `0x00A70C68` Real32x3 | `0x004EEDA0` |
| `0x004EEE39` | `0x004EEE3E` `0x4f1b70` | `E6` | `0x00A70C6C` Real32x9 | `0x004EEE4B` |
| `0x004EEF8F` | `0x004EEF94` `0x4f1b70` | `E6` | `0x00A75890` Real64x3 | `0x004EEFA1` |

**Constants / strings / globals:** the only constant is the stored `3`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.8 `0x004EFCD0` — stores `0x28` (`0x28u`); registered for Real32x9

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EFCD0` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x004EFCD4` | `c70028000000` | `mov dword [eax], 0x28` |
| `0x004EFCDA` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004EFCD0: *reinterpret_cast<std::uint32_t*>(value) = 0x28u;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 0x28u;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EEE20` | `0x004EEE25` `0x4f1b90` | `E9` | `0x00A70C6C` Real32x9 | `0x004EEE31` |

**Constants / strings / globals:** the only constant is the stored `0x28`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.9 `0x004EFF20` — stores `0x14` (`0x14u`); registered for Quat32

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004EFF20` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x004EFF24` | `c70014000000` | `mov dword [eax], 0x14` |
| `0x004EFF2A` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004EFF20: *reinterpret_cast<std::uint32_t*>(value) = 0x14u;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 0x14u;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EEC1F` | `0x004EEC24` `0x4f1b90` | `E9` | `0x00A71878` Quat32 | `0x004EEC30` |

**Constants / strings / globals:** the only constant is the stored `0x14`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.10 `0x004F00A0` — stores `0x18` (`0x18u`); registered for Real64x3

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F00A0` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x004F00A4` | `c70018000000` | `mov dword [eax], 0x18` |
| `0x004F00AA` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004F00A0: *reinterpret_cast<std::uint32_t*>(value) = 0x18u;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 0x18u;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EEF76` | `0x004EEF7B` `0x4f1b90` | `E9` | `0x00A75890` Real64x3 | `0x004EEF87` |

**Constants / strings / globals:** the only constant is the stored `0x18`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.11 `0x004F01B0` — stores `2` (`2u`); registered for Int16

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F01B0` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x004F01B4` | `c70002000000` | `mov dword [eax], 2` |
| `0x004F01BA` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004F01B0: *reinterpret_cast<std::uint32_t*>(value) = 2u;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 2u;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EF0CC` | `0x004EF0D1` `0x4f1b90` | `E9` | `0x00A70C78` Int16 | `0x004EF0DD` |

**Constants / strings / globals:** the only constant is the stored `2`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.12 `0x004F64E0` — stores `0` (`0u`); registered for Graph

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x004F64E0` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x004F64E4` | `c70000000000` | `mov dword [eax], 0` |
| `0x004F64EA` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x004F64E0: *reinterpret_cast<std::uint32_t*>(value) = 0u;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 0u;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EF5F0` | `0x004EF5F5` `0x4f1b90` | `E9` | `0x00A70CA0` Graph | `0x004EF602` |

**Constants / strings / globals:** the only constant is the stored `0`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.

#### 9.3.13 `0x0051BAA0` — stores `0xC` (`0xCu`); registered for Real32x3

**Retail body — complete:**

| VA | bytes | instruction |
| --- | --- | --- |
| `0x0051BAA0` | `8b442408` | `mov eax, dword [esp + 8]` |
| `0x0051BAA4` | `c7000c000000` | `mov dword [eax], 0xC` |
| `0x0051BAAA` | `c3` | `ret` |

**Exact C++ behaviour:**

```cpp
// 0x0051BAA0: *reinterpret_cast<std::uint32_t*>(value) = 0xCu;  (single 4-byte store)
NATIVE_DECL std::uintptr_t <name>(std::uintptr_t arg0, std::uintptr_t value)
{
    (void)arg0;
    *reinterpret_cast<std::uint32_t*>(value) = 0xCu;
    return value;
}
```

**Registrations** (`push handler` → `call getter` → `push eax` → load type handle → `call 0x4F1BC0`):

| `push` site | getter call, getter | kind slot | type handle, type | `0x4F1BC0` call |
| --- | --- | --- | --- | --- |
| `0x004EED75` | `0x004EED7A` `0x4f1b90` | `E9` | `0x00A70C68` Real32x3 | `0x004EED86` |

**Constants / strings / globals:** the only constant is the stored `0xC`; no string is read, no static data is read or written.

**Callees:** none.

**Accesses:** one 4-byte store through `value` (`[esp+8]`); no read through it; `arg0` (`[esp+4]`) unused; no stack write.

**-O3 risks:** none for the store (opaque escaped pointer). Only the return-type choice in 9.1 applies: retail EAX = `value`.


### 9.4 Landing notes for the thirteen

- **All thirteen are body-complete and self-contained:** no new globals, no strings, no callees, no dependencies on untranscribed data. They can land as soon as there is a place for them; the attribute-table role cannot be *invoked* until `0x4F1AF0`, `0x4F1BC0`, the six getters `0x4F1B60`–`0x4F1BB0`, and the kind-slot initialiser (`0x4F1840` + `0x4F1B20`) are transcribed. Their second, class-property role (above) additionally needs `0x401400` and the class-invocation path — the same open Channel 2 item as the six in §8. Neither role depends on `0x401530` (class *method* registration), only the six do.
- **One native per VA, thirteen files or thirteen functions** — whichever the Lead's layout uses, no merging (their bytes differ only in the immediate, but they are distinct retail functions addressed by distinct table slots).
- **Registration natives needed for them to be reachable:** `0x4F1BC0` (`0x4F1BC0(type, kind, handler)` — note the handler arrives as the third argument, per §8.0(a)) and the getters; plus `0x4F1840`/`0x4F1B20` so the kind slots hold real kind values before any registration runs.
- **Do not hardcode the kind numbers.** Slots `E6`/`E9` are addresses (`0x0054E0E6`/`0x0054E0E9`); the values inside are assigned at run time by `0x4F1840` (§8.0(b)). The registration index `19*type + (int8)kind` must be computed from those runtime bytes exactly as `0x4F1BC0` does.
- **Count sanity for this file (definitions given, from the committed listing):** the initialiser contains 443 `push` instructions, of which 176 carry an immediate operand: 119 pushes of code pointers (84 distinct = the handler set), 32 pushes of type-name string pointers, 1 push of the `"RGB"` pointer `0x0054DF10`, and 24 pushes of small/negative integers (19 × `-1`, 4 × `0x20`, 1 × `0x38`). It contains 262 `call` instructions to 17 distinct local targets and 40 distinct absolute memory operands. For citation, use these explicit definitions: §7.1's "139 `push` instructions" is the subset `119 code-pointer pushes + 19 × push -1 + 1 × push "RGB"` (139 exactly — small-integer pushes and the 32 type-name pushes are excluded), and §1/§6's "127 distinct absolute references" is the earlier loose aggregate over code+string+data operands; neither is a contradicting count, but the definitions above are the ones that can be re-derived one-to-one from the listing. The 84-handler set is set-equal under every parse.

## 10. APPENDED — the extent of `0x00A96AE0`, proven from its writers

### 10.0 The question and the method

The question is the storage definition of the dword table at `0x00A96AE0`: how many elements, how many bytes, and which retail instruction proves it. The instruction must not be a reader: `0x4014E0` multiplies a byte of the 16-byte map at `0x54FF24` by `0x180` and masks the other index with `0xffff`, so a *reader* would "require" a table of `255*0x180 + 0xffff + 1` dwords (`0x27E80` elements, `0x9FA00` bytes) if the map ever held a non-sentinel byte above `0xFD`. That number is an arithmetic artefact, not storage.

Method: enumerate every reference to the global (`axtj @ 0x00a96ae0` returns 588 references; an independent raw `disp32` scan of `.text` for the displacement bytes `e0 6a a9 00` returns the same 588 sites, so the two enumerations agree), then classify each site by the instruction that consumes the displacement, and inspect every site that forms a pointer into the region or performs a store. This section uses **only** instructions that write (directly, or through a pointer this code forms from the base); readers are quoted once, in §10.8, and only to explain where Lead's sentinel concern came from.

### 10.1 The extent, stated

**The table occupies `[0x00A96AE0, 0x00AF5EE0)`: 97,536 dwords = 390,144 bytes = `0x17D00` elements = `0x5F400` bytes.**

It is a two-dimensional array of 32-bit handler pointers: **254 phases (rows, index 0..253) × 384 classes (columns, index 0..383)**, row stride `0x600` bytes (`0x180` dwords), element index `class_id + phase * 0x180`. `254` is the row *capacity* (proved by the copy/zero loop count `0xFE` in §10.4); the phase ids actually passed by the observed call sites are 0..18 (§11.3), so the used rows are a prefix of the capacity, not the whole of it. The array ends exactly at the task-type table base `0xAF5EE0`, i.e. the handler table and the type table are two adjacent objects with no gap.

**The instruction that proves it** is the array's own zero-fill inside `QTask_Init` (`0x004010D0`, `0x0040117B`–`0x0040118F`):

| VA | bytes | instruction | role in the proof |
| --- | --- | --- | --- |
| `0x0040117B` | `33 c0` | `xor eax, eax` | fill value 0 |
| `0x0040117D` | `b9 00 7d 01 00` | `mov ecx, 0x17d00` | **element count = 97,536** |
| `0x00401183` | `bf e0 6a a9 00` | `mov edi, 0xa96ae0` | **destination = table base** |
| `0x0040118F` | `f3 ab` | `rep stosd dword es:[edi], eax` | **writes `ECX` dwords from `EDI` upward** |

`rep stosd` writes exactly `ECX` dwords (`0x17D00`), four bytes each, to consecutive addresses starting at `EDI`. Count and destination are literals of the instruction stream, not values computed from any reader; maximum element written = `0x17D00 - 1 = 0x17CFF`, last address `0xA96AE0 + 0x17CFC = 0xAF5EDC`, first address past the array `0xAF5EE0`. No `UNKNOWN` remains and no further observation is needed to settle the size.

### 10.2 The upper boundary is independently the next object

`0xAF5EE0` — the first address past the array — is the base of the task-type table, proved without reference to `0xA96AE0`:

- `QTask_ClearTaskTypes` `0x004011B0`: `mov eax, 0xaf5ee0` (`0x004011B0`), then the loop `0x004011B7`–`0x004011C4`: `mov byte [eax + 0x25], cl` / `mov dword [eax], ecx` / `add eax, 0x30` / `cmp eax, 0xafa6e0` / `jl 0x004011B7`. Base `0xAF5EE0`, stride `0x30`, end `0xAFA6E0` → 384 slots, extent `[0xAF5EE0, 0xAFA6E0)` = `0x4800` bytes.
- The three functions that address records confirm the same base: `0x401900` writes name/size/parent at `0xAF5EE0`/`0xAF5EE4`/`0xAF5EE8` (`0x0040193C`–`0x00401957`); `0x4014B0` writes the mask word at `0xAF5F08` = base`+0x28`; `0x401530`/`0x401400` compare the parent word at `0xAF5EE8` = base`+0x08` (their walk cursor) and read the in-use byte at `0xAF5F05` = base`+0x25`.

`0xA96AE0 + 0x5F400 = 0xAF5EE0` exactly. The zero-fill's extent and the adjacency therefore agree to the byte.

### 10.3 The lower boundary is independently the previous object

Immediately below the table sits a 256-byte byte map at `0xA969E0`:

- `0x401D30`: `and eax, 0xff` at `0x00401D3E`, then `mov byte [eax + 0xa969e0], cl` at `0x00401D43` → maximum byte address `0xA96ADF`, so that object is exactly `[0xA969E0, 0xA96AE0)`.
- `QTask_Init` initialises the same region: `mov ecx, 0x3f` / `mov edi, 0xa969e0` / `or eax, -1` / `rep stosd` (`0x004010FB`–`0x00401120`), then the `stosw` at `0x0040112E` writes the map's final word at `0xA96ADC`–`0xA96ADD` (the last two bytes, `0xA96ADE`–`0xA96ADF`, stay zero).

So the table's low boundary is `0xA96AE0` and its high boundary is `0xAF5EE0`; the neighbouring objects fix both ends and neither of them is touched by the `0x17D00`-dword fill.

### 10.4 A second, independent writer proof of the same geometry

`TaskType_Register` (`0x00401900`) initialises one table column per new class. After it has chosen the parent path (`cmp di, 0x181` at `0x004019AD`), it runs one of two loops:

| VA | instruction | meaning |
| --- | --- | --- |
| `0x004019BA` | `lea ecx, [ebp*4 + 0xa96ae0]` | destination = column `ebp` (the newly allocated class index) |
| `0x004019C1` | `mov edx, 0xfe` | **row count = 254** |
| `0x004019C6` | `lea eax, [edi*4 + 0xa96ae0]` | source = column `edi = parent_type & 0xffff` (copy path only) |
| `0x004019CD` | `mov esi, [eax]` | read parent's element for this row |
| `0x004019CF` | `add eax, 0x600` | **row stride = 0x600 bytes = 384 dwords** |
| `0x004019D4` | `mov [ecx], esi` | write the child's element |
| `0x004019D6` | `add ecx, 0x600` | next row |
| `0x004019DC` | `dec edx` / `0x004019DD jne 0x004019cd` | loop 254 times |

and the root path at `0x004019EB`–`0x00401A03` is the same loop with `mov dword [eax], 0` (zero the column) instead of the copy. The column index `ebp` comes from the free-slot scan in the same function: `mov eax, 0xaf5f05`, `add eax, 0x30`, `cmp eax, 0xafa705` → `ebp` ∈ `0..383`. Since 384 slots exist, the highest element any column loop can touch is row 253, column 383: `253*0x180 + 383 = 0x17CFF`, the last dword of the array — the same extent as §10.1, reached by a different writer.

### 10.5 The two semantic writers, with the maximum index each can produce from its own bounds

| # | function | store site | instruction | index as the instruction computes it |
| --- | --- | --- | --- | --- |
| W3 | `0x401530` | `0x0040156E` | `mov dword [eax*4 + 0xa96ae0], edx` | `eax = (arg1 & 0xffff) + (arg2*3 << 7)` where the shift is a 32-bit multiply by `0x180`; `arg2` is **not masked** |
| W4 | `0x401400` | `0x00401444` | `mov dword [eax*4 + 0xa96ae0], edx` | `eax = (arg1 & 0xffff) + (arg2 & 0xff) * 0x180` |

- **W3's own bounds:** `and eax, 0xffff` at `0x00401553` bounds `arg1` to `0xFFFF`; the phase path is `lea esi, [ebp + ebp*2]` (`0x00401558`) then `shl esi, 7` (`0x00401562`), and `inc`/`shl` do not bound the value. The instruction therefore gives **no finite bound** on the phase: for `arg2 >= 0x100_0000` the wrap-around even loses the row distinction. Only the argument's *domain* (a phase id, §11.3) bounds it.
- **W4's own bounds:** both masks are present, so the instruction-level maximum is `0xFFFF + 0xFF*0x180 = 0x27E7F` (163,455) — larger than the proved capacity `0x17CFF` (97,535) by 65,920 elements.
- Both writers store a single dword and (immediately after) one flag byte at the same index; neither is the defining writer.

Consequence: **the extent cannot be derived from the registrars at all** — their own masks over-run the array if a caller passes an out-of-domain phase or class id, which is precisely why the extent has to come from the array's own initialiser and from the class allocator's column loops (§10.1, §10.4). This is a statement about writer instructions, not an inference from a reader.

### 10.6 Complete writer census — and the answer to "is there a third owner?"

The 588 references to `0x00A96AE0` in `.text` classify exactly as follows:

| class | count | sites |
| --- | --- | --- |
| direct dword store | 2 | `0x00401444` (W4, in `0x401400`), `0x0040156E` (W3, in `0x401530`) |
| base-pointer formation feeding a store | 4 | `mov edi, 0xa96ae0` at `0x00401183` (W0, `QTask_Init`), `lea ecx, [ebp*4 + 0xa96ae0]` at `0x004019BA`, `lea eax, [edi*4 + 0xa96ae0]` at `0x004019C6`, `lea eax, [ebp*4 + 0xa96ae0]` at `0x004019EB` (W1/W2, `TaskType_Register`) |
| indirect dispatch `call dword [reg*4 + 0xa96ae0]` | 28 | `call dword [ecx*4 + 0xa96ae0]` ×17, `[edx*4 + 0xa96ae0]` ×8, other registers ×3; first sites `0x004081A8`, `0x00408265`, `0x00408413`, `0x00413D9F`, `0x004146D2`, `0x004147A2` |
| dword load (read) | 554 | handler lookups; the largest single shapes are `mov eax, [eax*4 + 0xa96ae0]` ×205, `[ecx*4 + ...]` ×125, `[edx*4 + ...]` ×89 |

So the answer to the Lead's question is: **yes, there are writers outside the two registrars — two more, in two more functions — but they are not a third owner of the semantics.**

- **W0 = `QTask_Init` (`0x004010D0`) at `0x0040118F`**: startup zero-fill of the entire array (the §10.1 proving instruction).
- **W1/W2 = `TaskType_Register` (`0x00401900`) at `0x004019CD`–`0x004019DD` / `0x004019F7`–`0x00401A03`**: per-class column copy (inheritance) or column zero (root), 254 dwords.
- **W3/W4 = the registrars**: the only stores of a handler *value*; the only stores that ever put a non-zero dword into the array.

There is no fifth writer and no other function that forms a pointer into the region: the remaining 554 references are loads and 28 are indirect calls. The region is written exactly by the task-type subsystem and by nothing else.

### 10.7 Why the registrars are still the right place to look for the *meaning* of an entry

The array is the phase → per-class handler dispatch table: `TaskType_Register` seeds each new class's column from its parent's column (or zero for the root word `0x0181`), `0x401530`/`0x401400` store one handler per (class, phase) and propagate it down the class tree, `0x4014E0` reads it to build a class's 16-bit phase mask, and the dispatcher `call dword [reg*4 + 0xa96ae0]` invokes it. §10.5 fixes the exact store arithmetic; §11 derives W3 statement by statement.

### 10.8 `0x0054FF24` — image state versus runtime state (the sentinel question)

Lead's sentinel concern is a statement about **runtime**, not about the image:

- Image bytes at `0x54FF24`: 16 zero bytes (`00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00`).
- `QTask_Init` overwrites all sixteen bytes with `0xFF` before any registration: `or eax, -1` at `0x004010F8`; `mov [0x54ff24], eax` at `0x00401100`; `mov [0x54ff28], eax` at `0x0040110A`; `mov [0x54ff2C], eax` at `0x00401111`; `mov [0x54ff30], eax` at `0x00401118`. So at runtime every slot is `0xFF` until assigned.
- `0x401D30` sets a slot (`mov byte [ecx + 0x54ff24], al` at `0x00401D38`) and `0x401D50` clears one (`mov byte [ecx + 0x54ff24], 0xff` at `0x00401D66`). The value stored is a *phase id* byte, masked to 8 bits, no range validation.
- The reader `0x4014E0` guards with `cmp dl, 0xff` / `je skip` (`0x004014EB`–`0x004014F3`), so the `0xFF` fill produces no table access.

The reader's worst case is still real but bounded by the map's contract: if a slot ever held `0xFE` or higher (other than `0xFF`), `0x4014E0` would read `phase*0x180 + type_id` with `phase >= 254`, i.e. **at or past `0x17D00` dwords** — the first read of the type table. The map's only writers store phase ids (`0x401D30`'s argument is the caller's phase byte), and every observed phase id is small (0..18, §11.3), so the contract holds; the code performs no check of its own. This is recorded as a residual boundary hazard, not as evidence about the table's size.

### 10.9 The parallel first-link flag array `0x0054FF34`

- Written by exactly two instructions, at the same composed index as W3/W4: `0x00401575` / `0x0040144B`, `mov byte [eax + 0x54ff34], cl`. Read at the same index shape: `0x00401593` / `0x00401469`, `mov al, byte [esi + ebx + 0x54ff34]`. Four references total.
- Element is one byte per table element, so the required extent is the same element count: `0x17D00` bytes = 97,536 bytes, `[0x54FF34, 0x567C34)`.
- The image holds zeros and no initialiser writes this array (the only writers are the two registrars), so every flag starts at `0` at runtime.
- The end `0x567C34` is not independently pinned by an adjacent object (the next known global block starts at `0x567C38`, four dwords later), so the byte extent is derived from the shared index space, not from adjacency. It is nevertheless exact: the two writers cannot address more than `0x17D00` elements without writing outside the table they index in the same instruction pair.
- Current port state: `task_types.cpp` sizes `QTask_FirstLinkFlag` as `384 * 384 = 147,456` bytes (and `QTask_HandlerTable` as 147,456 `FrameHandler` entries). The retail extent for both is `0x17D00` elements (`254 * 384`); the port is over-sized by `0xC300` elements and the header's `384*0x180` geometry is a slice of the retail's, not the retail's.

### 10.10 Storage definitions and constants this fixes

```json
{
 "task_handler_table": {
  "va": "0x00A96AE0",
  "element": "dword (code pointer)",
  "elements": 97536,
  "bytes": 390144,
  "start": "0x00A96AE0",
  "end_exclusive": "0x00AF5EE0",
  "geometry": {"phases": 254, "classes": 384, "row_stride_bytes": 1536, "index": "class_id + phase * 384"},
  "proving_writers": ["0x0040117D", "0x00401183", "0x0040118F", "0x004019C1", "0x004019CF", "0x004019D6", "0x004019AD"],
  "semantic_writers": ["0x0040156E", "0x00401444"]
 },
 "first_link_flag_table": {
  "va": "0x0054FF34",
  "element": "byte",
  "elements": 97536,
  "bytes": 97536,
  "start": "0x0054FF34",
  "end_exclusive": "0x00567C34",
  "writers": ["0x00401575", "0x0040144B"]
 },
 "phase_map": {
  "va": "0x0054FF24",
  "bytes": 16,
  "image_bytes": "all 0x00",
  "runtime_init": "0xFF x16 by 0x004010F8-0x0040111D",
  "assigned_by": ["0x00401D38", "0x00401D66"]
 }
}
```

Two constants belong with these sizes: the row stride `0x180` dwords and the row count `254` (`0xFE`); the class count `384` (`0x4800/0x30`). `TaskType_Register` already loops `phase < 0xFE` in the current port, so the port's arrays, not its loop, are the odd ones out. `QTask_Init` (`0x004010D0`) in the current port does not reproduce the `0x17D00`-dword zero-fill or the neighbouring `0xAFA6E0`/`0xA969E0` initialisations; that is a separate transcription gap, recorded here because §10.1's proving block is exactly the missing initialisation.

## 11. APPENDED — statement-level derivation of `0x401530`

### 11.1 Identity, ABI, and what this function is

- **VA:** `0x00401530`–`0x004015CD` (158 bytes; the following four bytes `0x004015CE`–`0x004015CF` are `nop` padding, the next function starts at `0x004015D0`). Four basic blocks: entry/straight-line `0x401530`–`0x401580`, loop body `0x401582`–`0x4015A5`, loop step `0x4015AD`–`0x4015B7`, tail `0x4015B9`–`0x4015CD`.
- **Calling convention:** cdecl, three 4-byte arguments, caller cleans `0x0C`. Call sites push, in order, handler → phase → class id, so the parameter list is `(std::uint16_t class_id, std::uint32_t phase, FrameHandler handler)`. 623 call sites (`axtj`).
- **Return:** `std::uint32_t` in EAX = the value of `0x00567C70` **before** this call (the nesting depth at entry: 0 for the outermost registration, 1 inside its propagation, …). It is not used by any of the inspected call sites (the instruction after each `call 0x401530` never consumes EAX: the sites either start the next push sequence or reload a register from memory), but its computation is part of the body and must be reproduced.
- **Side effects:** one dword and one byte into the phase/handler tables, increments of two globals (`0x00567C70`, `0x00536148`), a recursive walk of the 384 type slots that can write more dwords/bytes, and a call to `0x4014B0` which rewrites every in-use type slot's 16-bit mask word.

What it means: *link `handler` as the implementation of `phase` for class `class_id` and for every class derived from it*. `class_id` is the type-table slot index returned by `TaskType_Register` (`0x401900` returns it in AX, `0x00401A0B`); `phase` is a phase/event id.

### 11.2 The argument frame, derived from the function's own stack reads

The pushes are the proof. Entry `[esp]` = return address, `[esp+4]` = arg1, `[esp+8]` = arg2, `[esp+0xC]` = arg3. Inside the function each read must be evaluated with the exact number of bytes pushed at that instruction:

| read site | bytes pushed at that point | `[esp+n]` = entry offset | resolves to |
| --- | --- | --- | --- |
| `0x0040153E` `mov eax, [esp+0xc]` | `ebx`, `ebp` = 8 | `+4` | **arg1** (`class_id`) |
| `0x0040154B` `mov ebp, [esp+0x14]` | `ebx`, `ebp`, `esi` = 12 | `+8` | **arg2** (`phase`) |
| `0x0040154F` `mov edx, [esp+0x18]` | 12 | `+0xC` | **arg3** (`handler`) |
| `0x00401589` `mov dx, word [esp+0x14]` | `ebx`, `ebp`, `esi`, `edi` = 16 | `+4` | **arg1** — the loop's compare operand |
| `0x0040159E` `mov eax, [esp+0x1c]` | 16 | `+0xC` | **arg3** — forwarded to the recursive call |

**Correction to the handoff (rounds 152–155):** the compared value at `0x00401589` is **arg1**, not arg2, because `push edi` at `0x0040156D` has already happened (16 bytes pushed, so `[esp+0x14]` = entry `+4`). Likewise the `and eax, 0xffff` at `0x00401553` masks arg1 (`eax` was loaded from `[esp+0xc]` with only 8 bytes pushed). The loop therefore compares each type record's parent word against the **class id being registered** (arg1) and forwards `arg2` (phase) unchanged; it does not compare phases. This is exactly the sort of silent swap the statement-level sheet exists to prevent, and it changes the semantics: the propagation walks the *derived-class* tree from arg1, it does not filter by phase.

### 11.3 Instruction-by-instruction derivation

`reg` values in the comment column are after the instruction executes.

| VA | bytes | instruction | exact statement / purpose |
| --- | --- | --- | --- |
| `0x00401530` | `8b 0d 70 7c 56 00` | `mov ecx, [0x567c70]` | `old = QTask_NoContextHandlerCount;` |
| `0x00401536` | `53` | `push ebx` | save callee-saved |
| `0x00401537` | `55` | `push ebp` | save callee-saved |
| `0x00401538` | `8b 2d 48 61 53 00` | `mov ebp, [0x536148]` | `tmp = QTask_LinkedHandlerCounter;` |
| `0x0040153E` | `8b 44 24 0c` | `mov eax, [esp+0xc]` | `eax = class_id;` |
| `0x00401542` | `41` | `inc ecx` | `old += 1;` |
| `0x00401543` | `45` | `inc ebp` | `tmp += 1;` |
| `0x00401544` | `56` | `push esi` | save callee-saved |
| `0x00401545` | `89 2d 48 61 53 00` | `mov [0x536148], ebp` | `QTask_LinkedHandlerCounter = tmp;` (monotonic, never restored) |
| `0x0040154B` | `8b 6c 24 14` | `mov ebp, [esp+0x14]` | `ebp = phase;` (the counter value in EBP is dead from here) |
| `0x0040154F` | `8b 54 24 18` | `mov edx, [esp+0x18]` | `edx = handler;` |
| `0x00401553` | `25 ff ff 00 00` | `and eax, 0xffff` | `eax = class_id & 0xffff;` (load-bearing: callers push dwords whose upper half is stale) |
| `0x00401558` | `8d 74 6d 00` | `lea esi, [ebp + ebp*2]` | `esi = phase * 3;` |
| `0x0040155C` | `89 0d 70 7c 56 00` | `mov [0x567c70], ecx` | `QTask_NoContextHandlerCount = old;` (stored **before** the handler store) |
| `0x00401562` | `c1 e6 07` | `shl esi, 7` | `esi = phase * 0x180;` (32-bit wrap; **no phase mask**) |
| `0x00401565` | `03 c6` | `add eax, esi` | `index = (class_id & 0xffff) + phase*0x180;` |
| `0x00401567` | `83 f9 01` | `cmp ecx, 1` | `depth = (old == 1);` |
| `0x0040156A` | `0f 94 c1` | `sete cl` | `cl = (old == 1);` (only CL; ECX keeps the counter in its upper bytes) |
| `0x0040156D` | `57` | `push edi` | save callee-saved |
| `0x0040156E` | `89 14 85 e0 6a a9 00` | `mov dword [eax*4 + 0xa96ae0], edx` | `QTask_HandlerTable[index] = handler;` (W3) |
| `0x00401575` | `88 88 34 ff 54 00` | `mov byte [eax + 0x54ff34], cl` | `QTask_FirstLinkFlag[index] = (old == 1);` (byte store, index **not** scaled by 4) |
| `0x0040157B` | `33 db` | `xor ebx, ebx` | `record_index = 0;` |
| `0x0040157D` | `bf e8 5e af 00` | `mov edi, 0xaf5ee8` | `record = &QTask_TypeTable[0];` (cursor at record + 0x08 = the parent word) |
| `0x00401582` | `8a 47 1d` | `mov al, [edi+0x1d]` | `al = record->in_use;` (`+0x25` from the record base, `+0x1D` from the cursor) |
| `0x00401585` | `84 c0` | `test al, al` | `if (!in_use) goto step;` |
| `0x00401587` | `74 24` | `je 0x4015ad` | |
| `0x00401589` | `66 8b 54 24 14` | `mov dx, word [esp+0x14]` | `parent_of_record = record->parent_type;` |
| `0x0040158E` | `66 39 17` | `cmp word [edi], dx` | `if (record->parent_type != (uint16)class_id) goto step;` |
| `0x00401591` | `75 1a` | `jne 0x4015ad` | (compares against **arg1**) |
| `0x00401593` | `8a 84 1e 34 ff 54 00` | `mov al, byte [esi + ebx + 0x54ff34]` | `al = QTask_FirstLinkFlag[phase*0x180 + record_index];` |
| `0x0040159A` | `84 c0` | `test al, al` | `if (flag != 0) goto step;` (a directly registered class is not overwritten) |
| `0x0040159C` | `75 0f` | `jne 0x4015ad` | |
| `0x0040159E` | `8b 44 24 1c` | `mov eax, [esp+0x1c]` | `handler = arg3;` (reloaded from the stack every iteration) |
| `0x004015A2` | `50` | `push eax` | argument 3 of the recursive call |
| `0x004015A3` | `55` | `push ebp` | argument 2 = `phase` (unmasked value as loaded) |
| `0x004015A4` | `53` | `push ebx` | argument 1 = `record_index` (the derived class id) |
| `0x004015A5` | `e8 86 ff ff ff` | `call 0x401530` | `QTask_LinkQTaskEventHandlerNoContextFn((uint16)record_index, phase, handler);` |
| `0x004015AA` | `83 c4 0c` | `add esp, 0xc` | caller cleanup of the three arguments |
| `0x004015AD` | `83 c7 30` | `add edi, 0x30` | `record = record + 1;` |
| `0x004015B0` | `43` | `inc ebx` | `record_index += 1;` |
| `0x004015B1` | `81 ff e8 a6 af 00` | `cmp edi, 0xafa6e8` | `} while (record_index < 384);` (end cursor = record 384's `+0x08`) |
| `0x004015B7` | `7c c9` | `jl 0x401582` | |
| `0x004015B9` | `e8 f2 fe ff ff` | `call 0x4014b0` | `TaskType_RefreshHandlerMask();` (no arguments) |
| `0x004015BE` | `a1 70 7c 56 00` | `mov eax, [0x567c70]` | `old = QTask_NoContextHandlerCount;` (reload **after** the refresh) |
| `0x004015C3` | `5f` | `pop edi` | restore callee-saved |
| `0x004015C4` | `48` | `dec eax` | `old -= 1;` |
| `0x004015C5` | `5e` | `pop esi` | restore callee-saved |
| `0x004015C6` | `5d` | `pop ebp` | restore callee-saved |
| `0x004015C7` | `a3 70 7c 56 00` | `mov [0x567c70], eax` | `QTask_NoContextHandlerCount = old;` |
| `0x004015CC` | `5b` | `pop ebx` | restore callee-saved |
| `0x004015CD` | `c3` | `ret` | `return old;` (EAX = value before this call) |

### 11.4 Exact C++ statements, in retail order

```cpp
// Native: QTask_LinkQTaskEventHandlerNoContextFn; retail VA 0x00401530-0x004015CD.
// cdecl, three 4-byte arguments, caller cleans 0x0C, returns the pre-call depth counter.
std::uint32_t NATIVE_DECL QTask_LinkQTaskEventHandlerNoContextFn(
    std::uint16_t class_id, std::uint32_t phase, igi::retail::core::FrameHandler handler)
{
    std::uint32_t depth = QTask_NoContextHandlerCount;          // 0x401530
    ++QTask_LinkedHandlerCounter;                               // 0x401538 / 0x401543 / 0x401545
    depth = depth + 1U;                                         // 0x401542
    const std::uint32_t raw_class_id = class_id;                // 0x40153E
    const std::uint32_t index = (raw_class_id & 0xFFFFU)        // 0x401553
                              + (phase * 0x180U);               // 0x401558,0x401562,0x401565 (uint32 wrap, no mask)
    QTask_NoContextHandlerCount = depth;                        // 0x40155C (before the handler store)
    const std::uint8_t first_link = (depth == 1U) ? 1U : 0U;    // 0x401567,0x40156A
    QTask_HandlerTable[index] = handler;                        // 0x40156E
    QTask_FirstLinkFlag[index] = first_link;                    // 0x401575 (one byte, index not scaled)
    // 0x40157B-0x4015B7: propagate the link to every derived class of class_id.
    for (std::uint32_t type_id = 0U, offset = 0U; type_id < 384U; ++type_id, offset += 0x30U)
    {
        const auto* slot = reinterpret_cast<const std::uint8_t*>(QTask_TypeTable) + offset;
        if (slot[0x25] == 0U)                                   // 0x401582-0x401587
            continue;
        if (*reinterpret_cast<const std::uint16_t*>(slot + 0x08)
            != static_cast<std::uint16_t>(class_id))            // 0x401589-0x401591 (arg1, not phase)
            continue;
        if (QTask_FirstLinkFlag[phase * 0x180U + type_id] != 0U) // 0x401593-0x40159C
            continue;
        QTask_LinkQTaskEventHandlerNoContextFn(                 // 0x4015A2-0x4015A5
            static_cast<std::uint16_t>(type_id), phase, handler);
    }
    TaskType_RefreshHandlerMask();                              // 0x4015B9
    --depth;                                                    // 0x4015BE, 0x4015C4
    QTask_NoContextHandlerCount = depth;                        // 0x4015C7
    return depth;                                               // EAX at 0x4015CD
}
```

Retail-order notes that the code must preserve:

1. `QTask_NoContextHandlerCount` is written (`0x40155C`) **before** the handler table and flag stores, and re-read/re-written at the tail (`0x4015BE`, `0x4015C7`).
2. The flag is computed from the *incremented* counter and written **before** the propagation loop (`0x401567`–`0x401575` precede `0x401582`). The flag store is unconditional; recursive entries (depth ≥ 2) therefore write `0`, and only the outermost call writes `1`.
3. `QTask_LinkedHandlerCounter` is incremented on every entry, including recursive ones, and never restored.
4. The propagation compare uses **arg1** (`class_id`), and the recursive call passes the matched **record index** as the new arg1, `phase` unchanged, `handler` unchanged.
5. `TaskType_RefreshHandlerMask()` runs after the whole propagation, and the counter is reloaded after it.

### 11.5 Globals, with address, width, and the instruction that proves the width

| global | width | initial (image) | role | references in this function |
| --- | --- | --- | --- | --- |
| `0x00567C70` | dword | 0 | no-context link nesting counter | `0x401530` read, `0x40155C` store, `0x4015BE` read, `0x4015C7` store |
| `0x00536148` | dword | `0xFFFFFFFF` | linked-handler counter (monotonic, starts -1) | `0x401538` read, `0x401545` store; also `0x40140D`/`0x40141C` in `0x401400`; read from `0x4C5C47`, `0x4CE5C2`, `0x4CE603` outside the subsystem |
| `0x00A96AE0` | dword × 0x17D00 | 0 | handler table (§10) | `0x40156E` store |
| `0x0054FF34` | byte × 0x17D00 | 0 | first-link flags (§10.9) | `0x401575` store, `0x401593` load |
| `0x00AF5EE0 + i*0x30` | record (32/16/8-bit fields) | zeroed at init | type table; `+0x08` parent class id (word), `+0x25` in-use byte | `0x40157D`–`0x4015B1` walk |
| `0x0054FF24` | byte × 16 | image 0, runtime `0xFF` | phase map, read indirectly through `0x4014B0`→`0x4014E0` | not read directly here |

### 11.6 Callees, argument order, and transcription status in this checkout

| callee | call site | arguments (cdecl order) | returns | status in this checkout |
| --- | --- | --- | --- | --- |
| `0x004014B0` `TaskType_RefreshHandlerMask` | `0x004015B9` | none | EAX = last `0x4014E0` result; retail callers ignore it | **TRANSCRIBED**: `engine/src/retail/core/native_4014b0.cpp`, 43 bytes, 5 blocks, walks the 384 type slots and stores the mask from `0x4014E0` at slot`+0x28` for each in-use slot. Its callee `0x004014E0` (`TaskType_HandlerPhaseMask`, 75 bytes) is **TRANSCRIBED** in `native_4014e0.cpp`: for map slot i=0..15, phase=`0x54FF24[i]`; if phase != `0xFF`, set bit i when `0xA96AE0[phase*0x180 + (type_id & 0xFFFF)]` is non-null; then set bit 15 (`or ah, 0x80`). |
| `0x00401530` (self) | `0x004015A5` | `(type_id, phase, handler)`, caller cleans `0x0C` | pre-call depth (unused by the loop) | this section; current port transcription is **incomplete** — see §11.9 |

What `0x4014B0` and `0x401400` require from a correct `0x401530`:

- **`0x4014B0` needs the stores to be complete and to have happened before it is called.** It derives each class's 16-bit mask by reading `table[phase*0x180 + class_id]` for the 16 map slots (`0x4014E0`). Every derived class's column must therefore already contain the handler when the refresh runs; that is why the retail calls it after the recursion (`0x4015B9`), not before the loop. If the propagation (or the top-level store) were reordered after the refresh, all classes except the directly linked one would keep a mask bit of 0 and their handlers would never be dispatched — a silent behavioural loss with no memory error.
- **`0x401400` needs nothing from `0x401530`'s return value** — it is a sibling with its own counter (`0x567C6C`) and its own stores (`0x401444`, `0x40144B`, `0x40148F` for the refresh). The coupling is shared state and identical geometry: both index the same table with `class_id + phase*0x180`, both walk the same type table with the same parent word, both increment `0x536148`, and both call `0x4014B0` at their tail. A transcription of one must not be "simplified" by reusing the other's masking: `0x401400` masks the phase with `and eax, 0xff` at `0x00401417`, `0x401530` has no such mask (§11.8, risk 5).

Sibling differences, from the same instruction-level reading of `0x401400` (`0x00401400`–`0x004014A3`, 164 bytes):

| aspect | `0x401530` | `0x401400` |
| --- | --- | --- |
| counter | `0x00567C70`, written `0x40155C`/`0x4015C7` | `0x00567C6C`, written `0x401435`/`0x40149D` |
| phase mask | **none** (`0x401558`/`0x401562`) | `and eax, 0xff` at `0x00401417` before `lea esi,[eax+eax*2]`/`shl esi,7` (`0x401426`/`0x40142D`) |
| arg loading | arg1 at `0x40153E`, arg2 at `0x40154B`, arg3 at `0x40154F` | arg2 at `0x401408` (`[esp+0x10]`, 8 pushed), arg3 at `0x401422`, arg1 at `0x401429` (`[esp+0x10]`, 12 pushed) |
| mask write | `0x401575` | `0x40144B` |
| propagation | identical loop, compare at `0x40158E` vs arg1, recurse at `0x4015A5` | identical loop, compare at `0x401464` vs arg1, recurse at `0x40147B` |
| refresh + return | `0x4014B0` at `0x4015B9`, return counter-1 | `0x4014B0` at `0x40148F`, return counter-1 |

### 11.7 Recursion re-entry: where the flag check is insufficient

The recursive call re-enters `0x401530` with a *different* arg1 (the matched record index), the same phase, the same handler. Because the entry increments the counter, the recursive call's depth is at least 2, so its flag store writes `0` (`cmp ecx,1`/`sete cl` at `0x401567`/`0x40156A`). Consequences:

1. **The propagation never marks visited classes.** A flag value of `1` exists only for classes linked directly (depth 1). Entries written by propagation stay `0`, and the loop's guard (`0x401593`–`0x40159C`) therefore passes for them on every later visit. Termination rests entirely on the parent-word graph being acyclic: the loop recurses into a record whenever its parent word equals the current class id, and nothing else stops re-entry.
2. **Self-parent record → unbounded recursion.** If a type slot's parent word equalled its own slot index, the loop would match that record, recurse with the same arg1, and repeat forever: the flag at that entry is `0` (written by the recursive call), so the guard cannot stop it. The retail's own allocation order makes this unreachable (TaskType_Register returns the slot index; derived declarations can only pass indices that already exist, so a parent index is always *earlier* than the child's slot) — but `0x401530` itself performs no check.
3. **Cycle → unbounded recursion.** Same argument for two slots pointing at each other (A's parent = B, B's parent = A). Not producible by the retail's registration order, not checked here.
4. **The `0x0181` sentinel aliases instead of failing.** If arg1 is `0x0181` (the "no parent" root word, also the static initial value of every class-word global, e.g. `0x54DD0C` = `0x0181` in the image), then `index = phase*0x180 + 385 = (phase+1)*0x180 + 1`: the handler and the flag land one row up, column 1 — inside the array, in another class's slot, with no error. The compare then matches every **root** record (parent word `0x0181`), so the call propagates from all roots rather than from the intended class. This is a real, silent path if a link call runs before the class's `TaskType_Register` return has been stored; the retail's own startup order avoids it, and a transcription must reproduce the arithmetic rather than "sanitise" it.
5. **Depth/stack.** For an acyclic tree the recursion depth is bounded by the number of classes (≤384), and the counter `0x567C70` tracks it; the flag check adds no depth bound and no visited set, so it is not a defence against 2–4.

Lead's two loud flags, answered: **(a)** there *are* writers of `0xA96AE0` outside the two registrars — `QTask_Init`'s zero-fill and `TaskType_Register`'s column loops — but no third owner of handler values and no unrelated writer (§10.6). **(b)** the recursion can re-enter with an index that defeats the flag check exactly in cases 1–4 above; the flag is a *direct-registration* marker, not a visited marker, and the only thing that keeps the walk finite is the acyclic parent graph produced by `TaskType_Register`'s allocation order.

### 11.8 `-O3` loss risks for this transcription

1. **The 8-bit store (`0x00401575`).** `mov byte [eax + 0x54ff34], cl` must become a one-byte store into a `std::uint8_t` array. If the flag array is declared with a wider element (or a temporary `std::uint32_t` is written through a cast), three neighbouring elements are overwritten — the same defect class that bit the earlier work. The read at `0x401593` is likewise an 8-bit load.
2. **The flag store must stay between the handler store and the loop.** It is unconditional at depth 1 and writes `0` at depth ≥ 2; only `1` marks a directly registered class. Moving it after the propagation, or writing it only when it is `1`, changes which subtrees later propagations may overwrite (and case 2's self-loop guard depends on the top-level `1` being in place before the loop).
3. **`sete cl` writes only CL.** The upper 24 bits of ECX still hold the incremented counter at that point; a transcription must not assign the flag back into the counter variable (the tail re-reads the counter from memory, so the visible behaviour would survive, but a `count = (count == 1)` form would corrupt the tail's decrement and the return).
4. **The phase multiply is an unmasked 32-bit wrap** (`lea esi,[ebp+ebp*2]` / `shl esi,7` at `0x401558`/`0x401562`). It must be `std::uint32_t phase * 0x180U` (or the explicit shift form), not `std::size_t` and not signed: on the 64-bit host a `size_t` computation does not wrap at 32 bits, and a signed multiply is undefined on overflow. The same wrap applies to `eax + esi` and to `eax*4` in the store's addressing.
5. **`0x401530` does not mask the phase, `0x401400` does.** A shared helper that applies `& 0xff` to both (the current port does exactly this) is a behavioural divergence for phase ≥ `0x100` and for the negative/upper-bit residue of a caller that pushed more than the phase id.
6. **The flag index is the same dword index, not a byte offset ×4.** `0x401575` uses `[eax + 0x54ff34]` with EAX = the composed dword index; `0x401593` uses `[esi + ebx + 0x54ff34]` with ESI = `phase*0x180` *as an index*, not `phase*0x180*4`. Scaling the flag index by 4 would read/write four phases away.
7. **The compare operand is arg1** (`0x401589`–`0x401591`). Both sides are 16-bit, so a swap to arg2 compiles cleanly and is silently wrong (§11.2).
8. **The recursive call's argument order and cleanup.** `push handler` (`0x40159E`), `push phase` (`0x4015A3`), `push type_id` (`0x4015A4`), `call` (`0x4015A5`), `add esp, 0x0C` (`0x4015AA`): arg1 is the *type id*, arg2 the phase *as loaded, unmasked*, arg3 the handler reloaded from the stack. The callee is cdecl; the retail batches some cleanups (`add esp,0x30` after four calls, e.g. `0x004EEABE`), so the callee must not pop its arguments.
9. **Loop bound and walk cursor.** 384 iterations from `edi = 0xAF5EE8` to `< 0xAFA6E8` step `0x30`; the in-use byte is at cursor `+0x1D` (`record + 0x25`), the parent word at cursor `+0` (`record + 0x08`). Using `0xAF5EE8` as the table base, or 383/385 iterations, breaks the walk; the port's type table base must remain `0xAF5EE0` (§11.10, correction 1).
10. **Array sizes must be the retail capacity.** `QTask_HandlerTable` and `QTask_FirstLinkFlag` are `0x17D00` elements each (254 × 384), not `384 × 384`; `TaskType_Register`'s column loop already uses `0xFE`, so the current port's arrays are the inconsistent part (over-sized by `0xC000` elements; harmless for in-domain indices, but not the retail storage definition, and any index-overrun behaviour would differ).
11. **Tail order and return value.** The refresh call (`0x4015B9`) precedes the reload/decrement/store (`0x4015BE`, `0x4015C4`, `0x4015C7`), and the return is the *pre-call* counter value (0 at the outermost level). Returning the composed index (as the current port does) is a different function contract; even though the inspected call sites ignore EAX, this must be corrected for fidelity and for any caller that is not yet transcribed.
12. **Two counters stay separate.** `0x567C70` (this function, starts 0, incremented on entry, decremented on exit, returned) versus `0x536148` (shared with `0x401400`, starts `0xFFFFFFFF`, incremented on every entry including recursion, never restored). `0x536148` is read outside the subsystem (`0x4C5C47`, `0x4CE5C2`, `0x4CE603`), so merging or resetting it is observable.
13. **The recursion is a real call with a 16-byte frame plus the three arguments**, not a loop: `0x567C70` exposes the nesting depth, and the flag behaviour depends on the depth. Tail-call or loop conversion by the optimiser must be prevented (the function is not tail-recursive — the loop step and the refresh follow the call — so this holds, but keep it a separate native so no inlining across VAs occurs).

### 11.9 Diff against the transcription currently in this checkout

`engine/src/retail/core/task_types.cpp` currently implements `0x401530` as `QTask_LinkQTaskEventHandlerNoContextFn`, delegating to the anonymous-namespace `link_handler` helper shared with `0x401400` (lines ~18–33). Against the derivation above the deltas are:

| # | retail | current port | consequence |
| --- | --- | --- | --- |
| D1 | propagation loop `0x401582`–`0x4015B7` (384-slot walk, parent compare, flag guard, recursive call) | **absent** | derived classes never receive the handler; their `0x4014E0` masks miss the phase; handlers of derived classes are never dispatched |
| D2 | `call 0x4014B0` at `0x4015B9` | **absent** | class mask words are never refreshed after a link, so even the directly registered class's dispatcher bit stays stale |
| D3 | returns EAX = pre-call `0x567C70` value | returns the composed `index` | wrong return contract (inert only for callers that ignore EAX, all inspected ones do) |
| D4 | `0x401530` leaves the phase unmasked | helper applies `phase & 0xFF` to both variants | divergence at phase ≥ `0x100`; `0x401400` needs the mask, `0x401530` must not have it |
| D5 | the two bodies are distinct retail functions with different counters and different masks | one shared helper | hides D4 and the counter difference; per rule 10.13/10.14 (no helper functions, one implementation per retail VA) the two must be written as their own bodies |
| D6 | `QTask_HandlerTable`/`QTask_FirstLinkFlag` are `0x17D00` elements | `384 * 384 = 147,456` elements | over-sized storage; header comment "class_id * 0x180 + phase" is transposed (correct: `class_id + phase * 0x180`) |

The parts that are already correct in the port and should not be touched: the counter increments (`++count`, `++QTask_LinkedHandlerCounter`), the first-link flag value `(count == 1)`, the dword store, the `class_id & 0xFFFF` mask, the `TaskType_RefreshHandlerMask` / `TaskType_HandlerPhaseMask` natives, and `TaskType_Register`'s `phase < 0xFE` column loop (which matches W1/W2 in §10.4).

### 11.10 Loud corrections to the handoff (rounds 152–155), and what to land

1. **Type-table object base is `0xAF5EE0`, not `0xAF5EE8`.** The object is `[0xAF5EE0, 0xAFA6E0)` = 384 × `0x30`; `0xAF5EE8` is *record + 0x08* — the walk cursor `0x401530`/`0x401400` initialise, and the parent-word field. Declaring the table at `0xAF5EE8` clips 8 bytes from the front of every record boundary. Proved by `QTask_ClearTaskTypes` `0x4011B0` (`mov eax,0xaf5ee0` … `cmp eax,0xafa6e0`), `TaskType_Register`'s writes at `0xAF5EE0`/`0xAF5EE4`/`0xAF5EE8`/`0xAF5F04`/`0xAF5F05`/`0xAF5F0C`, and `TaskType_RefreshHandlerMask`'s write at `0xAF5F08`.
2. **The loop compares arg1 (class id), not arg2 (phase).** `[esp+0x14]` at `0x401589` is entry `+4` because 16 bytes are pushed (incl. `push edi` at `0x40156D`); the same error appears in round 153/154's argument frame. The loop's compare value is also not "the id that indexes the table" — `arg1` is the *class id* (the column, and the propagation root), `arg2` is the phase (the row, and the table multiplier).
3. **The byte at `0xAF5F05` is the in-use flag, not the name byte.** The record's name is a dword pointer at `+0x00`; `0x401582` reads `[edi+0x1D]` = `record+0x25` = `in_use` (cleared by `QTask_ClearTaskTypes`). The propagation therefore visits only in-use classes.
4. **The handler table is not "indexed by the dword table's id"; it is 254 phases × 384 classes with index `class_id + phase*0x180`, extent `0x17D00` dwords, ending exactly at the type table base** (§10.1–§10.5). The header comment `The entry for a class is at class_id * 0x180 + phase` is transposed.
5. **The first-link array is `0x17D00` bytes, not `384 × 384`**, and starts at `0x54FF34`; the map at `0x54FF24` is 16 bytes, image-zero, runtime `0xFF` until `0x401D30`/`0x401D50` assign/clear (Lead's "`0xFF`-filled" holds at runtime only).
6. **Landing checklist for `0x401530`:** (a) two separate native bodies (`0x401530` unmasked phase, `0x401400` masked phase, separate counters) with no shared helper; (b) the propagation loop and the `0x4014B0` tail call; (c) return the pre-call depth; (d) array sizes `0x17D00`; (e) byte-width flag store and load; (f) `uint32` arithmetic for the phase multiply and index. With those, the function is body-complete; its runtime status stays `UNTRANSCRIBED`/`TRANSCRIBED` until the paired-evidence gate, since `0x4014B0`'s mask values depend on the `0x54FF24` writers that are only partly transcribed.
7. **`QTask_Init` gap (needed for §10.1 to be reproduced):** the port's `QTask_Init` currently omits the `0x40`-dword clear at `0xAFA6E0`, the `0xAFA7E0`/`0x54F92C` clears, the `0xA969E0` fill, the `0x17D00`-dword handler-table zero-fill, the `0x567C38` setup through `0x4B0B00`, and the `0x4B8810`("QTASK_TYPE_UNDEFINED", `0x0181`) call. Those are separate natives/blocks but the `0x17D00` zero-fill is the §10.1 proving block and must be reproduced for the initial state to match.

## Appendix A — machine-readable handler set

Compact JSON for scripting (`args` are 0-based argument indices derived from the slot reads; `registrations` are the derived type/class registrations):

```json
{
 "image": {"file": "igi.exe", "sha256": "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01", "base": "0x400000"},
 "function": "0x004EE970",
 "handlers": [
  {"va":"0x00427D70","size":11,"end":"0x00427D7B","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["Int32.kindE9","Real32.kindE9"]},
  {"va":"0x00427F80","size":11,"end":"0x00427F8B","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["Real64.kindE9"]},
  {"va":"0x00487D90","size":129,"end":"0x00487E11","bb":11,"edges":15,"insn":45,"args":[1,2],"registrations":["Static.prop(0x4CEA10 state)"]},
  {"va":"0x00487E20","size":129,"end":"0x00487EA1","bb":11,"edges":15,"insn":45,"args":[1,2],"registrations":["Static.prop(0x4CEA20 state)"]},
  {"va":"0x00488700","size":104,"end":"0x00488768","bb":11,"edges":15,"insn":36,"args":[1],"registrations":["CameraBase.method1"]},
  {"va":"0x004D3710","size":1,"end":"0x004D3711","bb":1,"edges":0,"insn":1,"args":[],"registrations":["Static.method1","Idle.method1","VarString.kindEB","Graph.kindEA","Graph.kindEB","Graph.kindE7","Graph.kindE8","Graph.kindE6"]},
  {"va":"0x004EF7F0","size":53,"end":"0x004EF825","bb":1,"edges":0,"insn":24,"args":[1,2],"registrations":["QTaskNote.kindE7"]},
  {"va":"0x004EF830","size":21,"end":"0x004EF845","bb":1,"edges":0,"insn":8,"args":[1,2],"registrations":["QTaskNote.kindE8"]},
  {"va":"0x004EF850","size":11,"end":"0x004EF85B","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["QTaskNote.kindE9"]},
  {"va":"0x004EF860","size":66,"end":"0x004EF8A2","bb":3,"edges":3,"insn":31,"args":[1,2],"registrations":["GraphEnvelope.kindEA"]},
  {"va":"0x004EF8B0","size":11,"end":"0x004EF8BB","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["GraphEnvelope.kindE6"]},
  {"va":"0x004EF8C0","size":196,"end":"0x004EF984","bb":8,"edges":11,"insn":78,"args":[1,2],"registrations":["GraphEnvelope.kindEB"]},
  {"va":"0x004EF990","size":22,"end":"0x004EF9A6","bb":1,"edges":0,"insn":10,"args":[1,2],"registrations":["GraphEnvelope.kindE7"]},
  {"va":"0x004EF9B0","size":22,"end":"0x004EF9C6","bb":1,"edges":0,"insn":10,"args":[1,2],"registrations":["GraphEnvelope.kindE8"]},
  {"va":"0x004EF9D0","size":11,"end":"0x004EF9DB","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["GraphEnvelope.kindE9"]},
  {"va":"0x004EF9E0","size":41,"end":"0x004EFA09","bb":1,"edges":0,"insn":18,"args":[1,2],"registrations":["Real32.kindEA"]},
  {"va":"0x004EFA10","size":11,"end":"0x004EFA1B","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["Int16.kindE6","Int32.kindE6","LevelExpression.kindE6","QTaskNote.kindE6","Real32.kindE6","Real64.kindE6","String16.kindE6","String256.kindE6","String32.kindE6","VarString.kindE6","bool8.kindE6","bool8.kindE9"]},
  {"va":"0x004EFA20","size":100,"end":"0x004EFA84","bb":3,"edges":4,"insn":43,"args":[1,2],"registrations":["Real32.kindEB"]},
  {"va":"0x004EFA90","size":15,"end":"0x004EFA9F","bb":1,"edges":0,"insn":6,"args":[1,2],"registrations":["Int32.kindE7","Real32.kindE7"]},
  {"va":"0x004EFAA0","size":15,"end":"0x004EFAAF","bb":1,"edges":0,"insn":6,"args":[1,2],"registrations":["Int32.kindE8","Real32.kindE8"]},
  {"va":"0x004EFAB0","size":79,"end":"0x004EFAFF","bb":1,"edges":0,"insn":34,"args":[1,2],"registrations":["Real32x3.kindEA"]},
  {"va":"0x004EFB00","size":11,"end":"0x004EFB0B","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["Quat32.kindE6","Real32x3.kindE6","Real32x9.kindE6","Real64x3.kindE6"]},
  {"va":"0x004EFB10","size":66,"end":"0x004EFB52","bb":3,"edges":4,"insn":23,"args":[1,2],"registrations":["Real32x3.kindEB"]},
  {"va":"0x004EFB60","size":27,"end":"0x004EFB7B","bb":1,"edges":0,"insn":10,"args":[1,2],"registrations":["Real32x3.kindE7"]},
  {"va":"0x004EFB80","size":27,"end":"0x004EFB9B","bb":1,"edges":0,"insn":10,"args":[1,2],"registrations":["Real32x3.kindE8"]},
  {"va":"0x004EFBA0","size":108,"end":"0x004EFC0C","bb":1,"edges":0,"insn":42,"args":[1,2],"registrations":["Real32x9.kindEA"]},
  {"va":"0x004EFC10","size":120,"end":"0x004EFC88","bb":3,"edges":4,"insn":41,"args":[1,2],"registrations":["Real32x9.kindEB"]},
  {"va":"0x004EFC90","size":22,"end":"0x004EFCA6","bb":1,"edges":0,"insn":10,"args":[1,2],"registrations":["Real32x9.kindE7"]},
  {"va":"0x004EFCB0","size":22,"end":"0x004EFCC6","bb":1,"edges":0,"insn":10,"args":[1,2],"registrations":["Real32x9.kindE8"]},
  {"va":"0x004EFCD0","size":11,"end":"0x004EFCDB","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["Real32x9.kindE9"]},
  {"va":"0x004EFCE0","size":108,"end":"0x004EFD4C","bb":1,"edges":0,"insn":42,"args":[1,2],"registrations":["Quat32.kindEA"]},
  {"va":"0x004EFD50","size":399,"end":"0x004EFEDF","bb":12,"edges":17,"insn":133,"args":[1,2],"registrations":["Quat32.kindEB"]},
  {"va":"0x004EFEE0","size":22,"end":"0x004EFEF6","bb":1,"edges":0,"insn":10,"args":[1,2],"registrations":["Quat32.kindE7"]},
  {"va":"0x004EFF00","size":22,"end":"0x004EFF16","bb":1,"edges":0,"insn":10,"args":[1,2],"registrations":["Quat32.kindE8"]},
  {"va":"0x004EFF20","size":11,"end":"0x004EFF2B","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["Quat32.kindE9"]},
  {"va":"0x004EFF30","size":41,"end":"0x004EFF59","bb":1,"edges":0,"insn":18,"args":[1,2],"registrations":["Real64.kindEA"]},
  {"va":"0x004EFF60","size":46,"end":"0x004EFF8E","bb":3,"edges":4,"insn":18,"args":[1,2],"registrations":["Real64.kindEB"]},
  {"va":"0x004EFF90","size":21,"end":"0x004EFFA5","bb":1,"edges":0,"insn":8,"args":[1,2],"registrations":["Real64.kindE7"]},
  {"va":"0x004EFFB0","size":21,"end":"0x004EFFC5","bb":1,"edges":0,"insn":8,"args":[1,2],"registrations":["Real64.kindE8"]},
  {"va":"0x004EFFD0","size":79,"end":"0x004F001F","bb":1,"edges":0,"insn":34,"args":[1,2],"registrations":["Real64x3.kindEA"]},
  {"va":"0x004F0020","size":62,"end":"0x004F005E","bb":3,"edges":4,"insn":26,"args":[1,2],"registrations":["Real64x3.kindEB"]},
  {"va":"0x004F0060","size":22,"end":"0x004F0076","bb":1,"edges":0,"insn":10,"args":[1,2],"registrations":["Real64x3.kindE7"]},
  {"va":"0x004F0080","size":22,"end":"0x004F0096","bb":1,"edges":0,"insn":10,"args":[1,2],"registrations":["Real64x3.kindE8"]},
  {"va":"0x004F00A0","size":11,"end":"0x004F00AB","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["Real64x3.kindE9"]},
  {"va":"0x004F00B0","size":41,"end":"0x004F00D9","bb":1,"edges":0,"insn":18,"args":[1,2],"registrations":["Int32.kindEA"]},
  {"va":"0x004F00E0","size":42,"end":"0x004F010A","bb":3,"edges":4,"insn":16,"args":[1,2],"registrations":["Int32.kindEB"]},
  {"va":"0x004F0110","size":42,"end":"0x004F013A","bb":1,"edges":0,"insn":18,"args":[1,2],"registrations":["Int16.kindEA"]},
  {"va":"0x004F0140","size":43,"end":"0x004F016B","bb":3,"edges":4,"insn":16,"args":[1,2],"registrations":["Int16.kindEB"]},
  {"va":"0x004F0170","size":17,"end":"0x004F0181","bb":1,"edges":0,"insn":6,"args":[1,2],"registrations":["Int16.kindE7"]},
  {"va":"0x004F0190","size":17,"end":"0x004F01A1","bb":1,"edges":0,"insn":6,"args":[1,2],"registrations":["Int16.kindE8"]},
  {"va":"0x004F01B0","size":11,"end":"0x004F01BB","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["Int16.kindE9"]},
  {"va":"0x004F01C0","size":167,"end":"0x004F0267","bb":5,"edges":6,"insn":64,"args":[1,2],"registrations":["VarString.kindEA"]},
  {"va":"0x004F0270","size":59,"end":"0x004F02AB","bb":3,"edges":2,"insn":28,"args":[1,2],"registrations":["VarString.kindE7"]},
  {"va":"0x004F02B0","size":107,"end":"0x004F031B","bb":5,"edges":6,"insn":48,"args":[1,2],"registrations":["VarString.kindE8"]},
  {"va":"0x004F0320","size":40,"end":"0x004F0348","bb":3,"edges":2,"insn":17,"args":[1,2],"registrations":["VarString.kindE9"]},
  {"va":"0x004F0350","size":42,"end":"0x004F037A","bb":1,"edges":0,"insn":19,"args":[1,2],"registrations":["String16.kindEA"]},
  {"va":"0x004F0380","size":22,"end":"0x004F0396","bb":1,"edges":0,"insn":10,"args":[1,2],"registrations":["LevelExpression.kindE9","String16.kindE9","String256.kindE9","String32.kindE9"]},
  {"va":"0x004F03A0","size":42,"end":"0x004F03CA","bb":1,"edges":0,"insn":19,"args":[1,2],"registrations":["String32.kindEA"]},
  {"va":"0x004F03D0","size":42,"end":"0x004F03FA","bb":1,"edges":0,"insn":20,"args":[1,2],"registrations":["LevelExpression.kindE8","String16.kindE8","String256.kindE8","String32.kindE8"]},
  {"va":"0x004F0400","size":45,"end":"0x004F042D","bb":1,"edges":0,"insn":19,"args":[1,2],"registrations":["String256.kindEA"]},
  {"va":"0x004F0430","size":40,"end":"0x004F0458","bb":3,"edges":4,"insn":15,"args":[1,2],"registrations":["String16.kindEB","String256.kindEB","String32.kindEB"]},
  {"va":"0x004F0460","size":57,"end":"0x004F0499","bb":1,"edges":0,"insn":28,"args":[1,2],"registrations":["LevelExpression.kindEA"]},
  {"va":"0x004F04A0","size":42,"end":"0x004F04CA","bb":3,"edges":4,"insn":16,"args":[1,2],"registrations":["LevelExpression.kindEB"]},
  {"va":"0x004F04D0","size":42,"end":"0x004F04FA","bb":1,"edges":0,"insn":20,"args":[1,2],"registrations":["LevelExpression.kindE7","String16.kindE7","String256.kindE7","String32.kindE7"]},
  {"va":"0x004F0500","size":41,"end":"0x004F0529","bb":1,"edges":0,"insn":18,"args":[1,2],"registrations":["bool8.kindEA"]},
  {"va":"0x004F0530","size":43,"end":"0x004F055B","bb":3,"edges":4,"insn":16,"args":[1,2],"registrations":["bool8.kindEB"]},
  {"va":"0x004F0560","size":15,"end":"0x004F056F","bb":1,"edges":0,"insn":6,"args":[1,2],"registrations":["bool8.kindE7"]},
  {"va":"0x004F0570","size":15,"end":"0x004F057F","bb":1,"edges":0,"insn":6,"args":[1,2],"registrations":["bool8.kindE8"]},
  {"va":"0x004F0580","size":128,"end":"0x004F0600","bb":11,"edges":15,"insn":43,"args":[1],"registrations":["Level.method1"]},
  {"va":"0x004F0600","size":10,"end":"0x004F060A","bb":1,"edges":0,"insn":3,"args":[1],"registrations":["Level.prop(0)"]},
  {"va":"0x004F0610","size":11,"end":"0x004F061B","bb":1,"edges":0,"insn":2,"args":[],"registrations":["Level.method2"]},
  {"va":"0x004F0620","size":19,"end":"0x004F0633","bb":3,"edges":3,"insn":6,"args":[1],"registrations":["Static.prop(0)"]},
  {"va":"0x004F0640","size":20,"end":"0x004F0654","bb":3,"edges":3,"insn":5,"args":[],"registrations":["Static.method2"]},
  {"va":"0x004F0660","size":113,"end":"0x004F06D1","bb":12,"edges":17,"insn":39,"args":[1],"registrations":["Dynamic.method1"]},
  {"va":"0x004F06E0","size":10,"end":"0x004F06EA","bb":1,"edges":0,"insn":3,"args":[1],"registrations":["Dynamic.prop(0)"]},
  {"va":"0x004F06F0","size":11,"end":"0x004F06FB","bb":1,"edges":0,"insn":2,"args":[],"registrations":["Dynamic.method2"]},
  {"va":"0x004F0700","size":10,"end":"0x004F070A","bb":1,"edges":0,"insn":3,"args":[1],"registrations":["Idle.prop(0)"]},
  {"va":"0x004F0710","size":11,"end":"0x004F071B","bb":1,"edges":0,"insn":2,"args":[],"registrations":["Idle.method2"]},
  {"va":"0x004F0720","size":10,"end":"0x004F072A","bb":1,"edges":0,"insn":3,"args":[1],"registrations":["CameraBase.prop(0)"]},
  {"va":"0x004F0730","size":11,"end":"0x004F073B","bb":1,"edges":0,"insn":2,"args":[],"registrations":["CameraBase.method2"]},
  {"va":"0x004F0740","size":60,"end":"0x004F077C","bb":1,"edges":0,"insn":16,"args":[1],"registrations":["QVM:Task_DeclareParameters"]},
  {"va":"0x004F0780","size":731,"end":"0x004F0A5B","bb":30,"edges":44,"insn":228,"args":[1,2,3],"registrations":["QVM:Task_New"]},
  {"va":"0x004F64E0","size":11,"end":"0x004F64EB","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["Graph.kindE9"]},
  {"va":"0x0051BAA0","size":11,"end":"0x0051BAAB","bb":1,"edges":0,"insn":3,"args":[2],"registrations":["Real32x3.kindE9"]}
 ]
}
```

## Appendix B — evidence commands

```sh
# function listing (committed at docs/fidelity/evidence/0x004EE970-full-listing.txt)
r2 -q -e scr.color=0 -c 'pD 3709 @ 0x4EE970' igi.exe

# cross-references (aaa is required before axtj returns anything)
r2 -q -e scr.color=0 -e bin.relocs.apply=true -c 'aaa; axtj @ 0x00A70CA8' igi.exe

# exact instruction bytes/opcode for a proving instruction
r2 -q -e scr.color=0 -c 'aoj @ 0x4EF7C5' igi.exe
```

---

## §12 — WinMain flow: `0x4010B0` and `0x498480`

> Placement: inserted after **Appendix B** and before the already-present `## §13`, which a later pass appended to this same file, so that the section numbering stays in order. No existing section, table, appendix or §13 line is modified: the insertion is a pure add between two existing regions, and the document's earlier bytes and §13 are byte-identical to before this pass.

### 12.0 Scope, method and artefact

- **Artefact:** `/Users/haseeb-mir/Downloads/IGI/igi.exe`, SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, PE32 i386, preferred base `0x00400000`, `.text` VA `0x00401000`. Hash re-checked before and after this pass. **No runtime was produced:** no game process, no Wine/CrossOver, no Frida/IGI-Probe; every statement below is static evidence.
- **Method:** radare2 6.2.0 (`aaa`, `afij`, `afbj`, `pdj`) for decode and block recovery; independent Python reads of the image for every byte string and every `E8`/`E9` `rel32` target (each target recomputed as `next_ip + rel`). Linear `pdj` decode is used instead of `pdf` because `aaa` tail-merges `0x4010B0` into the `0x402C40` cascade (`afij`: `size 1250392`, `maxaddr 0x532208`, `is-lineal:false`); the true 28-byte body is only visible with a linear walk.
- **Stack-offset method (the proof for every argument claim):** `E` = `esp` at function entry (pointing at the return address). At any instruction `T` = bytes below `E` consumed by the frame (`sub esp,N` plus pushes, minus `pop`s, minus the argument bytes popped by each stdcall/COM callee). A `[esp+disp]` operand then resolves to entry offset `disp - T`: `+4` = arg1, `+8` = arg2, …. `T` is recomputed from the retail opcodes themselves, never from r2's `var_XXh` names (those names disagree with the bytes in `0x498480`; see C3).
- **No decompiler was run in this pass.** The Binary Ninja record already on disk for `0x4010B0` (`docs/fidelity/evidence/0x004010B0-static-analysis.json`) is cited as *prior evidence* and is re-derived here from bytes; the candidate sources under `scripts/GameNatives/` are cited as *candidates*. Neither substitutes for the selected-decompiler + angr step that AGENTS §10.2 requires before a transcription is promoted, and this section assigns no status beyond `ANALYZED`.

### 12.1 Corrections to what the current handoff believes

**C1 — `0x4010B0`'s call site is inside the window procedure, not in WinMain's own body.** WinMain stores `0x0048F970` into the `WNDCLASS.lpfnWndProc` slot at `0x0048F59E` (`c7 44 24 34 70 f9 48 00`) and registers the class at `0x0048F5E6` (`call [USER32!RegisterClassA]`). The `call 0x004010B0` at `0x0048F9D4` is inside `0x0048F970`'s `WM_DESTROY` arm: `0x48F9C5 push 1` → `0x48F9CE call [USER32!ShowCursor]` → `0x48F9D4 call 0x4010B0` → `0x48F9D9 push 0` → `0x48F9DB call [USER32!PostQuitMessage]`. So the `WinMain → 0x4010B0` relation is a *dispatch* edge (`WinMain → DispatchMessageA → WndProc → callee`), not a direct `call` instruction in `0x0048F530`. That is why `docs/fidelity/mvp-call-flow.md` row 68 records the site at `0x0048F9D4`: it is a WndProc call site that becomes reachable from WinMain only through message dispatch.

**C2 — `0x4010B0` has exactly one real predecessor: the direct call at `0x0048F9D4`. The apparent fall-through predecessor `0x004010A0` is not a reachable edge.** `0x004010A0` is its own 16-byte entry, laid out immediately before `0x4010B0`: `6a 01` `push 1`; `68 e0 f8 54 00` `push 0x0054F8E0`; `e8 d0 3f 0a 00` `call 0x004A507C`; then `90 90 90 90` at `0x004010AC`–`0x004010AF` and **no `ret`**. The prior record reports Binary Ninja's fall-through edge from `0x004010AF`, but the callee `0x004A507C` ends `0x4A50EB 8b 62 10 mov esp,[edx+0x10]` / `0x4A50EE 83 c4 04 add esp,4` / `0x4A50F1 ff 62 14 jmp dword [edx+0x14]` — it restores an SEH frame's saved stack and jumps to a saved continuation, so it never returns to `0x004010AC`. The four `nop`s are layout padding. `0x004A507C`'s only `ret` is at `0x004A50F4`, placed immediately after the unconditional `jmp dword [edx+0x14]` at `0x004A50F1` and therefore unreachable: every path of that callee ends in the indirect jump, not in a normal return. A full `.text` scan of every `e8`/`e9` `rel32` confirms `0x4010B0` has exactly one reference in the image, the call at `0x0048F9D4`. **Do not add a second predecessor to `0x4010B0`.**

**C3 — `0x498480` takes exactly two arguments, both write-only out-pointers; r2's three-argument signature is wrong.** `afij` prints `signature: fcn.00498480 (int32_t arg_130h, int32_t arg_130h_2, int32_t arg_134h)` with `nargs: 3`, listing the **same** `[esp+0x130]` slot twice. The body contains exactly **six** reads of `[esp+0x130]` (= entry `+4`: `0x4984C4`, `0x498503`, `0x498519`, `0x4985A8`, `0x4985D5`, `0x498632`) and exactly **two** reads of `[esp+0x134]` (= entry `+8`: `0x4984BD`, `0x4984E1`); no `[esp+0x138]` operand exists in the 374 instructions, and neither pointer is ever dereferenced before being written. WinMain's `push edx`/`push eax` at `0x0048F6E0`/`0x0048F6E1` are those two arguments — **no dead pushed word** at that call site.

**C4 — `0x00402C40` is a genuine, standalone zero-argument entry (775 bytes, one block, 154 calls + one tail jump), not part of `0x004010B0` and not part of `0x00402930`.** `aaa` cannot recover it because its only reference is the `jmp` at `0x004010C7`. The body is exactly `0x00402C40`–`0x00402F46`, ending `0x00402F42 e9 49 60 10 00 jmp 0x00508F90`, then `nop` padding to the next function. All 155 instructions are five bytes: 154 `e8 rel32` calls (`0x402C40`, `0x402C45`, …, `0x402F3D`) plus that final `e9 rel32`. This is the region r2 shows merged into `0x4010B0`; the two must not be treated as one function.

**C5 — `0x498480` is a platform probe, not an engine native.** Its 55 `call` instructions resolve to 8 distinct Win32/ole32 imports (through IAT slots), COM/DirectX interface calls through vtables, and two `GetProcAddress`-derived pointers. It reads no engine global, writes no engine global, and calls no engine native (12.11). Nothing in it joins the engine-native transcription queue; what a transcription needs from the port is the exact GUID/string constants and the exact per-path side effects (12.10, 12.12).

**C6 — `0x004A5AD5` is a real function entry and a CRT `exit`-family wrapper, not a disassembly artefact and not `Camera_ApplyBandFilter_6`.** 17 bytes, one argument, three genuine `E8` call sites, including `exit(main(...))` in CRT startup (12.13). The external catalogue name in `docs/DISCOVERED_NATIVES_RECORD.md` is contradicted by the body.

**C7 — Names.** None of these VAs has a retail symbol in this checkout's manifest. `QTask_DispatchEvent` (`0x4010B0`), `Display_DinputDll` (`0x498480`), `Flow_DeleteHandler` (`0x402C40`), `QTask_FindType` (`0x4015F0`), `QTask_DispatchEvent_2` (`0x401A60`), `Input_GetDIErrStr` (`0x48F970`) are Binary Ninja database annotations or external-catalogue names, not retail symbols. The names used in this sheet (`out_dx_version`, `out_platform`, `pDDraw`, `pDDraw2`, `pSurface`, `pSurf3`, `pSurf4`, `pDM`, `pDD7`, `ddsd`, `osi`) are working descriptions, marked as such wherever they appear. Only the retail strings and GUID bytes in 12.10 are evidence of what the code does.

### 12.2 `0x004010B0` — identity, ABI, and what the function is

- **VA range:** `0x004010B0`–`0x004010CB`, **28 bytes**: `e8 ab 09 00 00 85 c0 74 0e e8 c2 17 00 00 50 e8 2c 05 00 00 83 c4 04 e9 74 1b 00 00`. Bytes `0x004010CC`–`0x004010CF` are `90 90 90 90` padding; the next entry is `0x004010D0`.
- **Shape:** 3 basic blocks, 8 instructions, 3 edges. Entry `0x4010B0`–`0x4010B8`; conditional block `0x4010B9`–`0x4010C6`; shared tail `0x4010C7`–`0x4010CB`. No loop, no `ret`.
- **Arguments: none.** The 28 bytes contain **zero** `[esp+*]`/`[ebp+*]` memory operands (and no memory operand at all). Proof is the function's own reads, not the caller's pushes: `0x48F9C5 push 1` is `ShowCursor`'s argument and was consumed by the stdcall `call [ShowCursor]` at `0x48F9CE`; nothing is pushed between `0x48F9CE` and `0x48F9D4`, so `0x4010B0` is entered with an empty argument frame. **Any signature giving it an argument is wrong**; the `push 1` is a dead-looking word that belongs to the previous call.
- **Calling convention / cleanup:** no `ret`; the function ends in an unconditional `jmp 0x00402C40`, so the cascade's eventual `ret` returns directly to `0x0048F9D9`. With zero arguments there is nothing to clean. The `push eax`/`add esp,4` pair at `0x4010BE`/`0x4010C4` is the one-argument **cdecl** call to `0x004015F0`.
- **Return value: not defined by this body.** The caller-visible EAX is whatever the tail region leaves. The direct caller discards it (next instruction is `push 0` for `PostQuitMessage`). The last call executed in the cascade is `0x005324FE call 0x004B0B90` before `0x00532503 jmp 0x004C1780`, so the value would be `0x4B0B90`'s return — cascade-dependent; no source-level return type is proven.
- **Direct side effects:** none of its own. There is no global or absolute-address operand in the body (12.6). Everything is a callee effect.
- **What it is, stated only from its branches:** *if `0x401A60()` is non-zero, pass the object returned by `Game_GetContext()` to `0x4015F0`; then always continue into the `0x402C40` chain.* The semantics of `0x401A60`, `0x4015F0` and `0x402C40` are **not** established here.

### 12.3 `0x004010B0` — instruction-by-instruction derivation

| VA | bytes | instruction | exact statement / purpose |
| --- | --- | --- | --- |
| `0x004010B0` | `e8 ab 09 00 00` | `call 0x00401A60` | `if (f_401A60() != 0)` — EAX is the test operand; no other use |
| `0x004010B5` | `85 c0` | `test eax, eax` | flags from the returned dword; only zero/non-zero is proven, no width |
| `0x004010B7` | `74 0e` | `je 0x004010C7` | skip the two-call block; the target **is** the shared tail, so both paths converge there |
| `0x004010B9` | `e8 c2 17 00 00` | `call 0x00402880` | `Game_GetContext()` — zero-argument call; EAX = context pointer |
| `0x004010BE` | `50` | `push eax` | the single argument of `0x004015F0` |
| `0x004010BF` | `e8 2c 05 00 00` | `call 0x004015F0` | `f_4015F0(ctx)`; the following `add esp,4` proves cdecl + one dword argument |
| `0x004010C4` | `83 c4 04` | `add esp, 4` | caller cleanup for the cdecl call |
| `0x004010C7` | `e9 74 1b 00 00` | `jmp 0x00402C40` | tail jump; the callee's `ret` (at `0x4C1780`) returns to `0x0048F9D9`; no frame to unwind |

Then `90 90 90 90` at `0x004010CC`–`0x004010CF` (padding) and the next entry at `0x004010D0`.

### 12.4 `0x004010B0` — retail-order statements

```c
/* working names only; no retail symbol is proven for any callee */
void f_4010B0(void)              /* 0x4010B0; no arguments, no ret, tail-jumps */
{
    if (f_401A60() != 0)         /* 0x4010B0, 0x4010B5, 0x4010B7 */
    {
        void* ctx = Game_GetContext();      /* 0x4010B9 */
        f_4015F0(ctx);                      /* 0x4010BE push, 0x4010BF call, 0x4010C4 add esp,4 */
    }
    return f_402C40();           /* 0x4010C7 jmp; the value is the cascade's EAX, not this body's */
}
```

The `je` at `0x4010B7` jumps straight to the tail block, so there is no `else`: the tail always runs. The body has no counting, no logging, no global state.

### 12.5 `0x004010B0` — callees in call order, arguments, and the transcription queue

Argument counts are proven from each **callee's own** stack reads (or its complete lack of them), never from push sequences alone.

| order | call site | callee | arguments (proven from the callee) | return used | manifest / transcription status |
| ---: | --- | --- | --- | --- | --- |
| 1 | `0x004010B0` | `0x00401A60` | **0 args** — body `a1 58 7c 56 00 c3` = `mov eax,[0x00567C58]; ret`; no stack operand | tested for zero only | **NOT IN MANIFEST → missing** (external label `QTask_DispatchEvent_2`) |
| 2 | `0x004010B9` | `0x00402880` `Game_GetContext` | **0 args** — body `a1 8c 7c 56 00 c3` = `mov eax,[0x00567C8C]; ret` | passed as argument 1 of callee 3 | `STRUCTURALLY_VERIFIED`, `engine/src/retail/core/frame_natives.cpp` → **transcribed** |
| 3 | `0x004010BF` | `0x004015F0` | **1 cdecl dword argument** — `afij`: `cdecl`, `nargs 1`, `recursive: true`, 391 bytes, 25 blocks, 134 instructions; the `push eax` at `0x4010BE` is its argument and `add esp,4` cleans it | discarded | **NOT IN MANIFEST → missing** (external label `QTask_FindType`); 120 call sites in the image — a hub |
| 4 (tail) | `0x004010C7` | `0x00402C40` | **0 args** — 775 bytes, one block, 154 zero-argument `e8` calls, final `jmp 0x00508F90`; no push precedes any call | EAX becomes the caller's EAX | **NOT IN MANIFEST → missing** (external label `Flow_DeleteHandler`) |

**The `0x00402C40` cascade (call-order facts only; not transcribed here).** It calls, in order, `0x004C1780` — a **1-byte `c3 ret` no-op entry** reused many times — then `0x004192B0`, `0x004199B0`, `0x0041A1A0`, `0x0041AAC0`, `0x0041B260`, `0x0041B840`, `0x0041C660`, `0x0041D050`, `0x0041DBF0`, … and ends `jmp 0x00508F90`. The tail chain continues `0x00508F90` → `0x00518550` → `0x005302A0` → `0x00530A20` → `0x005311F0` → `0x005323D0` → `0x005323F0` → `0x00532490`, whose last call is `0x005324FE call 0x004B0B90`, followed by `0x00532503 e9 78 f2 f8 ff jmp 0x004C1780`; because `0x4C1780` is a single-byte `ret`, that jump returns to `0x4010B0`'s caller with `0x4B0B90`'s EAX. I re-opened `0x402C40` (155 five-byte instructions) and the `0x532490` block (last call `0x4B0B90`, then `jmp 0x4C1780`); the remaining chain boundaries are the ones already recorded in `0x004010B0-static-analysis.json` and are provenance-labelled as prior-record facts.

**Call-site census in this image** (every `e8`/`e9` `rel32` in `.text` recomputed as `site+5+rel`, so the counts are complete, not sampled): `0x004010B0` — **1** site, `0x0048F9D4`; `0x00401A60` — **2** sites, `0x004010B0` and `0x004317E8`; `0x004015F0` — **120** sites (a genuine hub, which is why it is the expensive item in the queue); `0x00402C40` — **1** reference, the `jmp` at `0x004010C7`, so it is reachable only as this tail and never called directly, which is exactly why `aaa` could not recover it; `0x00498480` — **1** site, `0x0048F6E2`; `0x004A5AD5` — **3** sites (`0x0048F71F`, `0x00498941`, `0x004A51C7`); `0x004A5AE6` — **1** site (`0x004A51E6`).

**Queue for the next rounds, in call order:** `0x00401A60` (six-byte getter — trivial), `0x004015F0` (391-byte recursive hub, 120 call sites), `0x00402C40` (775 bytes) plus its 13-block cascade down to `0x4C1780`. `0x00402880` is already transcribed. Note that the first call in the cascade, `0x4C1780`, is a legitimate transcription target (a 1-byte `ret` function), not something to skip: the chain calls it repeatedly.

### 12.6 `0x004010B0` — globals, `-O3` loss risks, callee-level effects

**Globals read/written by the 28-byte body: none.** There is no absolute-address operand of any kind in `0x004010B0`–`0x004010CB` (operands are registers, one call target, one branch displacement, the immediate `4`). Every global effect is a callee effect. The globals reached through the direct callees, with the proving instruction **in the callee**, are:

| global | width | access | proving instruction (callee) |
| --- | --- | --- | --- |
| `0x00567C58` | 4 | read | `0x00401A61 a1 58 7c 56 00 mov eax,[0x567C58]` |
| `0x00567C58` | 4 | read + write | `0x004015F0 8b 15 58 7c 56 00` … `0x004015F9 89 15 58 7c 56 00` |
| `0x00AFA7E0` | 4 | read | `0x004015FF 8b 15 e0 a7 af 00` |
| `0x00AFA6E0` | 4·N | read | `0x00401636 89 0c 85 e0 a6 af 00` (slot write) |
| `0x00A97CE0` | 4·N | read | `0x00401643 8b 04 85 e0 7c a9 00` (function-pointer table) |
| `0x00A976E0` | 4·N | read | `0x00401676 8b 04 8d e0 76 a9 00` (function-pointer table) |
| `0x00567C38` | 4 | read | `0x004016A0 8b 1d 38 7c 56 00` |
| `0x00567C3C`, `0x00567C40`, `0x00567C48` | 4 each | read | `0x0040169B`, `0x0040168E`, `0x004016A9` |
| `0x00AF5EE0`, `0x00AF5EEC`, `0x00AF5EF0`, `0x00AF5EF4`, `0x00AF5EFC`, `0x00AF5F00` | 4 each | read/write | `0x004016E4` … `0x0040175B` (type-table slots) |

These are callee facts, listed because the queue starts inside them. `0x4010B0` itself only forwards `Game_GetContext()`'s pointer to `0x004015F0` and enters the cascade.

**`-O3` loss risks for `0x4010B0`:**

1. **Tail jump, not a call.** `0x4010C7` is `jmp`; the retail never returns into `0x48F9D9` with a frame to unwind. Write it as the tail call it is and preserve the cascade's return value (or mark the return unused, as the caller does).
2. **Shared tail block.** `je` and fall-through both reach `0x4010C7`; adding an `else` or duplicating the tail changes the CFG. The `0x402C40` call is unconditional.
3. **`push eax` / `add esp,4` = one cdecl argument.** Do not model `0x4015F0` as zero-argument just because `0x4010B0` takes none; do not use `stdcall`.
4. **Only `test eax,eax` exists on `0x401A60`'s return.** No comparison against `-1`, `TRUE`, or a wider type is proven.
5. **No pushed argument at the call site.** `0x48F9C5 push 1` belongs to `ShowCursor`; copying it into this native would invent an argument.

**`0x4010B0` NOT proven:** its name; what `0x401A60`'s non-zero result means; what `0x4015F0` does beyond the globals listed; the cascade's return type/value; any runtime effect.

### 12.7 `0x00498480` — identity, frame, ABI, and the call site

- **VA range:** `0x00498480`–`0x004988C9`, **1098 bytes** (`0x44A`); end-exclusive `0x004988CA`; bytes `0x004988CA`–`0x004988CF` are six `90` padding bytes; the next entry is `0x004988D0`.
- **Shape:** 38 basic blocks, 40 edges, 374 instructions (`afbj`/`pdj`; the 374 decoded instructions are contiguous with no gaps), cdecl (`add esp,0x11C; ret`, caller removes the arguments), frame `sub esp,0x11C` + 4 saved registers = **0x12C bytes below `E`**.
- **Arguments: exactly two, both out-pointers, both 4-byte writes.** Proven by the function's own reads: six `[esp+0x130]` reads (entry `+4`) and two `[esp+0x134]` reads (entry `+8`); no other positive-displacement read exists. Neither pointer is ever dereferenced before a write, and both are only written. No pushed word at the call site is dead.
- **Return value: no source-level return type is proven.** EAX at the `ret` differs per path (0 on `GetVersionExA` failure, arg1's pointer on the NT < 4 path, `OutputDebugStringA`'s/`FreeLibrary`'s/`GetProcAddress`'s result elsewhere). The only caller discards it immediately (`mov eax,[esp+0x40]` at `0x0048F6E7` overwrites EAX).
- **Call site (single, proven by `axtj` and by the `E8` recomputation):** WinMain `0x0048F6E2` `e8 99 8d 00 00 call 0x00498480`. The surrounding bytes:
  - `0x0048F6D8 8d 54 24 40 lea edx,[esp+0x40]` — `&out_platform` (arg2)
  - `0x0048F6DC 8d 44 24 38 lea eax,[esp+0x38]` — `&out_dx_version` (arg1)
  - `0x0048F6E0 52 push edx` / `0x0048F6E1 50 push eax` — push order arg2, arg1 (cdecl right-to-left)
  - `0x0048F6E2 call 0x00498480`
  - `0x0048F6E7 8b 44 24 40 mov eax,[esp+0x40]` — reload `out_dx_version` (EAX from the call is discarded)
  - `0x0048F6EB 83 c4 30 add esp,0x30` — caller cleanup, which covers these two pushes plus the frame's other pending pushes
  - `0x0048F6EE 3d 00 07 00 00 cmp eax,0x700` / `0x0048F6F4 73 2e jae 0x48F724` — if the code is `< 0x700`, `MessageBoxA` prints `"This application requires DirectX version %d.%d or greater to run."` (`0x00541E68`) with caption `"Error"` (`0x00541E60`), then `0x0048F71D push 1` / `0x0048F71F call 0x004A5AD5` (termination, see 12.13)
  So the port's WinMain already consumes arg1 as a DirectX version code with a 7.0 floor; that consumption is WinMain's evidence, not the callee's.

**Argument-frame proof (every positive-displacement read in the body):**

| read site | `T` (bytes below `E`) | `[esp+disp]` | entry offset | resolves to |
| --- | ---: | --- | --- | --- |
| `0x4984BD 8b 8c 24 34 01 00 00` | 0x12C | `0x134` | `+8` | **arg2** (`out_platform`) |
| `0x4984E1 8b bc 24 34 01 00 00` | 0x12C | `0x134` | `+8` | **arg2** — kept in `edi` for the rest of the function |
| `0x4984C4 8b 94 24 30 01 00 00` | 0x12C | `0x130` | `+4` | **arg1** (`out_dx_version`) |
| `0x498503 8b 84 24 30 01 00 00` | 0x12C | `0x130` | `+4` | **arg1** |
| `0x498519 8b ac 24 30 01 00 00` | 0x12C | `0x130` | `+4` | **arg1** — kept in `ebp` on the NT 4 path |
| `0x4985A8 8b 8c 24 30 01 00 00` | 0x12C | `0x130` | `+4` | **arg1** |
| `0x4985D5 8b 94 24 30 01 00 00` | 0x12C | `0x130` | `+4` | **arg1** |
| `0x498632 8b 9c 24 30 01 00 00` | 0x12C | `0x130` | `+4` | **arg1** — kept in `ebx` for the rest of the function |

`T` is 0x12C at all eight sites because the frame is `0x11C` plus four saves and every earlier call is stdcall/COM, so each callee pops its own arguments. r2's symbolic names (`var_38h_2`, `[ppv]`, …) do not follow this and are not used here.

**Frame map (the 0x11C bytes are exactly full — no padding):**

| slot | `E` offset | size | role (working name) | proving instruction |
| --- | --- | ---: | --- | --- |
| `pDDraw` | `E-0x11C` | 4 | `LPDIRECTDRAW` (out of `DirectDrawCreate`) | `0x4985FD lea ecx,[esp+0x10]` (T=0x12C) |
| `pSurface` | `E-0x118` | 4 | primary surface (out of `CreateSurface`) | `0x498771 lea edx,[esp+0x14]` (T=0x12C) |
| `pDDraw2` | `E-0x114` | 4 | `IDirectDraw2` (out of QI) | `0x49863D lea edx,[esp+0x18]` (T=0x12C) |
| `pDM` | `E-0x110` | 4 | DirectMusic object (out of `CoCreateInstance`) | `0x49883C lea edx,[esp+0x1C]` (T=0x12C) |
| `pSurf3` | `E-0x10C` | 4 | `IDirectDrawSurface3` (out of QI) | `0x4987B2 lea ecx,[esp+0x20]` (T=0x12C) |
| `pDD7` | `E-0x108` | 4 | `IDirectDraw7` (out of `DirectDrawCreateEx`) | `0x49889A lea edx,[esp+0x28]` (T=0x130) |
| `pSurf4` | `E-0x104` | 4 | `IDirectDrawSurface4` (out of QI) | `0x4987E1 lea edx,[esp+0x28]` (T=0x12C) |
| `ddsd` | `E-0x100` | 0x6C | `DDSURFACEDESC` | `0x498778 lea edx,[esp+0x34]` (T=0x134) as `CreateSurface`'s descriptor |
| `osi` | `E-0x94` | 0x94 | `OSVERSIONINFOA` | `0x4984A8 mov dword [esp+0x9C],0x94` (T=0x130) sets `dwOSVersionInfoSize` at the slot base |

Arithmetic check: 7 dword slots (`0x1C`) + `DDSURFACEDESC` (`0x6C`) + `OSVERSIONINFOA` (`0x94`) = `0x11C`, exactly the frame. The `0x94` size and the `dwPlatformId`/`dwMajorVersion` offsets used later (`+0x10`/`+4`) are what identify `osi`; the `0x6C` size, `dwSize` `+0x0`, `dwFlags` `+0x4` and `ddsCaps.dwCaps` at `+0x68` are what identify `ddsd`. Both identifications are strong structural inference, not retail symbols.

**Where the two outgoing pointers are written (execution order, no path is missing a write it needs):**

| order | site | write | path |
| ---: | --- | --- | --- |
| 1 | `0x4984CF` | `*arg1 = 0` | `GetVersionExA` failed (arg2 was set to 0 first at `0x4984CD`) |
| 2 | `0x49850D` | `*arg1 = 0` | NT, `dwMajorVersion < 4` (arg2 already 2) |
| 3 | `0x498525` | `*arg1 = 0x200` | NT 4, before `LoadLibraryA("DINPUT.DLL")` |
| 4 | `0x49857E` | `*arg1 = 0x300` | NT 4, `GetProcAddress("DirectInputCreateA")` non-NULL |
| 5 | `0x4985B2` | `*arg1 = 0`, `*arg2 = 0` | `LoadLibraryA("DDRAW.DLL")` failed |
| 6 | `0x4985DD`/`0x4985DF` | `*arg1 = 0`, `*arg2 = 0` | `GetProcAddress("DirectDrawCreate")` NULL |
| 7 | `0x498612`/`0x498614` | `*arg1 = 0`, `*arg2 = 0` | `DirectDrawCreate` returned < 0 |
| 8 | `0x498641` | `*arg1 = 0x100` | `DirectDrawCreate` succeeded |
| 9 | `0x49868C` | `*arg1 = 0x200` | `IDirectDraw2` QI succeeded |
| 10 | `0x498704` | `*arg1 = 0x300` | `GetProcAddress("DirectInputCreateA")` non-NULL on the DDRAW path |
| 11 | `0x498758` | `*arg1 = 0` | `SetCooperativeLevel` failed |
| 12 | `0x498797` | `*arg1 = 0` | `CreateSurface` failed |
| 13 | `0x4987E5` | `*arg1 = 0x500` | before the `IDirectDrawSurface3` QI |
| 14 | `0x498816` | `*arg1 = 0x600` | after both surface QIs succeeded |
| 15 | `0x498876` | `*arg1 = 0x601` | after `CoCreateInstance(CLSID_DirectMusic, …)` succeeded |
| 16 | `0x4988B0` | `*arg1 = 0x700` | after `DirectDrawCreateEx(…, IID_IDirectDraw7, …)` succeeded |

So `out_dx_version` is a monotonically-raised code (0, 0x100, 0x200, 0x300, 0x500, 0x600, 0x601, 0x700) whose exact values are retail facts; the meaning *"major.minor DirectX version"* is WinMain's 0x700 comparison plus the probed APIs, not something this function states.

### 12.8 `0x00498480` — basic-block map (38 blocks, 374 instructions)

| # | start | end (excl.) | bytes | insns | terminator |
| ---: | --- | --- | ---: | ---: | --- |
| 1 | `0x00498480` | `0x004984BD` | 61 | 17 | `jne 0x4984da` |
| 2 | `0x004984BD` | `0x004984DA` | 29 | 10 | `ret` |
| 3 | `0x004984DA` | `0x004984F1` | 23 | 4 | `jne 0x49858f` |
| 4 | `0x004984F1` | `0x00498503` | 18 | 4 | `jae 0x498517` |
| 5 | `0x00498503` | `0x00498517` | 20 | 8 | `ret` |
| 6 | `0x00498517` | `0x00498519` | 2 | 1 | `jne 0x498595` |
| 7 | `0x00498519` | `0x00498538` | 31 | 7 | `jne 0x49854e` |
| 8 | `0x00498538` | `0x0049854E` | 22 | 8 | `ret` |
| 9 | `0x0049854E` | `0x00498567` | 25 | 8 | `jne 0x49857d` |
| 10 | `0x00498567` | `0x0049857D` | 22 | 8 | `ret` |
| 11 | `0x0049857D` | `0x0049858F` | 18 | 7 | `ret` |
| 12 | `0x0049858F` | `0x00498595` | 6 | 1 | fall-through; last: `mov dword [edi], 1` |
| 13 | `0x00498595` | `0x004985A8` | 19 | 6 | `jne 0x4985c5` |
| 14 | `0x004985A8` | `0x004985C5` | 29 | 11 | `ret` |
| 15 | `0x004985C5` | `0x004985D5` | 16 | 5 | `jne 0x4985fd` |
| 16 | `0x004985D5` | `0x004985FD` | 40 | 13 | `ret` |
| 17 | `0x004985FD` | `0x0049860A` | 13 | 7 | `jge 0x498632` |
| 18 | `0x0049860A` | `0x00498632` | 40 | 13 | `ret` |
| 19 | `0x00498632` | `0x00498656` | 36 | 11 | `jge 0x49867d` |
| 20 | `0x00498656` | `0x0049867D` | 39 | 14 | `ret` |
| 21 | `0x0049867D` | `0x0049869A` | 29 | 10 | `jne 0x4986c1` |
| 22 | `0x0049869A` | `0x004986C1` | 39 | 14 | `ret` |
| 23 | `0x004986C1` | `0x004986D1` | 16 | 5 | `jne 0x4986fd` |
| 24 | `0x004986D1` | `0x004986FD` | 44 | 17 | `ret` |
| 25 | `0x004986FD` | `0x0049874A` | 77 | 20 | `jge 0x49876f` |
| 26 | `0x0049874A` | `0x0049876F` | 37 | 14 | `ret` |
| 27 | `0x0049876F` | `0x00498785` | 22 | 10 | `jge 0x4987ae` |
| 28 | `0x00498785` | `0x004987AE` | 41 | 15 | `ret` |
| 29 | `0x004987AE` | `0x004987C5` | 23 | 9 | `jge 0x4987dd` |
| 30 | `0x004987C5` | `0x004987DD` | 24 | 12 | `ret` |
| 31 | `0x004987DD` | `0x004987FA` | 29 | 10 | `jge 0x498812` |
| 32 | `0x004987FA` | `0x00498812` | 24 | 12 | `ret` |
| 33 | `0x00498812` | `0x00498859` | 71 | 21 | `jge 0x498872` |
| 34 | `0x00498859` | `0x00498872` | 25 | 10 | `ret` |
| 35 | `0x00498872` | `0x00498898` | 38 | 11 | `je 0x4988bc` |
| 36 | `0x00498898` | `0x004988AC` | 20 | 8 | `jl 0x4988bc` |
| 37 | `0x004988AC` | `0x004988BC` | 16 | 5 | fall-through; last: `call dword [ecx + 8]` |
| 38 | `0x004988BC` | `0x004988CA` | 14 | 8 | `ret` |

The 40 edges are the 38 fall-through edges plus the `je`/`jne`/`jge`/`jl` branches and the two `ret`-terminated paths; `afbj` reports `edges: 40`, `ebbs: 17`.

### 12.9 `0x00498480` — instruction-by-instruction derivation (374 instructions)

| VA | bytes | instruction | exact statement / purpose |
| --- | --- | --- | --- |
| `0x00498480` | `81 ec 1c 01 00 00` | `sub esp, 0x11c` | frame: `sub esp,0x11C` reserves the locals; the two arguments stay above the frame at E+4/E+8 |
| `0x00498486` | `53` | `push ebx` | save callee-saved `ebx` |
| `0x00498487` | `55` | `push ebp` | save callee-saved `ebp` |
| `0x00498488` | `56` | `push esi` | save callee-saved `esi` |
| `0x00498489` | `8d 84 24 94 00 00 00` | `lea eax, [esp + 0x94]` | `eax = &osi` = E-0x94, computed **before** `push edi` (hence disp 0x94, not 0x90) |
| `0x00498490` | `57` | `push edi` | save callee-saved `edi`; frame complete: E-0x11C..E-0x01 are the locals, T=0x12C |
| `0x00498491` | `33 db` | `xor ebx, ebx` | `ebx = NULL`; every following zero store uses it |
| `0x00498493` | `50` | `push eax` | push `&osi` — computed at 0x498489 as E-0x94 |
| `0x00498494` | `89 5c 24 14` | `mov dword [esp + 0x14], ebx` | `pSurface = NULL` (E-0x118) |
| `0x00498498` | `89 5c 24 1c` | `mov dword [esp + 0x1c], ebx` | `pDM = NULL` (E-0x110) |
| `0x0049849C` | `89 5c 24 18` | `mov dword [esp + 0x18], ebx` | `pDDraw2 = NULL` (E-0x114) |
| `0x004984A0` | `89 5c 24 24` | `mov dword [esp + 0x24], ebx` | `pDD7 = NULL` (E-0x108) |
| `0x004984A4` | `89 5c 24 2c` | `mov dword [esp + 0x2c], ebx` | slot E-0x100 = NULL — dead slot, later reused as the `DDSURFACEDESC` base (see 0x498713) |
| `0x004984A8` | `c7 84 24 9c 00 00 00 94 00 00 00` | `mov dword [esp + 0x9c], 0x94` | `osi.dwOSVersionInfoSize = 0x94` (`sizeof(OSVERSIONINFOA)`) |
| `0x004984B3` | `ff 15 08 31 53 00` | `call dword [0x533108]` | `GetVersionExA(&osi)` through IAT `0x533108` |
| `0x004984B9` | `85 c0` | `test eax, eax` | `if (GetVersionExA(&osi) == 0)` — the failure path comes first in the layout |
| `0x004984BB` | `75 1d` | `jne 0x4984da` | `if (GetVersionExA(&osi) == 0)` — the failure path comes first in the layout |
| `0x004984BD` | `8b 8c 24 34 01 00 00` | `mov ecx, dword [esp + 0x134]` | failure path: `ecx = arg2`, `edx = arg1` (both reloaded from E+0x130/E+0x134) |
| `0x004984C4` | `8b 94 24 30 01 00 00` | `mov edx, dword [esp + 0x130]` | failure path: `ecx = arg2`, `edx = arg1` (both reloaded from E+0x130/E+0x134) |
| `0x004984CB` | `5f` | `pop edi` | restore `edi`,`esi` — the two out-pointer registers are already live |
| `0x004984CC` | `5e` | `pop esi` | restore `edi`,`esi` — the two out-pointer registers are already live |
| `0x004984CD` | `89 19` | `mov dword [ecx], ebx` | `*arg2 = 0` **before** `*arg1 = 0` |
| `0x004984CF` | `89 1a` | `mov dword [edx], ebx` | `*arg1 = 0` |
| `0x004984D1` | `5d` | `pop ebp` | `*arg1 = 0` |
| `0x004984D2` | `5b` | `pop ebx` | `*arg1 = 0` |
| `0x004984D3` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | epilogue: `add esp,0x11C; ret` (EAX left as the 0 from `GetVersionExA`) |
| `0x004984D9` | `c3` | `ret` | epilogue: `add esp,0x11C; ret` (EAX left as the 0 from `GetVersionExA`) |
| `0x004984DA` | `8b 84 24 a8 00 00 00` | `mov eax, dword [esp + 0xa8]` | `eax = osi.dwPlatformId` (E-0x84 = `osi`+0x10) |
| `0x004984E1` | `8b bc 24 34 01 00 00` | `mov edi, dword [esp + 0x134]` | `edi = arg2` — held for the whole rest of the function |
| `0x004984E8` | `83 f8 02` | `cmp eax, 2` | `cmp eax,2` → `if (platformId != 2)` |
| `0x004984EB` | `0f 85 9e 00 00 00` | `jne 0x49858f` | `if (osi.dwPlatformId != 2)` (2 = `VER_PLATFORM_WIN32_NT`) |
| `0x004984F1` | `8b 84 24 9c 00 00 00` | `mov eax, dword [esp + 0x9c]` | `eax = osi.dwMajorVersion` (E-0x90 = `osi`+4) |
| `0x004984F8` | `c7 07 02 00 00 00` | `mov dword [edi], 2` | `*arg2 = 2` (NT); this write precedes the version test in instruction order |
| `0x004984FE` | `83 f8 04` | `cmp eax, 4` | `cmp eax,4` → `if (major >= 4)` |
| `0x00498501` | `73 14` | `jae 0x498517` | `if (osi.dwMajorVersion >= 4)` |
| `0x00498503` | `8b 84 24 30 01 00 00` | `mov eax, dword [esp + 0x130]` | NT and major < 4: `eax = arg1`, then `*arg1 = 0`; `*arg2` stays 2 |
| `0x0049850A` | `5f` | `pop edi` | NT and major < 4: `eax = arg1`, then `*arg1 = 0`; `*arg2` stays 2 |
| `0x0049850B` | `5e` | `pop esi` | NT and major < 4: `eax = arg1`, then `*arg1 = 0`; `*arg2` stays 2 |
| `0x0049850C` | `5d` | `pop ebp` | NT and major < 4: `eax = arg1`, then `*arg1 = 0`; `*arg2` stays 2 |
| `0x0049850D` | `89 18` | `mov dword [eax], ebx` | NT and major < 4: `eax = arg1`, then `*arg1 = 0`; `*arg2` stays 2 |
| `0x0049850F` | `5b` | `pop ebx` | NT and major < 4: `eax = arg1`, then `*arg1 = 0`; `*arg2` stays 2 |
| `0x00498510` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | NT and major < 4: `eax = arg1`, then `*arg1 = 0`; `*arg2` stays 2 |
| `0x00498516` | `c3` | `ret` | NT and major < 4: `eax = arg1`, then `*arg1 = 0`; `*arg2` stays 2 |
| `0x00498517` | `75 7c` | `jne 0x498595` | `jne` reuses the `cmp eax,4` flags: only `major == 4` continues |
| `0x00498519` | `8b ac 24 30 01 00 00` | `mov ebp, dword [esp + 0x130]` | NT 4 path: `ebp = arg1`; push `"DINPUT.DLL"`; `*arg1 = 0x200` |
| `0x00498520` | `68 4c 3c 54 00` | `push 0x543c4c` | NT 4 path: `ebp = arg1`; push `"DINPUT.DLL"`; `*arg1 = 0x200` |
| `0x00498525` | `c7 45 00 00 02 00 00` | `mov dword [ebp], 0x200` | NT 4 path: `ebp = arg1`; push `"DINPUT.DLL"`; `*arg1 = 0x200` |
| `0x0049852C` | `ff 15 10 31 53 00` | `call dword [0x533110]` | `esi = LoadLibraryA("DINPUT.DLL")` — `esi` becomes the DInput module handle |
| `0x00498532` | `8b f0` | `mov esi, eax` | `esi = LoadLibraryA("DINPUT.DLL")` — `esi` becomes the DInput module handle |
| `0x00498534` | `3b f3` | `cmp esi, ebx` | `if (esi == NULL)` |
| `0x00498536` | `75 16` | `jne 0x49854e` | `if (esi == NULL)` |
| `0x00498538` | `68 2c 3c 54 00` | `push 0x543c2c` | NT 4 failure: `OutputDebugStringA("Couldn't LoadLibrary DInput\r\n")`; `*arg1` remains 0x200 |
| `0x0049853D` | `ff 15 14 31 53 00` | `call dword [0x533114]` | NT 4 failure: `OutputDebugStringA("Couldn't LoadLibrary DInput\r\n")`; `*arg1` remains 0x200 |
| `0x00498543` | `5f` | `pop edi` | NT 4 failure: `OutputDebugStringA("Couldn't LoadLibrary DInput\r\n")`; `*arg1` remains 0x200 |
| `0x00498544` | `5e` | `pop esi` | NT 4 failure: `OutputDebugStringA("Couldn't LoadLibrary DInput\r\n")`; `*arg1` remains 0x200 |
| `0x00498545` | `5d` | `pop ebp` | NT 4 failure: `OutputDebugStringA("Couldn't LoadLibrary DInput\r\n")`; `*arg1` remains 0x200 |
| `0x00498546` | `5b` | `pop ebx` | NT 4 failure: `OutputDebugStringA("Couldn't LoadLibrary DInput\r\n")`; `*arg1` remains 0x200 |
| `0x00498547` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | NT 4 failure: `OutputDebugStringA("Couldn't LoadLibrary DInput\r\n")`; `*arg1` remains 0x200 |
| `0x0049854D` | `c3` | `ret` | NT 4 failure: `OutputDebugStringA("Couldn't LoadLibrary DInput\r\n")`; `*arg1` remains 0x200 |
| `0x0049854E` | `68 18 3c 54 00` | `push 0x543c18` | push `"DirectInputCreateA"` |
| `0x00498553` | `56` | `push esi` | push `esi` (module handle argument) |
| `0x00498554` | `ff 15 18 31 53 00` | `call dword [0x533118]` | `GetProcAddress(hDInput, "DirectInputCreateA")` |
| `0x0049855A` | `56` | `push esi` | push `esi` again — this push is `FreeLibrary`'s argument, scheduled before the NULL test |
| `0x0049855B` | `8b f8` | `mov edi, eax` | `edi = pfn` (kept only for the NULL test) |
| `0x0049855D` | `ff 15 1c 31 53 00` | `call dword [0x53311c]` | `FreeLibrary(hDInput)` — the handle is closed on both success and failure |
| `0x00498563` | `3b fb` | `cmp edi, ebx` | `if (edi == NULL)` |
| `0x00498565` | `75 16` | `jne 0x49857d` | `if (edi == NULL)` |
| `0x00498567` | `68 f0 3b 54 00` | `push 0x543bf0` | NT 4 failure: `"Couldn't GetProcAddress DInputCreate\r\n"`; `*arg1` remains 0x200 |
| `0x0049856C` | `ff 15 14 31 53 00` | `call dword [0x533114]` | NT 4 failure: `"Couldn't GetProcAddress DInputCreate\r\n"`; `*arg1` remains 0x200 |
| `0x00498572` | `5f` | `pop edi` | NT 4 failure: `"Couldn't GetProcAddress DInputCreate\r\n"`; `*arg1` remains 0x200 |
| `0x00498573` | `5e` | `pop esi` | NT 4 failure: `"Couldn't GetProcAddress DInputCreate\r\n"`; `*arg1` remains 0x200 |
| `0x00498574` | `5d` | `pop ebp` | NT 4 failure: `"Couldn't GetProcAddress DInputCreate\r\n"`; `*arg1` remains 0x200 |
| `0x00498575` | `5b` | `pop ebx` | NT 4 failure: `"Couldn't GetProcAddress DInputCreate\r\n"`; `*arg1` remains 0x200 |
| `0x00498576` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | NT 4 failure: `"Couldn't GetProcAddress DInputCreate\r\n"`; `*arg1` remains 0x200 |
| `0x0049857C` | `c3` | `ret` | NT 4 failure: `"Couldn't GetProcAddress DInputCreate\r\n"`; `*arg1` remains 0x200 |
| `0x0049857D` | `5f` | `pop edi` | NT 4 success: `*arg1 = 0x300`; return. The resolved `DirectInputCreateA` address is **discarded** (no store anywhere) |
| `0x0049857E` | `c7 45 00 00 03 00 00` | `mov dword [ebp], 0x300` | NT 4 success: `*arg1 = 0x300`; return. The resolved `DirectInputCreateA` address is **discarded** (no store anywhere) |
| `0x00498585` | `5e` | `pop esi` | NT 4 success: `*arg1 = 0x300`; return. The resolved `DirectInputCreateA` address is **discarded** (no store anywhere) |
| `0x00498586` | `5d` | `pop ebp` | NT 4 success: `*arg1 = 0x300`; return. The resolved `DirectInputCreateA` address is **discarded** (no store anywhere) |
| `0x00498587` | `5b` | `pop ebx` | NT 4 success: `*arg1 = 0x300`; return. The resolved `DirectInputCreateA` address is **discarded** (no store anywhere) |
| `0x00498588` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | NT 4 success: `*arg1 = 0x300`; return. The resolved `DirectInputCreateA` address is **discarded** (no store anywhere) |
| `0x0049858E` | `c3` | `ret` | NT 4 success: `*arg1 = 0x300`; return. The resolved `DirectInputCreateA` address is **discarded** (no store anywhere) |
| `0x0049858F` | `c7 07 01 00 00 00` | `mov dword [edi], 1` | non-NT (9x): `*arg2 = 1` |
| `0x00498595` | `8b 2d 10 31 53 00` | `mov ebp, dword [0x533110]` | `ebp = LoadLibraryA` (cached IAT value for the rest of the DDRAW block); push `"DDRAW.DLL"` |
| `0x0049859B` | `68 e4 3b 54 00` | `push 0x543be4` | `ebp = LoadLibraryA` (cached IAT value for the rest of the DDRAW block); push `"DDRAW.DLL"` |
| `0x004985A0` | `ff d5` | `call ebp` | `esi = LoadLibraryA("DDRAW.DLL")` — `esi` becomes the DDRAW module handle |
| `0x004985A2` | `8b f0` | `mov esi, eax` | `esi = LoadLibraryA("DDRAW.DLL")` — `esi` becomes the DDRAW module handle |
| `0x004985A4` | `3b f3` | `cmp esi, ebx` | `if (esi == NULL)` |
| `0x004985A6` | `75 1d` | `jne 0x4985c5` | `if (esi == NULL)` |
| `0x004985A8` | `8b 8c 24 30 01 00 00` | `mov ecx, dword [esp + 0x130]` | `ecx = arg1` (the DDRAW-load failure path reloads it) |
| `0x004985AF` | `53` | `push ebx` | push `NULL` — `FreeLibrary`'s argument |
| `0x004985B0` | `89 19` | `mov dword [ecx], ebx` | `*arg1 = 0` |
| `0x004985B2` | `89 1f` | `mov dword [edi], ebx` | `*arg2 = 0` |
| `0x004985B4` | `ff 15 1c 31 53 00` | `call dword [0x53311c]` | `FreeLibrary(NULL)` — retail really calls it with NULL |
| `0x004985BA` | `5f` | `pop edi` | restore `edi` |
| `0x004985BB` | `5e` | `pop esi` | restore `esi` |
| `0x004985BC` | `5d` | `pop ebp` | restore `ebp` |
| `0x004985BD` | `5b` | `pop ebx` | restore `ebx` |
| `0x004985BE` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x004985C4` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |
| `0x004985C5` | `68 d0 3b 54 00` | `push 0x543bd0` | `GetProcAddress(hDDraw, "DirectDrawCreate")`; result tested, then called through EAX |
| `0x004985CA` | `56` | `push esi` | `GetProcAddress(hDDraw, "DirectDrawCreate")`; result tested, then called through EAX |
| `0x004985CB` | `ff 15 18 31 53 00` | `call dword [0x533118]` | `GetProcAddress(hDDraw, "DirectDrawCreate")`; result tested, then called through EAX |
| `0x004985D1` | `3b c3` | `cmp eax, ebx` | `if (pfn == NULL)` |
| `0x004985D3` | `75 28` | `jne 0x4985fd` | `if (pfn == NULL)` |
| `0x004985D5` | `8b 94 24 30 01 00 00` | `mov edx, dword [esp + 0x130]` | `edx = arg1` |
| `0x004985DC` | `56` | `push esi` | push `hDDraw` |
| `0x004985DD` | `89 1a` | `mov dword [edx], ebx` | `*arg1 = 0` |
| `0x004985DF` | `89 1f` | `mov dword [edi], ebx` | `*arg2 = 0` |
| `0x004985E1` | `ff 15 1c 31 53 00` | `call dword [0x53311c]` | `FreeLibrary(hDDraw)` |
| `0x004985E7` | `68 b0 3b 54 00` | `push 0x543bb0` | push `"Couldn't LoadLibrary DDraw\r\n"` — the message does not name the step that failed |
| `0x004985EC` | `ff 15 14 31 53 00` | `call dword [0x533114]` | `OutputDebugStringA(msg)` |
| `0x004985F2` | `5f` | `pop edi` | restore `edi` |
| `0x004985F3` | `5e` | `pop esi` | restore `esi` |
| `0x004985F4` | `5d` | `pop ebp` | restore `ebp` |
| `0x004985F5` | `5b` | `pop ebx` | restore `ebx` |
| `0x004985F6` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x004985FC` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |
| `0x004985FD` | `8d 4c 24 10` | `lea ecx, [esp + 0x10]` | push `NULL` (lpGUID) |
| `0x00498601` | `53` | `push ebx` | push `&pDDraw` — argument order `DirectDrawCreate(NULL, &pDDraw, NULL)` |
| `0x00498602` | `51` | `push ecx` | push `&pDDraw` — argument order `DirectDrawCreate(NULL, &pDDraw, NULL)` |
| `0x00498603` | `53` | `push ebx` | push `&pDDraw` — argument order `DirectDrawCreate(NULL, &pDDraw, NULL)` |
| `0x00498604` | `ff d0` | `call eax` | `hr = DirectDrawCreate(NULL, &pDDraw, NULL)` (3 stdcall arguments, callee pops 0xC) |
| `0x00498606` | `85 c0` | `test eax, eax` | `if (hr < 0)` — signed `jge`/`jl`, not `!= 0` |
| `0x00498608` | `7d 28` | `jge 0x498632` | `if (hr < 0)` — signed `jge`/`jl`, not `!= 0` |
| `0x0049860A` | `8b 94 24 30 01 00 00` | `mov edx, dword [esp + 0x130]` | `edx = arg1` |
| `0x00498611` | `56` | `push esi` | push `hDDraw` |
| `0x00498612` | `89 1a` | `mov dword [edx], ebx` | `*arg1 = 0` |
| `0x00498614` | `89 1f` | `mov dword [edi], ebx` | `*arg2 = 0` |
| `0x00498616` | `ff 15 1c 31 53 00` | `call dword [0x53311c]` | `FreeLibrary(hDDraw)` |
| `0x0049861C` | `68 98 3b 54 00` | `push 0x543b98` | push `"Couldn't create DDraw\r\n"` |
| `0x00498621` | `ff 15 14 31 53 00` | `call dword [0x533114]` | `OutputDebugStringA(msg)` |
| `0x00498627` | `5f` | `pop edi` | restore `edi` |
| `0x00498628` | `5e` | `pop esi` | restore `esi` |
| `0x00498629` | `5d` | `pop ebp` | restore `ebp` |
| `0x0049862A` | `5b` | `pop ebx` | restore `ebx` |
| `0x0049862B` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x00498631` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |
| `0x00498632` | `8b 9c 24 30 01 00 00` | `mov ebx, dword [esp + 0x130]` | `ebx = arg1` |
| `0x00498639` | `8b 44 24 10` | `mov eax, dword [esp + 0x10]` | `eax = pDDraw` |
| `0x0049863D` | `8d 54 24 18` | `lea edx, [esp + 0x18]` | `edx = &pDDraw2` = E-0x114 |
| `0x00498641` | `c7 03 00 01 00 00` | `mov dword [ebx], 0x100` | `*arg1 = 0x100` |
| `0x00498647` | `8b 08` | `mov ecx, dword [eax]` | push `&pDDraw2`, push IID `0x534A60`, push `pDDraw` (this) |
| `0x00498649` | `52` | `push edx` | push `&pDDraw2`, push IID `0x534A60`, push `pDDraw` (this) |
| `0x0049864A` | `68 60 4a 53 00` | `push 0x534a60` | push `&pDDraw2`, push IID `0x534A60`, push `pDDraw` (this) |
| `0x0049864F` | `50` | `push eax` | push `&pDDraw2`, push IID `0x534A60`, push `pDDraw` (this) |
| `0x00498650` | `ff 11` | `call dword [ecx]` | `call [ecx]` = `QueryInterface` (vtable slot 0) |
| `0x00498652` | `85 c0` | `test eax, eax` | `if (hr < 0)` |
| `0x00498654` | `7d 27` | `jge 0x49867d` | `if (hr < 0)` |
| `0x00498656` | `8b 44 24 10` | `mov eax, dword [esp + 0x10]` | `eax = pDDraw` (E-0x11C) |
| `0x0049865A` | `50` | `push eax` | push `pDDraw` |
| `0x0049865B` | `8b 08` | `mov ecx, dword [eax]` | `ecx = pDDraw->lpVtbl` |
| `0x0049865D` | `ff 51 08` | `call dword [ecx + 8]` | `pDDraw->Release()` — slot 8 |
| `0x00498660` | `56` | `push esi` | push `hDDraw` |
| `0x00498661` | `ff 15 1c 31 53 00` | `call dword [0x53311c]` | `FreeLibrary(hDDraw)` |
| `0x00498667` | `68 80 3b 54 00` | `push 0x543b80` | push `"Couldn't QI DDraw2\r\n"` |
| `0x0049866C` | `ff 15 14 31 53 00` | `call dword [0x533114]` | `OutputDebugStringA(msg)` |
| `0x00498672` | `5f` | `pop edi` | restore `edi` |
| `0x00498673` | `5e` | `pop esi` | restore `esi` |
| `0x00498674` | `5d` | `pop ebp` | restore `ebp` |
| `0x00498675` | `5b` | `pop ebx` | restore `ebx` |
| `0x00498676` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x0049867C` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |
| `0x0049867D` | `8b 44 24 18` | `mov eax, dword [esp + 0x18]` | read `pDDraw2` (E-0x114), push it, then `pDDraw2->Release()` — the QI result is released immediately |
| `0x00498681` | `50` | `push eax` | read `pDDraw2` (E-0x114), push it, then `pDDraw2->Release()` — the QI result is released immediately |
| `0x00498682` | `8b 10` | `mov edx, dword [eax]` | read `pDDraw2` (E-0x114), push it, then `pDDraw2->Release()` — the QI result is released immediately |
| `0x00498684` | `ff 52 08` | `call dword [edx + 8]` | `Release` (slot 8) on `pDDraw2` |
| `0x00498687` | `68 4c 3c 54 00` | `push 0x543c4c` | push `"DINPUT.DLL"`; `*arg1 = 0x200` (the write precedes the `LoadLibraryA` call in instruction order) |
| `0x0049868C` | `c7 03 00 02 00 00` | `mov dword [ebx], 0x200` | push `"DINPUT.DLL"`; `*arg1 = 0x200` (the write precedes the `LoadLibraryA` call in instruction order) |
| `0x00498692` | `ff d5` | `call ebp` | `edi = LoadLibraryA("DINPUT.DLL")` |
| `0x00498694` | `8b f8` | `mov edi, eax` | `edi = LoadLibraryA("DINPUT.DLL")` — `edi` now the DInput handle |
| `0x00498696` | `85 ff` | `test edi, edi` | `if (edi == NULL)` |
| `0x00498698` | `75 27` | `jne 0x4986c1` | `if (edi == NULL)` |
| `0x0049869A` | `68 2c 3c 54 00` | `push 0x543c2c` | push `"Couldn't LoadLibrary DInput\r\n"` |
| `0x0049869F` | `ff 15 14 31 53 00` | `call dword [0x533114]` | `OutputDebugStringA(msg)` |
| `0x004986A5` | `8b 44 24 10` | `mov eax, dword [esp + 0x10]` | `eax = pDDraw` (E-0x11C) |
| `0x004986A9` | `50` | `push eax` | push `pDDraw` |
| `0x004986AA` | `8b 08` | `mov ecx, dword [eax]` | `ecx = pDDraw->lpVtbl` |
| `0x004986AC` | `ff 51 08` | `call dword [ecx + 8]` | `pDDraw->Release()` — slot 8 |
| `0x004986AF` | `56` | `push esi` | push `hDDraw` |
| `0x004986B0` | `ff 15 1c 31 53 00` | `call dword [0x53311c]` | `FreeLibrary(hDDraw)` |
| `0x004986B6` | `5f` | `pop edi` | restore `edi` |
| `0x004986B7` | `5e` | `pop esi` | restore `esi` |
| `0x004986B8` | `5d` | `pop ebp` | restore `ebp` |
| `0x004986B9` | `5b` | `pop ebx` | restore `ebx` |
| `0x004986BA` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x004986C0` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |
| `0x004986C1` | `68 18 3c 54 00` | `push 0x543c18` | push `"DirectInputCreateA"`, push `edi` |
| `0x004986C6` | `57` | `push edi` | push `"DirectInputCreateA"`, push `edi` |
| `0x004986C7` | `ff 15 18 31 53 00` | `call dword [0x533118]` | `GetProcAddress(hDInput, "DirectInputCreateA")` |
| `0x004986CD` | `85 c0` | `test eax, eax` | `if (result == NULL)` |
| `0x004986CF` | `75 2c` | `jne 0x4986fd` | `if (result == NULL)` |
| `0x004986D1` | `57` | `push edi` | push `hDInput` (still in `edi`) — `FreeLibrary`'s argument |
| `0x004986D2` | `8b 3d 1c 31 53 00` | `mov edi, dword [0x53311c]` | `edi = FreeLibrary` (the IAT value is cached; the handle is now only on the stack) |
| `0x004986D8` | `ff d7` | `call edi` | `FreeLibrary(hDInput)` |
| `0x004986DA` | `56` | `push esi` | push `hDDraw` (`esi`) |
| `0x004986DB` | `ff d7` | `call edi` | `FreeLibrary(hDDraw)` |
| `0x004986DD` | `8b 44 24 10` | `mov eax, dword [esp + 0x10]` | `eax = pDDraw` (E-0x11C) |
| `0x004986E1` | `50` | `push eax` | push `pDDraw` |
| `0x004986E2` | `8b 10` | `mov edx, dword [eax]` | `edx = pDDraw->lpVtbl` |
| `0x004986E4` | `ff 52 08` | `call dword [edx + 8]` | `pDDraw->Release()` — slot 8 |
| `0x004986E7` | `68 f0 3b 54 00` | `push 0x543bf0` | push `"Couldn't GetProcAddress DInputCreate\r\n"` |
| `0x004986EC` | `ff 15 14 31 53 00` | `call dword [0x533114]` | `OutputDebugStringA(msg)` |
| `0x004986F2` | `5f` | `pop edi` | restore `edi` |
| `0x004986F3` | `5e` | `pop esi` | restore `esi` |
| `0x004986F4` | `5d` | `pop ebp` | restore `ebp` |
| `0x004986F5` | `5b` | `pop ebx` | restore `ebx` |
| `0x004986F6` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x004986FC` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |
| `0x004986FD` | `8b 2d 1c 31 53 00` | `mov ebp, dword [0x53311c]` | `ebp = FreeLibrary` (cached); push `edi` (its argument); `*arg1 = 0x300` |
| `0x00498703` | `57` | `push edi` | `ebp = FreeLibrary` (cached); push `edi` (its argument); `*arg1 = 0x300` |
| `0x00498704` | `c7 03 00 03 00 00` | `mov dword [ebx], 0x300` | `ebp = FreeLibrary` (cached); push `edi` (its argument); `*arg1 = 0x300` |
| `0x0049870A` | `ff d5` | `call ebp` | `FreeLibrary(hDInput)` |
| `0x0049870C` | `b9 1b 00 00 00` | `mov ecx, 0x1b` | `ecx = 0x1B` dwords = 0x6C bytes — the `DDSURFACEDESC` clear count |
| `0x00498711` | `33 c0` | `xor eax, eax` | `eax = 0` — the value `rep stosd` fills with |
| `0x00498713` | `8d 7c 24 2c` | `lea edi, [esp + 0x2c]` | `lea edi,[esp+0x2C]` = E-0x100 **before** the `push 8`, so the clear address does not shift |
| `0x00498717` | `6a 08` | `push 8` | `push 8` — the `SetCooperativeLevel` flag argument, hoisted ahead of the struct clear (-O3 scheduling) |
| `0x00498719` | `f3 ab` | `rep stosd dword es:[edi], eax` | `rep stosd`: write 0 to 27 dwords at E-0x100..E-0x95 (`memset(&ddsd,0,0x6C)`) |
| `0x0049871B` | `8b 44 24 14` | `mov eax, dword [esp + 0x14]` | `eax = pDDraw` (E-0x11C; disp 0x14 because `push 8` is live) |
| `0x0049871F` | `c7 44 24 30 6c 00 00 00` | `mov dword [esp + 0x30], 0x6c` | `ddsd.dwSize = 0x6C` |
| `0x00498727` | `c7 44 24 34 01 00 00 00` | `mov dword [esp + 0x34], 1` | `ddsd.dwFlags = 1` (`DDSD_CAPS`) |
| `0x0049872F` | `c7 84 24 98 00 00 00 00 02 00 00` | `mov dword [esp + 0x98], 0x200` | `ddsd.ddsCaps.dwCaps = 0x200` (`DDSCAPS_PRIMARYSURFACE`, dword at DDSD+0x68 = E-0x98) |
| `0x0049873A` | `8b 08` | `mov ecx, dword [eax]` | `ecx = pDDraw->lpVtbl` |
| `0x0049873C` | `6a 00` | `push 0` | push `NULL` (hWnd) |
| `0x0049873E` | `50` | `push eax` | push `pDDraw` (this) |
| `0x0049873F` | `ff 51 50` | `call dword [ecx + 0x50]` | `call [ecx+0x50]` = `SetCooperativeLevel` |
| `0x00498742` | `85 c0` | `test eax, eax` | `if (hr < 0)`: `test eax,eax` sets the flags, then `mov eax,[esp+0x10]` reloads `pDDraw` **without touching the flags**, and `jge` uses the test's result |
| `0x00498744` | `8b 44 24 10` | `mov eax, dword [esp + 0x10]` | `if (hr < 0)`: `test eax,eax` sets the flags, then `mov eax,[esp+0x10]` reloads `pDDraw` **without touching the flags**, and `jge` uses the test's result |
| `0x00498748` | `7d 25` | `jge 0x49876f` | `jge` on the `SetCooperativeLevel` HRESULT (the reloaded pointer is not tested) |
| `0x0049874A` | `8b 10` | `mov edx, dword [eax]` | `edx = pDDraw->lpVtbl` |
| `0x0049874C` | `50` | `push eax` | push `pDDraw` |
| `0x0049874D` | `ff 52 08` | `call dword [edx + 8]` | `pDDraw->Release()` — slot 8 |
| `0x00498750` | `56` | `push esi` | push `hDDraw` |
| `0x00498751` | `ff d5` | `call ebp` | `FreeLibrary(hDDraw)` |
| `0x00498753` | `68 64 3b 54 00` | `push 0x543b64` | push `"Couldn't Set coop level\r\n"` |
| `0x00498758` | `c7 03 00 00 00 00` | `mov dword [ebx], 0` | `*arg1 = 0` — written *after* the FreeLibrary, *before* the message call |
| `0x0049875E` | `ff 15 14 31 53 00` | `call dword [0x533114]` | `OutputDebugStringA(msg)` |
| `0x00498764` | `5f` | `pop edi` | restore `edi` |
| `0x00498765` | `5e` | `pop esi` | restore `esi` |
| `0x00498766` | `5d` | `pop ebp` | restore `ebp` |
| `0x00498767` | `5b` | `pop ebx` | restore `ebx` |
| `0x00498768` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x0049876E` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |
| `0x0049876F` | `8b 08` | `mov ecx, dword [eax]` | `ecx = pDDraw->lpVtbl` |
| `0x00498771` | `8d 54 24 14` | `lea edx, [esp + 0x14]` | `edx = &pSurface` = E-0x118 |
| `0x00498775` | `6a 00` | `push 0` | push `NULL` (pUnkOuter) |
| `0x00498777` | `52` | `push edx` | push `&pSurface` |
| `0x00498778` | `8d 54 24 34` | `lea edx, [esp + 0x34]` | `edx = &ddsd` = E-0x100 (disp 0x34 because two pushes are on the stack) |
| `0x0049877C` | `52` | `push edx` | push `&ddsd` |
| `0x0049877D` | `50` | `push eax` | push `pDDraw` (this) |
| `0x0049877E` | `ff 51 18` | `call dword [ecx + 0x18]` | `call [ecx+0x18]` = `CreateSurface` |
| `0x00498781` | `85 c0` | `test eax, eax` | `if (hr < 0)` |
| `0x00498783` | `7d 29` | `jge 0x4987ae` | `if (hr < 0)` |
| `0x00498785` | `8b 44 24 10` | `mov eax, dword [esp + 0x10]` | `eax = pDDraw` (E-0x11C) |
| `0x00498789` | `50` | `push eax` | push `pDDraw` |
| `0x0049878A` | `8b 08` | `mov ecx, dword [eax]` | `ecx = pDDraw->lpVtbl` |
| `0x0049878C` | `ff 51 08` | `call dword [ecx + 8]` | `pDDraw->Release()` — slot 8 |
| `0x0049878F` | `56` | `push esi` | push `hDDraw` |
| `0x00498790` | `ff d5` | `call ebp` | `FreeLibrary(hDDraw)` |
| `0x00498792` | `68 48 3b 54 00` | `push 0x543b48` | push `"Couldn't CreateSurface\r\n"` |
| `0x00498797` | `c7 03 00 00 00 00` | `mov dword [ebx], 0` | `*arg1 = 0` — again after the FreeLibrary, before the message call |
| `0x0049879D` | `ff 15 14 31 53 00` | `call dword [0x533114]` | `OutputDebugStringA(msg)` |
| `0x004987A3` | `5f` | `pop edi` | restore `edi` |
| `0x004987A4` | `5e` | `pop esi` | restore `esi` |
| `0x004987A5` | `5d` | `pop ebp` | restore `ebp` |
| `0x004987A6` | `5b` | `pop ebx` | restore `ebx` |
| `0x004987A7` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x004987AD` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |
| `0x004987AE` | `8b 44 24 14` | `mov eax, dword [esp + 0x14]` | `eax = pSurface` (E-0x118) |
| `0x004987B2` | `8d 4c 24 20` | `lea ecx, [esp + 0x20]` | `ecx = &pSurf3` = E-0x10C |
| `0x004987B6` | `51` | `push ecx` | push `&pSurf3` |
| `0x004987B7` | `68 40 4a 53 00` | `push 0x534a40` | push IID `0x534A40` (`IID_IDirectDrawSurface3`) |
| `0x004987BC` | `8b 10` | `mov edx, dword [eax]` | `edx = pSurface->lpVtbl` |
| `0x004987BE` | `50` | `push eax` | push `pSurface` (this) |
| `0x004987BF` | `ff 12` | `call dword [edx]` | `call [edx]` = `QueryInterface` (slot 0) |
| `0x004987C1` | `85 c0` | `test eax, eax` | `if (hr < 0)` |
| `0x004987C3` | `7d 18` | `jge 0x4987dd` | `if (hr < 0)` |
| `0x004987C5` | `8b 44 24 10` | `mov eax, dword [esp + 0x10]` | `eax = pDDraw` (E-0x11C) |
| `0x004987C9` | `50` | `push eax` | push `pDDraw` |
| `0x004987CA` | `8b 10` | `mov edx, dword [eax]` | `edx = pDDraw->lpVtbl` |
| `0x004987CC` | `ff 52 08` | `call dword [edx + 8]` | `pDDraw->Release()` — slot 8 |
| `0x004987CF` | `56` | `push esi` | push `hDDraw` |
| `0x004987D0` | `ff d5` | `call ebp` | `FreeLibrary(hDDraw)` |
| `0x004987D2` | `5f` | `pop edi` | restore `edi` |
| `0x004987D3` | `5e` | `pop esi` | restore `esi` |
| `0x004987D4` | `5d` | `pop ebp` | restore `ebp` |
| `0x004987D5` | `5b` | `pop ebx` | restore `ebx` |
| `0x004987D6` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x004987DC` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |
| `0x004987DD` | `8b 44 24 14` | `mov eax, dword [esp + 0x14]` | `eax = pSurface` |
| `0x004987E1` | `8d 54 24 28` | `lea edx, [esp + 0x28]` | `edx = &pSurf4` = E-0x104 |
| `0x004987E5` | `c7 03 00 05 00 00` | `mov dword [ebx], 0x500` | `*arg1 = 0x500` |
| `0x004987EB` | `52` | `push edx` | push `&pSurf4` |
| `0x004987EC` | `8b 08` | `mov ecx, dword [eax]` | `ecx = pSurface->lpVtbl` |
| `0x004987EE` | `68 30 4a 53 00` | `push 0x534a30` | push IID `0x534A30` (`IID_IDirectDrawSurface4`) |
| `0x004987F3` | `50` | `push eax` | push `pSurface` |
| `0x004987F4` | `ff 11` | `call dword [ecx]` | `call [ecx]` = `QueryInterface` (slot 0) |
| `0x004987F6` | `85 c0` | `test eax, eax` | `if (hr < 0)` |
| `0x004987F8` | `7d 18` | `jge 0x498812` | `if (hr < 0)` |
| `0x004987FA` | `8b 44 24 10` | `mov eax, dword [esp + 0x10]` | `eax = pDDraw` (E-0x11C) |
| `0x004987FE` | `50` | `push eax` | push `pDDraw` |
| `0x004987FF` | `8b 08` | `mov ecx, dword [eax]` | `ecx = pDDraw->lpVtbl` |
| `0x00498801` | `ff 51 08` | `call dword [ecx + 8]` | `pDDraw->Release()` — slot 8 |
| `0x00498804` | `56` | `push esi` | push `hDDraw` |
| `0x00498805` | `ff d5` | `call ebp` | `FreeLibrary(hDDraw)` |
| `0x00498807` | `5f` | `pop edi` | restore `edi` |
| `0x00498808` | `5e` | `pop esi` | restore `esi` |
| `0x00498809` | `5d` | `pop ebp` | restore `ebp` |
| `0x0049880A` | `5b` | `pop ebx` | restore `ebx` |
| `0x0049880B` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x00498811` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |
| `0x00498812` | `8b 44 24 14` | `mov eax, dword [esp + 0x14]` | `eax = pSurface` |
| `0x00498816` | `c7 03 00 06 00 00` | `mov dword [ebx], 0x600` | `*arg1 = 0x600` |
| `0x0049881C` | `50` | `push eax` | push `pSurface` (its `Release` argument) |
| `0x0049881D` | `8b 10` | `mov edx, dword [eax]` | `edx = pSurface->lpVtbl` |
| `0x0049881F` | `ff 52 08` | `call dword [edx + 8]` | `Release` (slot 8) on `pSurface` |
| `0x00498822` | `8b 44 24 10` | `mov eax, dword [esp + 0x10]` | `eax = pDDraw` (E-0x11C) |
| `0x00498826` | `50` | `push eax` | push `pDDraw` |
| `0x00498827` | `8b 08` | `mov ecx, dword [eax]` | `ecx = pDDraw->lpVtbl` |
| `0x00498829` | `ff 51 08` | `call dword [ecx + 8]` | `Release` (slot 8) on `pDDraw` |
| `0x0049882C` | `6a 00` | `push 0` | push `NULL` (`CoInitialize` argument) |
| `0x0049882E` | `c7 44 24 20 00 00 00 00` | `mov dword [esp + 0x20], 0` | `pDM = NULL` (E-0x110) — the only out-pointer zeroed immediately before its call |
| `0x00498836` | `ff 15 1c 32 53 00` | `call dword [0x53321c]` | `CoInitialize(NULL)` |
| `0x0049883C` | `8d 54 24 1c` | `lea edx, [esp + 0x1c]` | `edx = &pDM` = E-0x110 |
| `0x00498840` | `52` | `push edx` | push `&pDM` (ppv) |
| `0x00498841` | `68 e0 48 53 00` | `push 0x5348e0` | push IID `0x5348E0` (`IID_IDirectMusic`) |
| `0x00498846` | `6a 01` | `push 1` | push `1` (`CLSCTX_INPROC_SERVER`) |
| `0x00498848` | `6a 00` | `push 0` | push `NULL` (pUnkOuter) |
| `0x0049884A` | `68 f0 48 53 00` | `push 0x5348f0` | push CLSID `0x5348F0` (`CLSID_DirectMusic`) |
| `0x0049884F` | `ff 15 18 32 53 00` | `call dword [0x533218]` | `CoCreateInstance(...)` — 5 stdcall args |
| `0x00498855` | `85 c0` | `test eax, eax` | `if (hr < 0)` |
| `0x00498857` | `7d 19` | `jge 0x498872` | `if (hr < 0)` |
| `0x00498859` | `68 24 3b 54 00` | `push 0x543b24` | push `"Couldn't create CLSID_DirectMusic\r\n"` |
| `0x0049885E` | `ff 15 14 31 53 00` | `call dword [0x533114]` | `OutputDebugStringA(msg)` |
| `0x00498864` | `56` | `push esi` | push `hDDraw` |
| `0x00498865` | `ff d5` | `call ebp` | `FreeLibrary(hDDraw)` — no `CoUninitialize`, no `pDM` release on this path |
| `0x00498867` | `5f` | `pop edi` | restore `edi` |
| `0x00498868` | `5e` | `pop esi` | restore `esi` |
| `0x00498869` | `5d` | `pop ebp` | restore `ebp` |
| `0x0049886A` | `5b` | `pop ebx` | restore `ebx` |
| `0x0049886B` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x00498871` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |
| `0x00498872` | `8b 44 24 1c` | `mov eax, dword [esp + 0x1c]` | `eax = pDM` (E-0x110) |
| `0x00498876` | `c7 03 01 06 00 00` | `mov dword [ebx], 0x601` | `*arg1 = 0x601` |
| `0x0049887C` | `50` | `push eax` | push `pDM` |
| `0x0049887D` | `8b 08` | `mov ecx, dword [eax]` | `ecx = pDM->lpVtbl` |
| `0x0049887F` | `ff 51 08` | `call dword [ecx + 8]` | `Release` (slot 8) on `pDM` |
| `0x00498882` | `ff 15 20 32 53 00` | `call dword [0x533220]` | `CoUninitialize()` |
| `0x00498888` | `68 10 3b 54 00` | `push 0x543b10` | push `"DirectDrawCreateEx"`, push `esi` (hDDraw) |
| `0x0049888D` | `56` | `push esi` | push `"DirectDrawCreateEx"`, push `esi` (hDDraw) |
| `0x0049888E` | `ff 15 18 31 53 00` | `call dword [0x533118]` | `GetProcAddress(hDDraw, "DirectDrawCreateEx")` |
| `0x00498894` | `85 c0` | `test eax, eax` | `if (pfn == NULL)` — jumps directly into the shared `FreeLibrary(hDDraw); return` tail |
| `0x00498896` | `74 24` | `je 0x4988bc` | `if (pfn == NULL)` — jumps directly into the shared `FreeLibrary(hDDraw); return` tail |
| `0x00498898` | `6a 00` | `push 0` | push `NULL` (pUnkOuter) |
| `0x0049889A` | `8d 54 24 28` | `lea edx, [esp + 0x28]` | `edx = &pDD7` = E-0x108 (disp 0x28 because `push 0` is on the stack) |
| `0x0049889E` | `68 50 4a 53 00` | `push 0x534a50` | push IID `0x534A50` (`IID_IDirectDraw7`) |
| `0x004988A3` | `52` | `push edx` | push `&pDD7` |
| `0x004988A4` | `6a 00` | `push 0` | push `NULL` (lpGUID) |
| `0x004988A6` | `ff d0` | `call eax` | `DirectDrawCreateEx(NULL, &pDD7, IID, NULL)` |
| `0x004988A8` | `85 c0` | `test eax, eax` | `if (hr < 0)` — falls into the shared `FreeLibrary` tail, `*arg1` stays 0x601 |
| `0x004988AA` | `7c 10` | `jl 0x4988bc` | `if (hr < 0)` — falls into the shared `FreeLibrary` tail, `*arg1` stays 0x601 |
| `0x004988AC` | `8b 44 24 24` | `mov eax, dword [esp + 0x24]` | `eax = pDD7` (E-0x108) |
| `0x004988B0` | `c7 03 00 07 00 00` | `mov dword [ebx], 0x700` | `*arg1 = 0x700` |
| `0x004988B6` | `50` | `push eax` | push `pDD7` |
| `0x004988B7` | `8b 08` | `mov ecx, dword [eax]` | `ecx = pDD7->lpVtbl` |
| `0x004988B9` | `ff 51 08` | `call dword [ecx + 8]` | `Release` (slot 8) on `pDD7` |
| `0x004988BC` | `56` | `push esi` | push `esi` (`FreeLibrary` argument = hDDraw) |
| `0x004988BD` | `ff d5` | `call ebp` | `FreeLibrary(hDDraw)` |
| `0x004988BF` | `5f` | `pop edi` | restore `edi` |
| `0x004988C0` | `5e` | `pop esi` | restore `esi` |
| `0x004988C1` | `5d` | `pop ebp` | restore `ebp` |
| `0x004988C2` | `5b` | `pop ebx` | restore `ebx` |
| `0x004988C3` | `81 c4 1c 01 00 00` | `add esp, 0x11c` | `add esp,0x11C` — release the frame |
| `0x004988C9` | `c3` | `ret` | `ret` (cdecl: the caller removes the two arguments) |

### 12.10 `0x00498480` — retail-order statements, globals, constants and strings

Statement order follows retail instruction order exactly; each entry gives the instruction range that proves it.

```c
/* working names only; no retail symbol is proven for anything here */
void f_498480(void** out_dx_version, void** out_platform)   /* E+4, E+8; cdecl */
{
    LPDIRECTDRAW  pDDraw   = ?;      /* E-0x11C: never pre-zeroed; DirectDrawCreate writes it */
    LPDIRECTDRAW  pSurface = NULL;   /* E-0x118 */
    IUnknown*     pDDraw2  = NULL;   /* E-0x114 */
    IUnknown*     pDM      = NULL;   /* E-0x110 */
    IUnknown*     pSurf3   = NULL;   /* E-0x10C */
    IUnknown*     pDD7     = NULL;   /* E-0x108 */
    IUnknown*     pSurf4   = NULL;   /* E-0x104 */
    DDSURFACEDESC ddsd;              /* E-0x100, 0x6C */
    OSVERSIONINFOA osi;              /* E-0x94, 0x94 */

    /* 0x4984A4 also zeroes the dword at E-0x100, the slot ddsd later reuses; that early store is dead */
    osi.dwOSVersionInfoSize = 0x94;                  /* 0x4984A8 */
    if (!GetVersionExA(&osi)) {                      /* 0x4984B3, 0x4984B9, 0x4984BB */
        *out_platform = 0;                           /* 0x4984CD — written before arg1 */
        *out_dx_version = 0;                         /* 0x4984CF */
        return;                                      /* 0x4984D9 */
    }
    if (osi.dwPlatformId != 2)                       /* 0x4984DA, 0x4984E8, 0x4984EB: non-NT */
        *out_platform = 1;                           /* 0x49858F */
    else {                                           /* VER_PLATFORM_WIN32_NT */
        DWORD major = osi.dwMajorVersion;            /* 0x4984F1 */
        *out_platform = 2;                           /* 0x4984F8 — before the version test */
        if (major < 4) {                             /* 0x4984FE, 0x498501 */
            *out_dx_version = 0;                     /* 0x49850D */
            return;                                  /* 0x498516 */
        }
        if (major == 4) {                            /* 0x498517: reuses the cmp eax,4 flags */
            *out_dx_version = 0x200;                 /* 0x498525 */
            hDInput = LoadLibraryA("DINPUT.DLL");      /* 0x498520 push, 0x49852C call */
            if (hDInput == NULL) {                   /* 0x498534, 0x498536 */
                OutputDebugStringA("Couldn't LoadLibrary DInput\r\n");   /* 0x498538, 0x49853D */
                return;                              /* 0x49854D */
            }
            pfn = GetProcAddress(hDInput, "DirectInputCreateA");  /* 0x49854E, 0x498553, 0x498554 */
            FreeLibrary(hDInput);                    /* 0x49855A push, 0x49855D call */
            if (pfn == NULL) {                       /* 0x498563, 0x498565 */
                OutputDebugStringA("Couldn't GetProcAddress DInputCreate\r\n"); /* 0x498567, 0x49856C */
                return;                              /* 0x49857C */
            }
            *out_dx_version = 0x300;                 /* 0x49857E — pfn is discarded, never stored */
            return;                                  /* 0x49858E */
        }
    }

    /* DDRAW path (non-NT, or NT with major > 4) */
    hDDraw = LoadLibraryA("DDRAW.DLL");              /* 0x498595 caches the IAT entry, 0x49859B push, 0x4985A0 call */
    if (hDDraw == NULL) {                            /* 0x4985A4, 0x4985A6 */
        *out_dx_version = 0;                         /* 0x4985B2 */
        *out_platform = 0;                           /* 0x4985B2 */
        FreeLibrary(NULL);                           /* 0x4985AF push 0, 0x4985B4 call — retail really calls it with NULL */
        return;                                      /* 0x4985C4 */
    }
    pfn = GetProcAddress(hDDraw, "DirectDrawCreate");     /* 0x4985C5, 0x4985CB */
    if (pfn == NULL) {                               /* 0x4985D1, 0x4985D3 */
        *out_dx_version = 0;                         /* 0x4985DD */
        *out_platform = 0;                           /* 0x4985DF */
        FreeLibrary(hDDraw);                         /* 0x4985DC push, 0x4985E1 call */
        OutputDebugStringA("Couldn't LoadLibrary DDraw\r\n");   /* 0x4985E7, 0x4985EC — message name does not match this step */
        return;                                      /* 0x4985FC */
    }
    hr = DirectDrawCreate(NULL, &pDDraw, NULL);      /* 0x4985FD push NULL, 0x498601 push &pDDraw, 0x4985FD..0x498604 */
    if (hr < 0) {                                    /* 0x498606, 0x498608 — signed jge */
        *out_dx_version = 0; *out_platform = 0;      /* 0x498612, 0x498614 */
        FreeLibrary(hDDraw);                         /* 0x498616 */
        OutputDebugStringA("Couldn't create DDraw\r\n");        /* 0x49861C, 0x498621 */
        return;                                      /* 0x498631 */
    }
    *out_dx_version = 0x100;                         /* 0x498641 */
    hr = pDDraw->QueryInterface(IID_IDirectDraw2, &pDDraw2);  /* 0x498647..0x498650, vtable slot 0 */
    if (hr < 0) {                                    /* 0x498652, 0x498654 */
        pDDraw->Release();                           /* 0x49865D, slot 8 */
        FreeLibrary(hDDraw);                         /* 0x498661 */
        OutputDebugStringA("Couldn't QI DDraw2\r\n");           /* 0x49866C */
        return;                                      /* 0x49867C */
    }
    pDDraw2->Release();                              /* 0x498684, slot 8 — released immediately after QI */
    *out_dx_version = 0x200;                         /* 0x49868C */
    hDInput = LoadLibraryA("DINPUT.DLL");              /* 0x498687 push, 0x498692 call */
    if (hDInput == NULL) {                           /* 0x498696, 0x498698 */
        OutputDebugStringA("Couldn't LoadLibrary DInput\r\n");   /* 0x49869A, 0x49869F */
        pDDraw->Release();                           /* 0x4986AC */
        FreeLibrary(hDDraw);                         /* 0x4986B0 */
        return;                                      /* 0x4986C0 */
    }
    pfn = GetProcAddress(hDInput, "DirectInputCreateA");  /* 0x4986C1, 0x4986C7 */
    if (pfn == NULL) {                               /* 0x4986CD, 0x4986CF */
        FreeLibrary(hDInput);                        /* 0x4986D8 */
        FreeLibrary(hDDraw);                         /* 0x4986DB */
        pDDraw->Release();                           /* 0x4986E4 */
        OutputDebugStringA("Couldn't GetProcAddress DInputCreate\r\n"); /* 0x4986E7, 0x4986EC */
        return;                                      /* 0x4986FC */
    }
    *out_dx_version = 0x300;                         /* 0x498704 — written before the FreeLibrary call */
    FreeLibrary(hDInput);                            /* 0x49870A — the resolved pfn is again discarded */
    memset(&ddsd, 0, 0x6C);                          /* 0x49870C..0x498719: 27 dwords via rep stosd */
    ddsd.dwSize = 0x6C;                              /* 0x49871F */
    ddsd.dwFlags = 1;                                /* 0x498727, DDSD_CAPS */
    ddsd.ddsCaps.dwCaps = 0x200;                     /* 0x49872F, DDSCAPS_PRIMARYSURFACE at DDSD+0x68 */
    hr = pDDraw->SetCooperativeLevel(NULL, 8);       /* 0x498717 push 8, 0x49873C/0x49873E pushes, 0x49873F call, slot 0x50 */
    if (hr < 0) {                                    /* 0x498742 test, 0x498744 reload, 0x498748 jge */
        pDDraw->Release();                           /* 0x49874D */
        FreeLibrary(hDDraw);                         /* 0x498751 */
        *out_dx_version = 0;                         /* 0x498758 — after the FreeLibrary, before the message call */
        OutputDebugStringA("Couldn't Set coop level\r\n");      /* 0x498753, 0x49875E */
        return;                                      /* 0x49876E */
    }
    hr = pDDraw->CreateSurface(&ddsd, &pSurface, NULL);   /* 0x498771..0x49877E, slot 0x18 */
    if (hr < 0) {                                    /* 0x498781, 0x498783 */
        pDDraw->Release();                           /* 0x49878C */
        FreeLibrary(hDDraw);                         /* 0x498790 */
        *out_dx_version = 0;                         /* 0x498797 — same placement: after FreeLibrary, before the message */
        OutputDebugStringA("Couldn't CreateSurface\r\n");       /* 0x498792, 0x49879D */
        return;                                      /* 0x4987AD */
    }
    hr = pSurface->QueryInterface(IID_IDirectDrawSurface3, &pSurf3); /* 0x4987AE..0x4987BF, slot 0 */
    if (hr < 0) {                                    /* 0x4987C1, 0x4987C3 — no message on this path */
        pDDraw->Release();                           /* 0x4987CC */
        FreeLibrary(hDDraw);                         /* 0x4987D0 */
        return;                                      /* 0x4987DC */
    }
    *out_dx_version = 0x500;                         /* 0x4987E5 */
    hr = pSurface->QueryInterface(IID_IDirectDrawSurface4, &pSurf4); /* 0x4987DD..0x4987F4, slot 0 */
    if (hr < 0) {                                    /* 0x4987F6, 0x4987F8 */
        pDDraw->Release();                           /* 0x498801 */
        FreeLibrary(hDDraw);                         /* 0x498805 */
        return;                                      /* 0x498811 */
    }
    *out_dx_version = 0x600;                         /* 0x498816 */
    pSurface->Release();                             /* 0x49881F */
    pDDraw->Release();                               /* 0x498829 */
    CoInitialize(NULL);                              /* 0x49882C push 0, 0x498836 call */
    pDM = NULL;                                      /* 0x49882E */
    hr = CoCreateInstance(&CLSID_DirectMusic, NULL, CLSCTX_INPROC_SERVER, &IID_IDirectMusic, &pDM); /* 0x49883C..0x49884F */
    if (hr < 0) {                                    /* 0x498855, 0x498857 */
        OutputDebugStringA("Couldn't create CLSID_DirectMusic\r\n"); /* 0x498859, 0x49885E */
        FreeLibrary(hDDraw);                         /* 0x498865 */
        return;                                      /* 0x498871 — no CoUninitialize, no pDM release */
    }
    *out_dx_version = 0x601;                         /* 0x498876 */
    pDM->Release();                                  /* 0x49887F */
    CoUninitialize();                                /* 0x498882 */
    pfn = GetProcAddress(hDDraw, "DirectDrawCreateEx");   /* 0x498888, 0x49888E */
    if (pfn == NULL)                                 /* 0x498894, 0x498896: jumps into the shared tail */
    { FreeLibrary(hDDraw); return; }                  /* 0x4988BC, 0x4988BD, 0x4988C9 */
    hr = pfn(NULL, &pDD7, &IID_IDirectDraw7, NULL);  /* 0x498898..0x4988A6 */
    if (hr >= 0) {                                   /* 0x4988A8, 0x4988AA */
        *out_dx_version = 0x700;                     /* 0x4988B0 */
        pDD7->Release();                             /* 0x4988B9 */
    }
    FreeLibrary(hDDraw);                             /* 0x4988BD — shared tail for both outcomes */
}
```

Points that a transcription must not "fix", all visible above: the `LoadLibraryA`-missing message on the `GetProcAddress("DirectDrawCreate")` failure path says *"Couldn't LoadLibrary DDraw"*; `FreeLibrary(NULL)` is really called on the DDRAW load-failure path; the NT 4 `DirectInputCreateA` address is resolved and immediately discarded (only its NULL-ness is used); `pSurf3`, `pSurf4`, and `pSurface`/`pDDraw` on several failure paths are never released; `CoUninitialize` is skipped when `CoCreateInstance` fails; the version code is raised in steps and several failure paths leave a *non-zero* code behind.

**Engine globals read/written: none.** There is no absolute-address **store** in the body; the only absolute references are reads of IAT slots and pushes of constant addresses. The IAT slots read (all 4-byte reads, all read-only here):

| IAT slot | import | access form | sites |
| --- | --- | --- | --- |
| `0x00533108` | `KERNEL32!GetVersionExA` | `ff 15 08 31 53 00` | `0x4984B3` |
| `0x00533110` | `KERNEL32!LoadLibraryA` | `ff 15 10 31 53 00`, `8b 2d 10 31 53 00` then `ff d5` | `0x49852C`, `0x498595`, `0x4985A0`, `0x498692` |
| `0x00533114` | `KERNEL32!OutputDebugStringA` | `ff 15 14 31 53 00` | `0x49853D`, `0x49856C`, `0x4985EC`, `0x498621`, `0x49866C`, `0x49869F`, `0x4986EC`, `0x49875E`, `0x49879D`, `0x49885E` |
| `0x00533118` | `KERNEL32!GetProcAddress` | `ff 15 18 31 53 00` | `0x498554`, `0x4985CB`, `0x4986C7`, `0x49888E` |
| `0x0053311C` | `KERNEL32!FreeLibrary` | `ff 15 1C 31 53 00`, `8b 3d 1C 31 53 00`, `8b 2d 1C 31 53 00` | `0x49855D`, `0x4985B4`, `0x4985E1`, `0x498616`, `0x498661`, `0x4986B0`, `0x4986D2`, `0x4986DB`, `0x4986FD`, `0x49870A`, `0x498751`, `0x498790`, `0x4987D0`, `0x498805`, `0x498865`, `0x4988BD` |
| `0x00533218` | `ole32!CoCreateInstance` | `ff 15 18 32 53 00` | `0x49884F` |
| `0x0053321C` | `ole32!CoInitialize` | `ff 15 1C 32 53 00` | `0x498836` |
| `0x00533220` | `ole32!CoUninitialize` | `ff 15 20 32 53 00` | `0x498882` |

**Constant data read (all 16-byte GUID reads and NUL-terminated string pushes; nothing is written):**

| VA | size | kind | pushed/used at |
| --- | ---: | --- | --- |
| `0x005348E0` | 16 | GUID | `0x498841` (CoCreateInstance `riid`) |
| `0x005348F0` | 16 | GUID | `0x49884A` (CoCreateInstance `rclsid`) |
| `0x00534A30` | 16 | GUID | `0x4987EE` (surface QI #2) |
| `0x00534A40` | 16 | GUID | `0x4987B7` (surface QI #1) |
| `0x00534A50` | 16 | GUID | `0x49889E` (`DirectDrawCreateEx`) |
| `0x00534A60` | 16 | GUID | `0x49864A` (`IDirectDraw2` QI) |
| `0x00543B10`, `0x00543B24`, `0x00543B48`, `0x00543B64`, `0x00543B80`, `0x00543B98`, `0x00543BB0`, `0x00543BD0`, `0x00543BE4`, `0x00543BF0`, `0x00543C18`, `0x00543C2C`, `0x00543C4C` | ≤ 40 each | strings | see the table below |

**The GUIDs, as exact image bytes and canonical text.** Bytes are the image bytes; the first three fields are little-endian. The *names* in the right column are DirectX SDK constants that match these byte-for-byte, but they are interpretation, not retail symbols — the only in-binary corroboration is the `"DDraw2"` debug string for `0x534A60` and `"CLSID_DirectMusic"` for `0x5348F0`.

| VA | 16 bytes | GUID text | SDK name (interpretation) |
| --- | --- | --- | --- |
| `0x00534A60` | `e0 f3 a6 b3 43 2b cf 11 a2 de 00 aa 00 b9 33 56` | `{B3A6F3E0-2B43-11CF-A2DE-00AA00B93356}` | IID_IDirectDraw2 (corroborated by "Couldn't QI DDraw2") |
| `0x00534A50` | `c0 5e e6 15 9c 3b d2 11 b9 2f 00 60 97 97 ea 5b` | `{15E65EC0-3B9C-11D2-B92F-00609797EA5B}` | IID_IDirectDraw7 |
| `0x00534A40` | `00 4e 04 da b2 69 d0 11 a1 d5 00 aa 00 b8 df bb` | `{DA044E00-69B2-11D0-A1D5-00AA00B8DFBB}` | IID_IDirectDrawSurface3 |
| `0x00534A30` | `30 86 2b 0b 35 ad d0 11 8e a6 00 60 97 97 ea 5b` | `{0B2B8630-AD35-11D0-8EA6-00609797EA5B}` | IID_IDirectDrawSurface4 |
| `0x005348F0` | `10 9f 6b 63 7d 0c d1 11 95 b2 00 20 af dc 74 21` | `{636B9F10-0C7D-11D1-95B2-0020AFDC7421}` | CLSID_DirectMusic (corroborated by "CLSID_DirectMusic") |
| `0x005348E0` | `5a 11 36 65 2d 7b d2 11 ba 18 00 00 f8 75 ac 12` | `{6536115A-7B2D-11D2-BA18-0000F875AC12}` | IID_IDirectMusic |

**Strings, exact bytes** (every one of these is pushed as an immediate address; the byte sequences below are read from the image, including the `0D 0A 00` terminators):

| VA | bytes | literal | pushed at |
| --- | --- | --- | --- |
| `0x00543B10` | `44 69 72 65 63 74 44 72 61 77 43 72 65 61 74 65 45 78 00` | `"DirectDrawCreateEx"` | `0x498888` |
| `0x00543B24` | `43 6f 75 6c 64 6e 27 74 20 63 72 65 61 74 65 20 43 4c 53 49 44 5f 44 69 72 65 63 74 4d 75 73 69 63 0d 0a 00` | `"Couldn't create CLSID_DirectMusic\r\n"` | `0x498859` |
| `0x00543B48` | `43 6f 75 6c 64 6e 27 74 20 43 72 65 61 74 65 53 75 72 66 61 63 65 0d 0a 00` | `"Couldn't CreateSurface\r\n"` | `0x498792` |
| `0x00543B64` | `43 6f 75 6c 64 6e 27 74 20 53 65 74 20 63 6f 6f 70 20 6c 65 76 65 6c 0d 0a 00` | `"Couldn't Set coop level\r\n"` | `0x498753` |
| `0x00543B80` | `43 6f 75 6c 64 6e 27 74 20 51 49 20 44 44 72 61 77 32 0d 0a 00` | `"Couldn't QI DDraw2\r\n"` | `0x498667` |
| `0x00543B98` | `43 6f 75 6c 64 6e 27 74 20 63 72 65 61 74 65 20 44 44 72 61 77 0d 0a 00` | `"Couldn't create DDraw\r\n"` | `0x49861C` |
| `0x00543BB0` | `43 6f 75 6c 64 6e 27 74 20 4c 6f 61 64 4c 69 62 72 61 72 79 20 44 44 72 61 77 0d 0a 00` | `"Couldn't LoadLibrary DDraw\r\n"` | `0x4985E7` |
| `0x00543BD0` | `44 69 72 65 63 74 44 72 61 77 43 72 65 61 74 65 00` | `"DirectDrawCreate"` | `0x4985C5` |
| `0x00543BE4` | `44 44 52 41 57 2e 44 4c 4c 00` | `"DDRAW.DLL"` | `0x49859B` |
| `0x00543BF0` | `43 6f 75 6c 64 6e 27 74 20 47 65 74 50 72 6f 63 41 64 64 72 65 73 73 20 44 49 6e 70 75 74 43 72 65 61 74 65 0d 0a 00` | `"Couldn't GetProcAddress DInputCreate\r\n"` | `0x4986E7` |
| `0x00543C18` | `44 69 72 65 63 74 49 6e 70 75 74 43 72 65 61 74 65 41 00` | `"DirectInputCreateA"` | `0x49854E, 0x4986C1` |
| `0x00543C2C` | `43 6f 75 6c 64 6e 27 74 20 4c 6f 61 64 4c 69 62 72 61 72 79 20 44 49 6e 70 75 74 0d 0a 00` | `"Couldn't LoadLibrary DInput\r\n"` | `0x498538, 0x49869A` |
| `0x00543C4C` | `44 49 4e 50 55 54 2e 44 4c 4c 00` | `"DINPUT.DLL"` | `0x498520, 0x498687` |

Call-site mapping checks out with the snippet: `0x498520 push 0x00543C4C` is `"DINPUT.DLL"` for the NT 4 branch, `0x49859B push 0x00543BE4` is `"DDRAW.DLL"`, `0x498687 push 0x00543C4C` is the second `"DINPUT.DLL"`, and `0x4985E7 push 0x00543BB0` is the `"Couldn't LoadLibrary DDraw\r\n"` message printed when `DirectDrawCreate` could not be resolved.

### 12.11 `0x00498480` — callees in call order, arguments, and transcription status

All 25 call sites are external (Win32/ole32 imports or DirectX/COM calls through vtable/`GetProcAddress` pointers). **There is no engine native among them**, so `0x498480` contributes nothing to the native transcription queue; its `MISSING` entry in `docs/fidelity/mvp-call-flow.md` is about this function itself, and once it is transcribed its dependencies are headers/constants, not natives.

| # | site | instruction | target | arguments, in push order |
| ---: | --- | --- | --- | --- |
| 1 | `0x004984B3` | `call dword [0x533108]` | KERNEL32!GetVersionExA | `&osi` (stdcall, 1) |
| 2 | `0x0049852C` | `call dword [0x533110]` | KERNEL32!LoadLibraryA | `"DINPUT.DLL"` (stdcall, 1) — NT 4 path |
| 3 | `0x0049853D` | `call dword [0x533114]` | KERNEL32!OutputDebugStringA | `"Couldn't LoadLibrary DInput\r\n"` (1) |
| 4 | `0x00498554` | `call dword [0x533118]` | KERNEL32!GetProcAddress | `hDInput`, `"DirectInputCreateA"` (2) |
| 5 | `0x0049855D` | `call dword [0x53311C]` | KERNEL32!FreeLibrary | `hDInput` (1) — pushed at `0x49855A`, before the NULL test |
| 6 | `0x0049856C` | `call dword [0x533114]` | KERNEL32!OutputDebugStringA | `"Couldn't GetProcAddress DInputCreate\r\n"` (1) |
| 7 | `0x004985A0` | `call ebp` | KERNEL32!LoadLibraryA (cached in `ebp` at `0x498595`) | `"DDRAW.DLL"` (1) |
| 8 | `0x004985B4` | `call dword [0x53311C]` | KERNEL32!FreeLibrary | `NULL` (1) — the DDRAW-load-failure path really passes NULL |
| 9 | `0x004985CB` | `call dword [0x533118]` | KERNEL32!GetProcAddress | `hDDraw`, `"DirectDrawCreate"` (2) |
| 10 | `0x004985E1` | `call dword [0x53311C]` | KERNEL32!FreeLibrary | `hDDraw` (1) |
| 11 | `0x004985EC` | `call dword [0x533114]` | KERNEL32!OutputDebugStringA | `"Couldn't LoadLibrary DDraw\r\n"` (1) |
| 12 | `0x00498604` | `call eax` | `DirectDrawCreate` (from `GetProcAddress`) | `NULL` (lpGUID), `&pDDraw`, `NULL` (pUnkOuter) (3) |
| 13 | `0x00498616` | `call dword [0x53311C]` | KERNEL32!FreeLibrary | `hDDraw` (1) |
| 14 | `0x00498621` | `call dword [0x533114]` | KERNEL32!OutputDebugStringA | `"Couldn't create DDraw\r\n"` (1) |
| 15 | `0x00498650` | `call dword [ecx]` | `pDDraw` vtable slot 0 = `QueryInterface` | `this`, `&pDDraw2`, IID `0x534A60` (3) |
| 16 | `0x0049865D` | `call dword [ecx+8]` | `pDDraw` vtable slot 8 = `Release` | `this` (1) |
| 17 | `0x00498661` | `call dword [0x53311C]` | KERNEL32!FreeLibrary | `hDDraw` (1) |
| 18 | `0x0049866C` | `call dword [0x533114]` | KERNEL32!OutputDebugStringA | `"Couldn't QI DDraw2\r\n"` (1) |
| 19 | `0x00498684` | `call dword [edx+8]` | `pDDraw2` slot 8 = `Release` | `this` (1) |
| 20 | `0x00498692` | `call ebp` | KERNEL32!LoadLibraryA | `"DINPUT.DLL"` (1) — second load |
| 21 | `0x0049869F` | `call dword [0x533114]` | KERNEL32!OutputDebugStringA | `"Couldn't LoadLibrary DInput\r\n"` (1) |
| 22 | `0x004986AC` | `call dword [ecx+8]` | `pDDraw` slot 8 = `Release` | `this` (1) |
| 23 | `0x004986B0` | `call dword [0x53311C]` | KERNEL32!FreeLibrary | `hDDraw` (1) |
| 24 | `0x004986C7` | `call dword [0x533118]` | KERNEL32!GetProcAddress | `hDInput`, `"DirectInputCreateA"` (2) |
| 25 | `0x004986D8` | `call edi` | KERNEL32!FreeLibrary (cached in `edi` at `0x4986D2`) | `hDInput` (1) |
| 26 | `0x004986DB` | `call edi` | KERNEL32!FreeLibrary | `hDDraw` (1) |
| 27 | `0x004986E4` | `call dword [edx+8]` | `pDDraw` slot 8 = `Release` | `this` (1) |
| 28 | `0x004986EC` | `call dword [0x533114]` | KERNEL32!OutputDebugStringA | `"Couldn't GetProcAddress DInputCreate\r\n"` (1) |
| 29 | `0x0049870A` | `call ebp` | KERNEL32!FreeLibrary (re-cached in `ebp` at `0x4986FD`) | `hDInput` (1) |
| 30 | `0x0049873F` | `call dword [ecx+0x50]` | `pDDraw` vtable slot 0x50 = `SetCooperativeLevel` | `this`, `NULL` (hWnd), `8` (flags; pushed early at `0x498717`) (3) |
| 31 | `0x0049874D` | `call dword [edx+8]` | `pDDraw` slot 8 = `Release` | `this` (1) |
| 32 | `0x00498751` | `call ebp` | KERNEL32!FreeLibrary | `hDDraw` (1) |
| 33 | `0x0049875E` | `call dword [0x533114]` | KERNEL32!OutputDebugStringA | `"Couldn't Set coop level\r\n"` (1) |
| 34 | `0x0049877E` | `call dword [ecx+0x18]` | `pDDraw` vtable slot 0x18 = `CreateSurface` | `this`, `&ddsd`, `&pSurface`, `NULL` (pUnkOuter) (4) |
| 35 | `0x0049878C` | `call dword [ecx+8]` | `pDDraw` slot 8 = `Release` | `this` (1) |
| 36 | `0x00498790` | `call ebp` | KERNEL32!FreeLibrary | `hDDraw` (1) |
| 37 | `0x0049879D` | `call dword [0x533114]` | KERNEL32!OutputDebugStringA | `"Couldn't CreateSurface\r\n"` (1) |
| 38 | `0x004987BF` | `call dword [edx]` | `pSurface` vtable slot 0 = `QueryInterface` | `this`, `&pSurf3`, IID `0x534A40` (3) |
| 39 | `0x004987CC` | `call dword [edx+8]` | `pDDraw` slot 8 = `Release` | `this` (1) |
| 40 | `0x004987D0` | `call ebp` | KERNEL32!FreeLibrary | `hDDraw` (1) |
| 41 | `0x004987F4` | `call dword [ecx]` | `pSurface` vtable slot 0 = `QueryInterface` | `this`, `&pSurf4`, IID `0x534A30` (3) |
| 42 | `0x00498801` | `call dword [ecx+8]` | `pDDraw` slot 8 = `Release` | `this` (1) |
| 43 | `0x00498805` | `call ebp` | KERNEL32!FreeLibrary | `hDDraw` (1) |
| 44 | `0x0049881F` | `call dword [edx+8]` | `pSurface` slot 8 = `Release` | `this` (1) |
| 45 | `0x00498829` | `call dword [ecx+8]` | `pDDraw` slot 8 = `Release` | `this` (1) |
| 46 | `0x00498836` | `call dword [0x53321C]` | ole32!CoInitialize | `NULL` (1) |
| 47 | `0x0049884F` | `call dword [0x533218]` | ole32!CoCreateInstance | rclsid `&0x5348F0`, `NULL` (pUnkOuter), `1` (CLSCTX_INPROC_SERVER), riid `&0x5348E0`, `&pDM` (5) |
| 48 | `0x0049885E` | `call dword [0x533114]` | KERNEL32!OutputDebugStringA | `"Couldn't create CLSID_DirectMusic\r\n"` (1) |
| 49 | `0x00498865` | `call ebp` | KERNEL32!FreeLibrary | `hDDraw` (1) |
| 50 | `0x0049887F` | `call dword [ecx+8]` | `pDM` slot 8 = `Release` | `this` (1) |
| 51 | `0x00498882` | `call dword [0x533220]` | ole32!CoUninitialize | none (0) |
| 52 | `0x0049888E` | `call dword [0x533118]` | KERNEL32!GetProcAddress | `hDDraw`, `"DirectDrawCreateEx"` (2) |
| 53 | `0x004988A6` | `call eax` | `DirectDrawCreateEx` (from `GetProcAddress`) | `NULL` (lpGUID), `&pDD7`, IID `0x534A50`, `NULL` (pUnkOuter) (4) |
| 54 | `0x004988B9` | `call dword [ecx+8]` | `pDD7` slot 8 = `Release` | `this` (1) |
| 55 | `0x004988BD` | `call ebp` | KERNEL32!FreeLibrary | `hDDraw` (1) — the shared tail |

The table lists all 55 `call` instructions of the body in execution order (checked against the decoded 374-instruction listing: no site missing, none extra). The argument column follows the actual push sequence — the push closest to the `call` is the first argument after `this` — and the stdcall/COM argument pops are what reset `T` at every step used in 12.7.

### 12.12 `0x00498480` — `-O3` loss risks

1. **`push 8` is hoisted ahead of the `DDSURFACEDESC` clear.** `0x498717 push 8` is the `SetCooperativeLevel` flag argument, executed before `0x498719 rep stosd` and long before the call at `0x49873F`. A source-order transcription (build `ddsd`, then call) is behaviourally right; a *statement-order* transcription that puts the push where the source seems to is not the retail order. Record the hoist, do not "fix" it into the clear.
2. **`mov eax,[esp+0x10]` between `test eax,eax` and `jge`.** At `0x498742` the HRESULT is tested; `0x498744` reloads `pDDraw` and does **not** affect flags; `0x498748 jge` therefore still branches on the `SetCooperativeLevel` HRESULT. A transcription that tests the reloaded pointer would flip the branch.
3. **Signed `HRESULT` tests.** Every COM call uses `test eax,eax` + `jge`/`jl` (or `cmp`+`jge`), i.e. `hr >= 0` is success, not `hr == S_OK` and not `hr != 0`. `DirectDrawCreate`/`DirectDrawCreateEx` use `jge`/`jl` too (`0x498608`, `0x4988AA`).
4. **The NT 4 branch reuses the `cmp eax,4` flags.** `0x498501 jae` and `0x498517 jne` both read the same comparison; the `== 4` case falls through and everything else (`> 4`) goes to DDRAW. A rewrite as `if (major >= 4 && major != 4)` is the same branch but the flag reuse is why there is no second `cmp`; keep the structure.
5. **Version-code writes are placed between calls.** `*arg1 = 0x100/0x200/0x300/0x500/0x600/0x601/0x700` occur at specific points relative to the following call (e.g. `*arg1 = 0x200` is written *before* `LoadLibraryA`, `*arg1 = 0` on the coop-level path is written *after* `FreeLibrary`). The observable result on partial-failure paths depends on that placement.
6. **`0x4984A4` zeroes the slot later reused as `ddsd`.** It is a dead store once the struct is built; a transcription that treats it as part of `ddsd`'s initial value would only be accidentally right. Conversely the `rep stosd` at `0x498719` must be 27 dwords = 108 bytes (`0x6C`), no more.
7. **Two out-pointers are never dereferenced before they are written**, and `pDDraw` is *not* pre-zeroed, whereas `pSurface`, `pDDraw2`, `pDM`, `pDD7` are. Do not normalise the initialisation set: the exact set is `pSurface`, `pDM`, `pDDraw2`, `pDD7`, plus the dead slot at `E-0x100`.
8. **`GetProcAddress` results are discarded twice** (the two `DirectInputCreateA` resolutions). Retail only tests them for NULL. Storing them "for later" invents state.
9. **`FreeLibrary(NULL)` is real** on the DDRAW-load-failure path (`0x4985AF` pushes `ebx`=0, `0x4985B4` calls `[FreeLibrary]`). Do not guard it.
10. **Several releases are intentionally absent**: `pSurf3`, `pSurf4` are never released; `pSurface`/`pDDraw` are not released on the QI/creations failure paths; the DM object is not released and `CoUninitialize` is not called when `CoCreateInstance` fails. Adding balanced cleanup changes the retail effect set and the leaks.
11. **`CoInitialize`/`CoUninitialize` bracket only the DirectMusic creation**, and `CoInitialize(NULL)`'s return is ignored (`0x498836`); there is no `SUCCEEDED` test on it.
12. **`0x4984B9 test eax,eax; jne` treats `GetVersionExA`'s BOOL as zero/non-zero** and the failure path writes arg2 *before* arg1 (`0x4984CD`, `0x4984CF`); a swap is observable if the two pointers alias — and the retail order is part of the body.

### 12.13 `0x004A5AD5` — real function entry, not a disassembly artefact

**Verdict: real entry.** 17 bytes, `0x004A5AD5`–`0x004A5AE5`, one basic block, 6 instructions, cdecl, exactly one argument:

| VA | bytes | instruction | derivation |
| --- | --- | --- | --- |
| `0x004A5AD5` | `6a 00` | `push 0` | third argument to `0x004A5AF7` (pushed first) |
| `0x004A5AD7` | `6a 00` | `push 0` | second argument |
| `0x004A5AD9` | `ff 74 24 0c` | `push dword [esp+0xC]` | **the function's own argument read**: with 8 bytes pushed, `[esp+0xC]` = entry `+4` = arg1 → pushed last = first argument |
| `0x004A5ADD` | `e8 15 00 00 00` | `call 0x004A5AF7` | the CRT termination routine |
| `0x004A5AE2` | `83 c4 0c` | `add esp, 0xC` | cdecl cleanup of the three arguments — the body cannot be calling an unknown signature |
| `0x004A5AE5` | `c3` | `ret` | balanced |

Proof it is not mid-instruction: (a) the byte immediately before is `0x004A5AD4 c3 ret`; (b) three compiler-emitted `E8 rel32` call sites target exactly `0x4A5AD5` — `0x0048F71F` (`e8 b1 63 01 00`, `next 0x48F724 + 0x163B1`), `0x00498941` (`e8 8f d1 00 00`, `next 0x498946 + 0xD18F`), `0x004A51C7` (`e8 09 09 00 00`, `next 0x4A51CC + 0x909`); (c) the body is a coherent, stack-balanced 3-push/`add esp,0xC` wrapper; (d) its sibling `0x004A5AE6` is a real entry with the same 17-byte shape and the same `[esp+0xC]` argument read, differing only in the second forwarded constant (`push 1` at `0x004A5AE8` instead of `push 0`), and is called at `0x004A51E6` with `e8 fb 08 00 00`. Function entries in this MSVC image are not required to be 16-byte aligned; alignment is not evidence.

**What the call sites show (semantics, with evidence):**

- `0x004A51C7`, in the CRT startup routine: `0x4A51BE call main` → `0x4A51C3 mov [var_60h],eax` → `0x4A51C6 push eax` → `0x4A51C7 call 0x4A5AD5`. That is `exit(main(...))`: the argument is the program's return code.
- `0x0048F71F`, in WinMain: after `MessageBoxA(NULL, "This application requires DirectX version %d.%d or greater to run.", "Error", 0x10)` the code does `0x48F71D push 1` / `0x48F71F call 0x4A5AD5` — terminating with code 1 from an error path.
- `0x00498941`, in a small assertion-failure routine: `0x498930 call 0x48F1B0` → `0x498935 test eax,eax` → `0x498937 je 0x49893F` → `0x498939 call [KERNEL32!DebugBreak]` → `0x49893F push 1` → `0x498941 call 0x4A5AD5` — the CRT-style "assert failed: break then terminate with 1" pattern.

**What it forwards to.** `0x004A5AF7` (body `0x004A5AF7`–`0x004A5B8F`, 153 bytes) is the termination implementation: `0x4A5AF7 push edi`, `0x4A5AF8 push 1` / `0x4A5AFA pop edi` (`edi = 1`), `0x4A5AFB cmp dword [0x9360AC],1`; when the flag equals 1 it calls `TerminateProcess(GetCurrentProcess(), arg1)` (`0x4A5B03 push [esp+8]`, `0x4A5B07 call [GetCurrentProcess]`, `0x4A5B0E call [TerminateProcess]`). Otherwise it sets `[0x9360A8]=1` and `[0x9360A4]=arg3` (byte), walks the exit-function table at `0x00C32550`/`0x00C3254C`, calls `0x004A5B90(0x00536024, 0x00536028)`, and then `0x4A5B79 test ebx,ebx` / `0x4A5B7C jne 0x4A5B8E`; with `arg3 == 0` (both wrappers pass 0) it continues to `0x4A5B82 mov dword [0x9360AC],1` + `0x4A5B88 call [KERNEL32!ExitProcess]`, and `ebx != 0` would instead `pop edi; ret` at `0x4A5B8E`. `0x4A5AD5` passes `(code, 0, 0)`; its sibling `0x4A5AE6` has the same 17-byte shape and passes `(code, 1, 0)`. **No return path is observable from any of the three call sites**, which is consistent with the `ExitProcess`/`TerminateProcess` tail. The exact CRT symbol name (`exit` vs a wrapper around it) is not proven; the behaviour — terminate the process with the given code after running the exit table — is.

**Manifest:** `0x004A5AD5` and `0x004A5AF7` are **NOT IN MANIFEST**. The former is `MISSING` in `mvp-call-flow.md` row 34; it is a legitimate transcription target (a small CRT wrapper plus its 153-byte implementation), not an artefact to skip and not a camera routine.

### 12.14 What is NOT proven anywhere in this section

1. **No retail names.** Every name used is a working description or an external-catalogue label (C7). No symbol table entry was found for `0x4010B0`, `0x401A60`, `0x4015F0`, `0x402C40`, `0x498480`, `0x4A5AD5`, `0x4A5AF7`.
2. **`0x4010B0`:** the meaning of `0x401A60`'s non-zero result; `0x4015F0`'s actual effect beyond the globals in 12.6; the cascade's semantics and return type; any runtime behaviour.
3. **`0x498480`:** its return type (EAX is inconsistent per path and the caller discards it); the *meaning* of the version codes `0x100`/`0x200`/`0x300`/`0x500`/`0x600`/`0x601`/`0x700` (the 0x700 floor is WinMain's comparison; "major.minor" is an interpretation); the semantics of `out_platform`'s `1`/`2` values (only "2 when `dwPlatformId == 2`" is mechanical fact); whether the two out-parameters may alias; the ABI of the two interface-pointer types beyond "4-byte pointer".
4. **Type identifications** are structural: `OSVERSIONINFOA` from the `0x94` size, the `dwOSVersionInfoSize` write and the `+4`/`+0x10` field reads; `DDSURFACEDESC` from the `0x6C` size, `dwSize`/`dwFlags` writes, the `+0x68` caps write and its use as `CreateSurface`'s descriptor. The *GUID names* are SDK-knowledge matches to the exact image bytes (only `DDraw2` and `CLSID_DirectMusic` are corroborated by retail strings). The *vtable offsets* 0, 8, 0x18, 0x50 are corroborated by the retail messages (`Couldn't QI DDraw2`, `Couldn't CreateSurface`, `Couldn't Set coop level`) plus the pointer-returning `GetProcAddress` results, but no DirectX header was consulted in this pass.
5. **`0x498480` runtime behaviour:** no path was executed; the per-path effects above are static reconstructions of the retail instructions.
6. **Call-site reachability:** the WndProc dispatch path (C1) and the CRT startup path (`exit(main(...))`) are normal Windows/CRT flow, but no runtime trace was captured to confirm they are hit in this build's scenario.
7. **The cascade region beyond `0x402C40`:** only `0x402C40` and the final `0x532490` block were re-verified in this pass; the intermediate block boundaries are carried from `0x004010B0-static-analysis.json` and are labelled as prior-record facts.
8. **No verification status is claimed:** this section is `ANALYZED` for both natives. `TRANSCRIBED`/`STRUCTURALLY_VERIFIED` require the selected-decompiler + angr + build/runtime steps of AGENTS §10.2–10.9, which were not performed here.

### 12.15 Recommended next actions from this evidence

- Transcribe `0x00401A60` first: six bytes, zero arguments, `return *(uint32_t*)0x00567C58;` — it unblocks `0x4010B0`'s first branch.
- Then `0x00402C40` + its cascade: the first call, `0x4C1780`, is a 1-byte `ret` function; the chain is straight-line (no branches except the tail jumps), so it is mechanical but large; do not model it as one helper.
- `0x004015F0` is the hub (391 bytes, recursive, 120 call sites); transcribe it from its own sheet before `0x4010B0` can be considered body-complete.
- `0x00498480` can be transcribed independently of the engine-native queue; it needs the exact GUID bytes and the 13 strings from 12.10 (byte-identical, including `\r\n`), the seven local pointers, the `0x6C` descriptor and the `0x94` version-info frame, and it must keep the leaks / skipped `CoUninitialize` / dead-slot zeroing described in 12.12.
- `0x004A5AD5`/`0x004A5AF7` complete the WinMain error path and the CRT `exit(main())` tail; transcribe the wrapper, and treat `0x4A5AF7`'s exit-table walk as its own native.

## §13. APPENDED — identifier names derived from retail evidence

Requested by the Lead: (Task A) evidence-derived names for the 33 identifiers left address-suffixed by the rename pass (`994850a4`), (Task B) semantic names for the six-byte phase-record globals and the class-channel globals **only where the retail instructions prove the meaning**. This section is append-only; no earlier section, appendix, `engine/` file, manifest, listing or git state was changed.

**Status vocabulary used below**

| tag | meaning |
| --- | --- |
| **PROVEN** | the retail instructions establish both the data the function touches and its role (registration slot / class word / phase id / size); the name follows directly. |
| **BEHAVIOUR** | the instructions establish exactly what the function does mechanically; the *domain* role is not established, and the name says only what is proven. |
| **UNRESOLVED** | the instructions establish no role beyond "writes constant N". The suffixed placeholder is retained; the missing evidence is named. |

### 13.0 Rules applied to the names

- Derived only from retail instructions in `igi.exe` (SHA-256 `dea34892…15a01`). No name asserts a game meaning that the instructions do not show; the mechanism the name describes is cited in the row.
- No `Retail*`, `Host*`, `Port*`, `Wrapper*`, `Adapter*` fragments; retail-style project form `<Domain>_<Subject><Action>` (e.g. `Script_ParameterEventSlotGetEB`, `AppMain_Set5C8C0C`).
- A name is unique across the 33 and across the identifiers that already exist in `engine/`. Where the only uniquely identifying fact is a slot or global, the slot byte (as in the rename pass's `Script_ParameterEventSlotGetEB`) is part of the name.
- Where a function exists only to be registered as a callback, the slot it fills is the strongest evidence; where its own body moves/copies/sizes a value, the body is used instead.

### 13.1 Method and tool limits

- Body evidence: `r2 -q -e scr.color=0 -c 'pd N @ <VA>' -c q igi.exe` (radare2 6.2.0, `/opt/homebrew/bin/r2`, run from `/Users/haseeb-mir/Downloads/IGI`).
- Reference evidence: radare2 `axt`/`axtj` returns **0** for code and data references to every one of the 33 VAs in this image (no `aa` analysis; data references disabled). All reference evidence below therefore comes from PE-aware raw scans of the image:
  - every `push imm32` (`68 <le32>`) occurrence of a VA → registration sites;
  - every direct `call rel32` whose computed target is the VA → call sites;
  - every 4-byte literal in a global range → read/write sites;
  - complete `e8` call-target enumeration over `.text` to validate the call sites.
- `.text` VA `0x401000`, raw `0x1000`; `.data` VA `0x536000`, raw `0x136000`; RVA→file offset `raw + (va - base - section_va)`.
- The scans are reproducible with the commands in §13.7.

### 13.2 Task A — the 33 identifiers

Columns: **current** = the address-suffixed name from the rename pass; **VA** = retail entry; **evidence** = the instructions the proposal rests on; **proposed** = the name, with its status tag, or **UNRESOLVED** plus what is missing.

| # | current | VA | evidence (retail instructions) | proposed / status |
| --- | --- | --- | --- | --- |
| 1 | `Graph_Get_4F9200` | `0x4F9200` | body `a1 10 6d a7 00; c3` = `mov eax,[0xA76D10]; ret`. `0xA76D10` is written once, in `Graph_Init` `0x4FAA60`: `0x4FAB94 push 5`, `0x4FAB96 call 0x4F1950`, `0x4FAB9E a3 10 6d a7 00 mov [0xA76D10],eax`. `0x4F1950` (0x4F1950–0x4F1978) is an array constructor: `shl eax,4; add eax,0xC; call 0x4B0C60(n*16+0xC, 4); [eax+4]=n; [eax]=0; [eax+8]=eax+0xC`, i.e. 16-byte elements, count at +4, data at +8. `0x4F1980` (0x4F1980–0x4F19AF) grows such an array by `n` elements (realloc + `rep movsd`). Only caller of the getter: `0x509123 call 0x4F9200; push eax; call 0x4F1980` (grow the array). | `Graph_GetArray` — **BEHAVIOUR**. What the elements are is not established by any instruction (the 16-byte stride and the `0x4F1A80` fills in `Graph_Init` do not name them). |
| 2 | `Graph_Get_4F9210` | `0x4F9210` | body `66 a1 64 e3 54 00; c3` = `mov ax,[0x54E364]; ret`. `0x54E364` is written once, in `Graph_Init 0x4FAA60`: `0x4FAA68 xor edi,edi`, `0x4FAA6A push edi`, `0x4FAA6B push edi`, `0x4FAA6C call 0x5034D0`, `0x4FAA71 push eax`, `0x4FAA72 push 0x80`, `0x4FAA77 push "Graph"`, `0x4FAA7C call 0x401900`, `0x4FAA82 66 a3 64 e3 54 00 mov [0x54E364],ax` — so the word is the class word `0x401900(name, size, parent)` returned for `"Graph"` (size `0x80`, parent = the value `0x5034D0(0,0)` returned). Two direct use proofs of the role: (a) `0x508FA8 call 0x4F9210` (`0x508FA6 push 0; 0x508FA8 call`) and the returned word is passed as the **parent** argument of the `"AIGraph"` registration — `0x508FAA push eax`, `0x508FB0 push 0xC0`, `0x508FB5 push "AIGraph"`, `0x508FBA call 0x401900` — and `Graph_Init` uses the same global as the class word of its own `0x401400` phase registrations (`0x4FAAF5 mov ax,[0x54E364]`, `0x4FAB01 push eax`, `0x4FAB02 call 0x401400`); (b) it is a dispatch-table selector: `0x509632 call 0x4F9210; and eax,0xffff; 0x50963D mov eax,[eax*4+0xA976E0]; call eax`. | `Graph_GetClassWord` — **PROVEN**. |
| 3 | `Graph_Get_4F9220` | `0x4F9220` | body `a0 6b e3 54 00; c3` = `mov al,[0x54E36B]; ret`. Slot written in `Graph_Init`: `0x4FAAA9 call 0x4017C0`, `0x4FAAAF a2 6b e3 54 00 mov [0x54E36B],al`. Used as the **phase argument** of `0x401400(classWord, phase, handler)` in the AIGraph block: `0x509067 push 0x509450`, `0x50906C call 0x4F9220`, `0x509071 push eax`, `0x509072 mov ax,[0x54E83C]`, `0x509079 call 0x401400`. Dispatch use: `0x4FA376 call 0x4F9220; and eax,0xff; mov cx,[edi+0x1C]; lea eax,[eax+eax*2]; shl eax,7 (*0x180); add eax,ecx; mov eax,[eax*4+0xA96AE0]` (same class+phase formula as §11's table). | `Graph_PhaseIdGetE36B` — **PROVEN** role (allocated phase id of the Graph type), unique only via the slot. |
| 4 | `Graph_Get_4F9230` | `0x4F9230` | body `a0 6c e3 54 00; c3`; slot written `0x4FAAB4 call 0x4017C0`, `0x4FAABA mov [0x54E36C],al`; registration use `0x509017 push 0x509520; 0x50901C call 0x4F9230; 0x509021 push eax; 0x509022 mov ax,[0x54E83C]; 0x509029 call 0x401400`. | `Graph_PhaseIdGetE36C` — **PROVEN**. |
| 5 | `Graph_Get_4F9240` | `0x4F9240` | body `a0 6d e3 54 00; c3`; slot written `0x4FAAE0 call 0x4017C0`, `0x4FAAE6 mov [0x54E36D],al`; registration use `0x509081 push 0x509480; 0x509086 call 0x4F9240; 0x50908B mov cx,[0x54E83C]; 0x509092 push eax; 0x509094 call 0x401400`; dispatch use `0x4FA2E1 call 0x4F9240; and eax,0xff; mov dx,[edi+0x1C]; lea ecx,[eax+eax*2]; shl ecx,7`. | `Graph_PhaseIdGetE36D` — **PROVEN**. |
| 6 | `Graph_Get_4F9250` | `0x4F9250` | body `a0 6e e3 54 00; c3`; slot written `0x4FAAEB call 0x4017C0`, `0x4FAAF0 mov [0x54E36E],al`; registration use `0x509031 push 0x4D3710; 0x509036 call 0x4F9250; 0x50903B mov cx,[0x54E83C]; 0x509042 push eax; 0x509044 call 0x401400`. | `Graph_PhaseIdGetE36E` — **PROVEN**. |
| 7 | `Graph_Get_4F9260` | `0x4F9260` | body `a0 6f e3 54 00; c3`; slot written `0x4FAABF call 0x4017C0`, `0x4FAAC5 mov [0x54E36F],al`; registration use `0x50904C push 0x4D3710; 0x509051 call 0x4F9260; 0x509056 mov dx,[0x54E83C]; 0x50905D push eax; 0x50905F call 0x401400`. The slot is *read* inline (not written) once more, in the same dispatch pattern: `0x4F95DC a0 6f e3 54 00 mov al,[0x54E36F]; 0x4F95E4 lea edx,[eax+eax*2]; 0x4F95E9 mov ax,[ebp+0x1C]; 0x4F95ED shl edx,7`. | `Graph_PhaseIdGetE36F` — **PROVEN** role. |
| 8 | `Graph_Get_4F9280` | `0x4F9280` | body `a0 69 e3 54 00; c3`; slot written `0x4FAA93 call 0x4017C0`, `0x4FAA99 mov [0x54E369],al`; registration use `0x508FFC push 0x5093B0; 0x509001 call 0x4F9280; 0x509006 mov dx,[0x54E83C]; 0x50900D push eax; 0x50900E push edx; 0x50900F call 0x401400`; dispatch use `0x4F96A2 call 0x4F9280; and eax,0xff; lea edx,[eax+eax*2]; shl edx,7; add edx,[ebx+0x1C]; mov eax,[edx*4+0xA96AE0]` (also `0x4FA1D4`). | `Graph_PhaseIdGetE369` — **PROVEN**. |
| 9 | `Script_Handler_4EF830` | `0x4EF830` | body `8b 44 24 08` (arg1) `8b 54 24 04` (arg0) `8b 08` (ecx=*arg1) `51 52` `e8 3f 1f fd ff call 0x4C1780` `83 c4 08` `c3`. `0x4C1780` is a **single byte `c3`** (ret-only; the checkout documents it as the task-list registrar that registers nothing). Registration: `0x4EF481 push 0x4EF830; 0x4EF486 call 0x4F1BB0` (slot E8); `0x4EF48C mov eax,[0xA75894]` (QTaskNote type handle); `0x4EF492 call 0x4F1BC0`. QTaskNote.kindE8. | `QTaskNote_ValueSet` — **PROVEN role / BEHAVIOUR body**: kindE8 is the value-write slot (see §13.3.3), but the body delegates to the ret-only `0x4C1780`, so it stores nothing. A transcription must keep the call, not replace it with a copy. |
| 10 | `Script_Handler_4EF850` | `0x4EF850` | body `8b 44 24 08; c7 00 41 00 00 00; c3` = stores `0x41` through arg1. Registration `0x4EF49A push 0x4EF850; 0x4EF49F call 0x4F1B90` (slot E9); `0x4EF4AA push [0xA75894]`; `0x4EF4AC call 0x4F1BC0`. QTaskNote.kindE9. The same type's kindE7 handler `0x4EF7F0` copies a NUL-terminated string (`0x4EF7F7 call 0x4F1180`, then `strlen`-style `repne scasb` and `rep movsb`), so `0x41` (65) is the note string's capacity, not an exact length. | `QTaskNote_ValueSizeGet` — **PROVEN** (E9 = value size; §13.3.2). |
| 11 | `Script_Handler_4EF8B0` | `0x4EF8B0` | body `8b 44 24 08; c7 00 0b 00 00 00; c3` = stores `0xB`. Registration `0x4EF6B5 push 0x4EF8B0; 0x4EF6BA call 0x4F1B70` (slot E6); `0x4EF6BF mov edx,[0xA70C80]` (GraphEnvelope); `0x4EF6C7 call 0x4F1BC0`. GraphEnvelope.kindE6. | **UNRESOLVED.** KindE6's meaning is not decodable from the image (§13.3.4): the E6 slot is an attribute index whose consumer is the data-driven class channel, and the E6 handler bodies store three different constants (1, 3, `0xB`), so no role follows. Missing: the consumer that decodes an E6 value (none in `.text`), or the QVM/class data that defines the slot. |
| 12 | `Script_Handler_4EF990` | `0x4EF990` | body `8b 44 24 08` (arg1) `56` `8b 74 24 08` (arg0) `57` `8b 38` (edi=*arg1) `b9 0b 00 00 00` `f3 a5 rep movsd` (11 dwords = 44 bytes, arg0→*(arg1)) `5f 5e c3`. Registration `0x4EF668 push 0x4EF990; 0x4EF66D call 0x4F1BA0` (slot E7); `0x4EF678 push [0xA70C80]`; `0x4EF67A call 0x4F1BC0`. GraphEnvelope.kindE7. | `GraphEnvelope_ValueGet` — **PROVEN** (kindE7 = copy the value out; §13.3.3). |
| 13 | `Script_Handler_4EF9B0` | `0x4EF9B0` | body `8b 44 24 08` (arg1) `56 57` `8b 7c 24 0c` (arg0) `8b 30` (esi=*arg1) `b9 0b 00 00 00` `f3 a5` (11 dwords, *(arg1)→arg0) `5f 5e c3`. Registration `0x4EF682 push 0x4EF9B0; 0x4EF687 call 0x4F1BB0` (slot E8); `0x4EF68D push [0xA70C80]`; `0x4EF693 call 0x4F1BC0`. GraphEnvelope.kindE8. | `GraphEnvelope_ValueSet` — **PROVEN** (§13.3.3). |
| 14 | `Script_Handler_4EF9D0` | `0x4EF9D0` | body `8b 44 24 08; c7 00 2c 00 00 00; c3` = stores `0x2C` (44). Registration `0x4EF69B push 0x4EF9D0; 0x4EF6A0 call 0x4F1B90` (slot E9); `0x4EF6AC push [0xA70C80]`; `0x4EF6AD call 0x4F1BC0`. GraphEnvelope.kindE9. The same type's kindE7 handler `0x4EF990` copies exactly 44 bytes. | `GraphEnvelope_ValueSizeGet` — **PROVEN**. |
| 15 | `Script_Handler_4EFA10` | `0x4EFA10` | body `8b 44 24 08; c7 00 01 00 00 00; c3` = stores `1`. Registered **11×** as kindE6 in the initialiser — `0x4EECE3` (Real32, type `0xA71870`), `0x4EEEE4` (Real64, `0xA71874`), `0x4EF03A` (Int32, `0xA70C7C`), `0x4EF0E5` (Int16, `0xA70C78`), `0x4EF190` (VarString, `0xA70C9C`), `0x4EF23B` (String16, `0xA71884`), `0x4EF2E6` (String32, `0xA71888`), `0x4EF391` (String256, `0xA7186C`), `0x4EF43C` (LevelExpression, `0xA7188C`), `0x4EF4B4` (QTaskNote, `0xA75894`), `0x4EF55F` (bool8, `0xA70C90`) — each `call 0x4F1B70` (slot E6) followed by `0x4F1BC0`; plus three outside registrations (`0x4772AB`, `0x477403`, `0x477526`). It also serves **bool8.kindE9**: `0x4EF545 push 0x4EFA10; 0x4EF54A call 0x4F1B90` (slot E9), type `0xA70C90`. | **UNRESOLVED.** KindE6 is undecoded (§13.3.4); its single kindE9 use (bool8 → 1) agrees with the byte size, but that does not name the function's dominant E6 role. |
| 16 | `Script_Handler_4EFA90` | `0x4EFA90` | body `8b 44 24 08` (arg1) `8b 54 24 04` (arg0) `8b 08` (ecx=*arg1) `8b 02` (eax=*arg0) `89 01` (store through *arg1) `c3` — copies the 4-byte value pointed at by arg0 through `*(arg1)`. Registered Int32.kindE7 (`0x4EEFED`) and Real32.kindE7 (`0x4EEC96`): pushes `0x4EFA90` then `call 0x4F1BA0` (slot E7). | `Script_ValueGet4` — **PROVEN** (kindE7 copy-out for every 4-byte type; §13.3.3). |
| 17 | `Script_SetConstant_427D70` | `0x427D70` | body `8b 44 24 08; c7 00 04 00 00 00; c3` = stores `4`. Registered Real32.kindE9 (`0x4EECCA push 0x427D70; 0x4EECCF call 0x4F1B90`) and Int32.kindE9 (`0x4EF021 call 0x4F1B90`). Real32/Int32 kindE7 `0x4EFA90` copies exactly 4 bytes. | `Script_ValueSize4Get` — **PROVEN**. |
| 18 | `Script_SetConstant_427F80` | `0x427F80` | body `8b 44 24 08; c7 00 08 00 00 00; c3` = stores `8`. Registered Real64.kindE9 (`0x4EEECB push 0x427F80; 0x4EEED0 call 0x4F1B90`). Real64 kindE7 `0x4EFF90` copies 2 dwords = 8 bytes. | `Script_ValueSize8Get` — **PROVEN**. |
| 19 | `Script_SetConstant_4EFB00` | `0x4EFB00` | body `8b 44 24 08; c7 00 03 00 00 00; c3` = stores `3`. Registered kindE6 for Quat32 (`0x4EEC38`, type `0xA71878`), Real32x3 (`0x4EED8E`, `0xA70C68`), Real32x9 (`0x4EEE39`, `0xA70C6C`), Real64x3 (`0x4EEF8F`, `0xA75890`), plus `0x44656E`. | **UNRESOLVED.** KindE6 undecoded; `3` is not those types' byte size (Quat32 = 20, Real32x9 = 40), so no size reading is available either. |
| 20 | `Script_SetConstant_4EFCD0` | `0x4EFCD0` | body `8b 44 24 08; c7 00 28 00 00 00; c3` = stores `0x28` (40). Registered Real32x9.kindE9 (`0x4EEE20 push 0x4EFCD0; 0x4EEE25 call 0x4F1B90`). Real32x9 kindE7 `0x4EFC90` copies `0xA` dwords = 40 bytes (`b9 0a 00 00 00; f3 a5`). | `Real32x9_ValueSizeGet` — **PROVEN**. |
| 21 | `Script_SetConstant_4EFF20` | `0x4EFF20` | body `8b 44 24 08; c7 00 14 00 00 00; c3` = stores `0x14` (20). Registered Quat32.kindE9 (`0x4EEC1F`). Quat32 kindE7 `0x4EFEE0` copies `5` dwords = 20 bytes. | `Quat32_ValueSizeGet` — **PROVEN**. |
| 22 | `Script_SetConstant_4F00A0` | `0x4F00A0` | body `8b 44 24 08; c7 00 18 00 00 00; c3` = stores `0x18` (24). Registered Real64x3.kindE9 (`0x4EEF76 push 0x4F00A0; 0x4EEF7B call 0x4F1B90`; type `0xA75890` in that block — the Real64x3 handle per the §4 field map). Real64x3 kindE7 `0x4F0060` copies `6` dwords = 24 bytes. | `Real64x3_ValueSizeGet` — **PROVEN**. |
| 23 | `Script_SetConstant_4F01B0` | `0x4F01B0` | body `8b 44 24 08; c7 00 02 00 00 00; c3` = stores `2`. Registered Int16.kindE9 (`0x4EF0CC`). Int16 kindE7 `0x4F0170` copies one word = 2 bytes (`66 8b 02; 66 89 01`). | `Int16_ValueSizeGet` — **PROVEN**. |
| 24 | `Script_SetConstant_4F64E0` | `0x4F64E0` | body `8b 44 24 08; c7 00 00 00 00 00; c3` = stores `0`. Registered Graph.kindE9 (`0x4EF5F0 push 0x4F64E0; 0x4EF5F5 call 0x4F1B90`; `0x4EF5FA mov ecx,[0xA70CA0]` = Graph handle). Graph kindE7 is the ret-only no-op `0x4D3710`, i.e. a 0-byte value. | `Graph_ValueSizeGet` — **PROVEN**. |
| 25 | `Script_SetConstant_51BAA0` | `0x51BAA0` | body `8b 44 24 08; c7 00 0c 00 00 00; c3` = stores `0xC` (12). Registered Real32x3.kindE9 (`0x4EED75 push 0x51BAA0; 0x4EED7A call 0x4F1B90`). Real32x3 kindE7 `0x4EFB60` copies 3 dwords = 12 bytes (three explicit dword moves). | `Real32x3_ValueSizeGet` — **PROVEN**. |
| 26 | `Camera_Get_4F0F80` | `0x4F0F80` | body `a1 74 18 a7 00; c3` = `mov eax,[0xA71874]; ret`. `0xA71874` is the Real64 type handle: §4 R5 field map, written by the initialiser at `0x4EEE5F` immediately after `0x4F19C0("Real64", -1)`. | `Script_TypeHandleGetReal64` — **PROVEN**. |
| 27 | `Camera_Get_4F1000` | `0x4F1000` | body `a1 90 0c a7 00; c3` = `mov eax,[0xA70C90]; ret`. `0xA70C90` is the bool8 type handle: §4 R3 field `+0x28`, writer `0x4EF4DA`, reader `0x4F1000`. | `Script_TypeHandleGetBool8` — **PROVEN**. |
| 28 | `Script_Get_4F1A60` | `0x4F1A60` | body `a0 e4 e0 54 00; c3` = `mov al,[0x54E0E4]; ret`. Slot written in `0x4F1840`: `0x4F185E push eax(0)`, `0x4F1864 call 0x4017C0`, `0x4F1869 push 0`, `0x4F186B a2 e4 e0 54 00 mov [0x54E0E4],al`. Use: `0x4064C1 push 0x406B60; 0x4064C6 call 0x4F1A60; 0x4064CB mov dx,[0x5383D8]; 0x4064D2 push eax; 0x4064D3 push edx; 0x4064D4 call 0x401400` — i.e. the getter's value is the phase argument. `Graph_Init` uses the same getter as a **Graph** phase id: `0x4FAB63 push 0x4FAE40; 0x4FAB68 call 0x4F1A60; 0x4FAB6D mov dx,[0x54E364]; 0x4FAB74 push eax; 0x4FAB76 call 0x401400`. There are 140 call sites; all pass the result as a phase/state argument. | `Script_PhaseIdGetE4` — **PROVEN** role (a phase id allocated by `QTask_AllocateEvent(0)`); if family consistency with `Script_ParameterEventSlotGetE6..EB` is preferred, `Script_ParameterEventSlotGetE4` also fits the shape. Lead's choice. |
| 29 | `Script_Get_4F1A70` | `0x4F1A70` | body `a0 e5 e0 54 00; c3`; slot written `0x4F1870 call 0x4017C0`, `0x4F1878 mov [0x54E0E5],al`; use as phase argument `0x4064E1 call 0x4F1A70; 0x4064E6 push eax; 0x4064E7 mov ax,[0x5383D8]; 0x4064ED push eax; 0x4064EE call 0x401400`, and as a **Graph** phase id `0x4FAB48 push 0x4F9180; 0x4FAB4D call 0x4F1A70; 0x4FAB52 mov cx,[0x54E364]; 0x4FAB59 push eax; 0x4FAB5B call 0x401400`. 223 call sites, all as a phase/state argument. | `Script_PhaseIdGetE5` — **PROVEN** (or `Script_ParameterEventSlotGetE5`). |
| 30 | `Script_StateByteGet_4CEA10` | `0x4CEA10` | body `a0 76 81 54 00; c3` = `mov al,[0x548176]; ret`. Slot written by `QTask_InitPhaseHandlers` `0x4C5900`: `0x4C5974 push 0x181; 0x4C5979 call 0x4017C0`, `0x4C5983 a2 76 81 54 00 mov [0x548176],al`; cleared in the matching teardown `0x4C5AFB mov al,[0x548176]; push eax; call 0x4018C0`. Getter use as phase argument: `0x427ED4 push 0; 0x427ED6 call 0x4CEA10; 0x427EDB push eax; 0x427EDC mov ax,[0x53BB6C]; 0x427EE2 push eax; 0x427EE3 call 0x401400`. | `QTask_PhaseIdGet548176` — **PROVEN role** (phase id allocated with value `0x181`); the slot address is the only unique identifier, so it is in the name. Alternative if a shorter form is preferred: `QTask_FirstPhaseIdGet` (it is the first of the two allocations). |
| 31 | `Script_StateByteGet_4CEA20` | `0x4CEA20` | body `a0 77 81 54 00; c3`; slot written by the next allocation in `0x4C5900` (`0x4C597E push 0x181; 0x4C5988 call 0x4017C0`, `0x4C5992 mov [0x548177],al`) and linked as the phase of class word `0x54875C` with handler `0x4C5A10`: `0x4C598D push 0x4C5A10`, `0x4C5998 mov ax,[0x54875C]`, `0x4C599F call 0x401400`; cleared at `0x4C5B06`. Getter use as phase argument `0x427EBE call 0x4CEA20; 0x427EC3 mov dx,[0x53BB6C]; 0x427ECA push eax; 0x427ECB push edx; 0x427ECC call 0x401400`. | `QTask_PhaseIdGet548177` — **PROVEN role** (this one is additionally proven to be a phase argument of `0x401400` by its own allocation site). Alternative `QTask_SecondPhaseIdGet`. |
| 32 | `Error_CallbackSet_4AF870` | `0x4AF870` | body `8b 44 24 04; a3 70 62 93 00; c3` = stores arg into `0x936270`. Whole-image reference scan for `0x00936270`: exactly **one** occurrence — this store at `0x4AF875`. Only caller: `0x4AF8D0 push 0; call 0x4AF870` inside the callback-reset routine `0x4AF8CC`-`0x4AF8E3` (which also calls `0x4AF880`/`0x4AF890`). | **UNRESOLVED.** The global has no reader anywhere in the image, so no instruction establishes what the callback is called for; the setter's body only shows "store a pointer". Missing: any read/invocation of `0x936270` (none exists in this image), or a call site that passes a known function. |
| 33 | `Error_CallbackSet_4AF880` | `0x4AF880` | body `8b 44 24 04; a3 74 62 93 00; c3` = stores arg into `0x936274`. References to `0x00936274`: `0x4AF811 mov eax,[0x936274]` (test) and `0x4AF855 call dword [0x936274]` inside `Script_WarningLog 0x4AF810`, which first formats `"Warning:\n\n"` + the message into `0xA84640` (`0x4AF81A`–`0x4AF849`) and then calls the callback with that buffer; the callback is installed with the display routine (`0x498919 push 0x4988D0; call 0x4AF880`, `0x491835 push 0x491890; call 0x4AF880`) and cleared with 0 by `0x4AF8D7`. | `Error_WarningCallbackSet` — **PROVEN**. |

**Result:** 29 names proposed — 27 **PROVEN**, plus 2 with a proven registration role but only mechanical body evidence (`Graph_GetArray` for `0x4F9200` and `QTaskNote_ValueSet` for `0x4EF830`) — and 4 left **UNRESOLVED**: `0x4EF8B0`, `0x4EFB00`, `0x4EFA10` (all three kindE6 handlers) and `0x4AF870` (write-only callback global).

### 13.3 The evidence the names rest on (new facts established by this pass)

#### 13.3.1 The attribute-slot ids: E4/E5 are allocated, E6..EB are allocated, and the six kinds are attribute indices

`0x4F1840` (documented in §8) zeroes the parameter table at `0xA758E0` and then: `0x4017C0(0)` → `[0x54E0E4]` (`0x4F185E`–`0x4F186B`), `0x4017C0(0)` → `[0x54E0E5]` (`0x4F1870`–`0x4F1878`), then six `0x4F1B20` calls → `[0x54E0EB]`, `[0x54E0EA]`, `[0x54E0E7]`, … (`0x4F187D`–`0x4F18xx`). The six getters map to slots as follows (one `mov al, byte [slot]; ret` each): `0x4F1B70`→`0x54E0E6` (E6), `0x4F1BA0`→`0x54E0E7` (E7), `0x4F1BB0`→`0x54E0E8` (E8), `0x4F1B90`→`0x54E0E9` (E9), `0x4F1B60`→`0x54E0EA` (EA), `0x4F1B80`→`0x54E0EB` (EB). The registration sites read `0x4F1BC0(type, kind, handler)` with those getters' results as the kind argument, so E6..EB are **attribute indices into the type's per-type slot table**, not data. Nothing in the image decodes an E6..EB index — the consumer is the class/QVM channel (§9.0 Channel 2, invocation not located), which is why no E6 name can be derived.

#### 13.3.2 PROOF that kindE9 is the value byte size — this supersedes the §9.2 "unproven" caution

§9.2 stated the E9 immediates "look like byte sizes … not proven". The kindE7 handler of each type is the value copy-out whose copy width is a direct measurement of the value. For every type with a constant E9 handler the copy width equals the E9 constant exactly:

| type | kindE9 handler | E9 value | kindE7 handler | kindE7 copy width | match |
| --- | --- | --- | --- | --- | --- |
| bool8 | `0x4EFA10` (shared) | 1 | `0x4F0560` | `8a 02; 88 01` — 1 byte | = 1 |
| Int16 | `0x4F01B0` | 2 | `0x4F0170` | `66 8b 02; 66 89 01` — 1 word | = 2 |
| Int32 | `0x427D70` (shared) | 4 | `0x4EFA90` | `8b 02; 89 01` — 1 dword | = 4 |
| Real32 | `0x427D70` (shared) | 4 | `0x4EFA90` | 1 dword | = 4 |
| Real32x3 | `0x51BAA0` | `0xC` | `0x4EFB60` | 3 explicit dword moves | = 12 |
| Real64 | `0x427F80` | 8 | `0x4EFF90` | 2 dwords | = 8 |
| Real64x3 | `0x4F00A0` | `0x18` | `0x4F0060` | `b9 06; f3 a5` — 6 dwords | = 24 |
| Quat32 | `0x4EFF20` | `0x14` | `0x4EFEE0` | `b9 05; f3 a5` — 5 dwords | = 20 |
| Real32x9 | `0x4EFCD0` | `0x28` | `0x4EFC90` | `b9 0a; f3 a5` — 10 dwords | = 40 |
| GraphEnvelope | `0x4EF9D0` | `0x2C` | `0x4EF990` | `b9 0b; f3 a5` — 11 dwords | = 44 |
| Graph | `0x4F64E0` | 0 | `0x4D3710` | single `c3` — copies nothing | = 0 |
| QTaskNote | `0x4EF850` | `0x41` | `0x4EF7F0` | copies a NUL-terminated string (`0x4F1180` + `repne scasb`/`rep movsb`) | capacity 65, not an exact match |

11 of 12 agree exactly; the one string type copies a length-dependent payload, so `0x41` is its buffer capacity. This is a direct instruction-level identity (E9 constant = width of the E7 copy for the same type), not an inference from the names `Real32`/`Int16`. It is the basis for the ten `..._ValueSizeGet` names.

#### 13.3.3 kindE7 copies the value out, kindE8 copies the value in

- `0x4EF990` (GraphEnvelope E7): `esi = arg0`, `edi = *(arg1)`, `rep movsd` ×11 → copies **arg0 to `*(arg1)`**.
- `0x4EF9B0` (GraphEnvelope E8): `edi = arg0`, `esi = *(arg1)`, `rep movsd` ×11 → copies **`*(arg1)` to arg0**.
- `0x4EFA90` (Int32/Real32 E7): `ecx = *(arg1)`, `eax = *(arg0)`, `mov [ecx],eax` → copies **arg0 through `*(arg1)`**.
- `0x4EFAA0` (Int32/Real32 E8): `ecx = *(arg1)`, `eax = arg0`, `mov [eax],edx` → copies **`*(arg1)` into arg0**.

So in both directions `arg0` is the value's own storage; E7 writes it out through `*(arg1)` and E8 reads it in from `*(arg1)`. That is the only reading consistent with all four bodies, and it is why the names use `ValueGet` (E7) and `ValueSet` (E8) rather than "read/write". The same pattern repeats for Real64 (`0x4EFF90`/`0x4EFFB0`), Int16 (`0x4F0170`/`0x4F0190`), Quat32 (`0x4EFEE0`/`0x4EFF00`), Real32x3 (`0x4EFB60`/`0x4EFB80`), Real32x9 (`0x4EFC90`/`0x4EFCB0`), Real64x3 (`0x4F0060`/`0x4F0080`), bool8 (`0x4F0560`/`0x4F0570`).

#### 13.3.4 The Graph type channel: one class word plus ten allocated phase ids

`Graph_Init 0x4FAA60` registers `"Graph"` (`0x4FAA6A push 0; 0x4FAA6B push 0; 0x4FAA6C call 0x5034D0; 0x4FAA77 push "Graph"`, `0x4FAA7C call 0x401900`, size `0x80`, parent = the value `0x5034D0(0,0)` returned) and stores its class word at `0x54E364` (`0x4FAA82`). It then calls `QTask_AllocateEvent(0)` **ten** times — `0x4FAA88`, `0x4FAA93`, `0x4FAA9E`, `0x4FAAA9`, `0x4FAAB4`, `0x4FAABF`, `0x4FAACA`, `0x4FAAD5`, `0x4FAAE0`, `0x4FAAEB`, each `push 0` before the call — and stores the returned id into ten one-byte slots in this order: `0x54E368` (`0x4FAA8E`), `0x54E369` (`0x4FAA99`), `0x54E36A` (`0x4FAAA4`), `0x54E36B` (`0x4FAAAF`), `0x54E36C` (`0x4FAABA`), `0x54E36F` (`0x4FAAC5`), `0x54E371` (`0x4FAAD0`), `0x54E370` (`0x4FAADB`), `0x54E36D` (`0x4FAAE6`), `0x54E36E` (`0x4FAAF0`). Seven of them have read getters: the six among the 33 (E369/E36B/E36C/E36D/E36E/E36F) plus `0x4F9270` = `mov al,[0x54E371]; ret` (`0x4F9270 a0 71 e3 54 00; c3`; the rest of `0x4F9270` is `0x4F9270`–`0x4F9275`), which is **not** among the 33 and is not yet transcribed anywhere in the candidate (`grep` over `engine/` and the rename map finds nothing) — flag for the Graph-channel pass. E368/E36A/E370 have no getter; they are only stored in `Graph_Init` and read in the Graph teardown `0x4FB110`: `0x4FB11B mov cl,[0x54E368]; 0x4FB122 call 0x4018C0`, `0x4FB133` (E36A), `0x4FB16D` (E370), with the other seven slots cleared the same way (`0x4FB127`–`0x4FB18B`), after `0x4FB110 mov eax,[0xA76D10]; 0x4FB116 call 0x4C1830` and before `0x4FB190 mov dx,[0x54E364]; 0x4FB198 call 0x401A20`. The ids are used (a) as the phase argument of `0x401400` with a class word: `AIGraph` uses class `0x54E83C` — `0x509001` (E369), `0x50901C` (E36C), `0x509036` (E36E), `0x509051` (E36F), `0x50906C` (E36B), `0x509086` (E36D); `Graph_Init` itself registers handlers at `0x4FAAF5`/`0x4FAB11`/`0x4FAB21`/`0x4FAB34`/`0x4FAB48`/`0x4FAB63` with its own class word — and (b) as the phase index of the class+phase dispatch table `0xA96AE0` (`0x4F96A2`, `0x4FA1D4`, `0x4FA2E1`, `0x4FA376`, and the inline load at `0x4F95DC`: `and eax,0xff; lea/shl *0x180; add class word; mov eax,[eax*4+0xA96AE0]; call eax`). Therefore "phase id of the Graph type" is proven; *which* phase (which script operation) each slot denotes is not, hence the slot byte in the name.

#### 13.3.5 `0x4EF830` calls a ret-only function

`0x4C1780` is exactly one byte `c3`. The checkout already records this (`class_registration.cpp`: "0x004C1780: ret-only task-list registrar, transcribed; it registers nothing"; `native_4c1780.hpp` flags the BN alias `AnimSound_LoadParameters` as unestablished). `0x4EF830` (`QTaskNote.kindE8`) reads `*arg1` and calls it; the call has no effect on memory. Retail therefore wires QTaskNote's value-set slot to a stub: a transcription must keep the `0x4C1780` call (retail flow) and must **not** "helpfully" copy the value.

#### 13.3.6 The two error callbacks

`0x936274` is read and invoked only by `Script_WarningLog 0x4AF810` (`0x4AF811` test, `0x4AF855 call dword [0x936274]`), which formats `"Warning:\n\n"` + message into `0xA84640` first. The installers pass the message-box routine `0x4988D0` (`0x498919`) or `0x491890` (`0x491835`), and the reset routine passes 0 (`0x4AF8D7`). `0x936270` (the other setter, `0x4AF870`) has **no reader at all** in the image; its only reference is the store itself. The two setters are not interchangeable and the first cannot be named.

#### 13.3.7 QTask's own phase ids at `0x548176`/`0x548177`

`QTask_InitPhaseHandlers 0x4C5900` allocates an event with value `0x181` twice and stores the two ids at `0x548176`/`0x548177`; the second is immediately used as the phase of `0x401400(classWord 0x54875C, phase, handler 0x4C5A10)` (`0x4C598D`–`0x4C599F`); both are cleared through `0x4018C0` in the teardown (`0x4C5AFB`, `0x4C5B06`). Both are then reused as phase arguments by unrelated class registrations (`0x427ED6`, `0x427EBE`, `0x40FEDB`, …). They are phase ids, not state bytes; the `Script_StateByteGet_*` placeholders misdescribe them.

#### 13.3.8 `0x4F9200`'s global `0xA76D10`

`0xA76D10` is written once (`0x4FAB9E` in `Graph_Init`) with the result of `0x4F1950(5)` (`0x4FAB94 push 5; 0x4FAB96 call 0x4F1950`). `0x4F1950` allocates `n*16+0xC` bytes with `0x4B0C60(n*16+0xC, 4)`, sets count at `+4`, data pointer at `+8`, and zero at `+0`; `0x4F1980` grows the array by `n` elements (recomputes the count, calls `0x4F1950`, copies the old elements). `Graph_Init` fills entries only via `0x4F1A80` (`0x4FABBD`, `0x4FABDF`, …) with type handles from `0x4F1010`/`0x4F0EF0`. The only other caller of the getter grows the array (`0x509123 call 0x4F9200; push eax; call 0x4F1980`). The Graph teardown releases it (`0x4FB110 mov eax,[0xA76D10]; 0x4FB116 call 0x4C1830`). So the getter returns a growable 16-byte-element array whose element meaning is not established by any instruction.

### 13.4 Proposed names — uniqueness

New basenames introduced by §13.2 (no collision among themselves or with the candidate's existing identifiers):

`Graph_GetArray`, `Graph_GetClassWord`, `Graph_PhaseIdGetE369`, `Graph_PhaseIdGetE36B`, `Graph_PhaseIdGetE36C`, `Graph_PhaseIdGetE36D`, `Graph_PhaseIdGetE36E`, `Graph_PhaseIdGetE36F`, `Graph_ValueSizeGet`, `GraphEnvelope_ValueGet`, `GraphEnvelope_ValueSet`, `GraphEnvelope_ValueSizeGet`, `QTaskNote_ValueSet`, `QTaskNote_ValueSizeGet`, `Script_ValueGet4`, `Script_ValueSize4Get`, `Script_ValueSize8Get`, `Real32x3_ValueSizeGet`, `Real32x9_ValueSizeGet`, `Real64x3_ValueSizeGet`, `Quat32_ValueSizeGet`, `Int16_ValueSizeGet`, `Script_TypeHandleGetReal64`, `Script_TypeHandleGetBool8`, `Script_PhaseIdGetE4`, `Script_PhaseIdGetE5`, `QTask_PhaseIdGet548176`, `QTask_PhaseIdGet548177`, `Error_WarningCallbackSet`.

Four identifiers keep their address suffix: `Script_Handler_4EF8B0`, `Script_SetConstant_4EFB00`, `Script_Handler_4EFA10`, `Error_CallbackSet_4AF870`.

### 13.5 Task B — the phase-record globals and the class-channel globals

| global | width | role proven by instructions | proposed name | status |
| --- | --- | --- | --- | --- |
| `0x0054F928` | `uint16` | Written once by `QTask_Init`: `0x401127 push 0x20`, `0x401129 push "QTask"`, `0x40112E call 0x401900`, `0x401138 66 a3 28 f9 54 00 mov [0x54F928],ax` — i.e. the class word of the `"QTask"` type registration. Read by `QTask_GetCurrentType 0x4018F0` (`66 a1 28 f9 54 00`) and at `0x401216` (`66 a1 28 f9 54 00; push eax; call 0x401A20`). Every later task type registers with it as parent (`0x4D9763`, `0x5310E4`, `0x530A84`, …). | `QTask_CurrentType` (the candidate already uses this; it is evidence-correct) | **PROVEN** |
| `0x0054F930` | `uint8[254][6]` | Base of the 6-byte records. Extent `[0x0054F930, 0x0054FF24)` = `0x5F4` bytes = **254 records**; `0x0054FF24` is a different object (16 bytes of a phase map initialised `mov dword [0x54FF24],-1` at `0x401100`, `-1` at `0x54FF28`/`0x54FF2C`/`0x54FF30`, byte-mapped by `0x401D30`, read by `0x4014E0`). Extent proof in `QTask_ClearEventTable 0x4011D0`: `mov eax,0x54F932`; loop `mov byte [eax],0; add eax,6; cmp eax,0x54FF26; jl` → 254 iterations. Allocator `0x4017C0`/`0x401810` scan the same table from `0x54F9F2` with id `0x20` (32) and bound `0x54FF26`. The allocated ids are used as the `phase` argument of `0x401400`/`0x401530` (§13.3.7), so "phase record" is the evidence-backed description. | record type `QTaskPhaseRecord` { `value` `+0`; `inUse` `+2`; unresolved `+3` }, table `QTask_PhaseTable`; current candidate alias `QTask_EventTable` | **PROVEN** (extent + role) |
| `0x0054F932` (record `+2`) | `uint8` per record | The allocation flag. Read by the allocator scans (`0x4017CA`, `0x40181A`: `cmp byte [ecx],0`, `ecx` stepping 6 from `0x54F9F2`); written `1` by `0x4017F7`, `0x401844`, `0x401875`, `0x40189E`; cleared by `0x4018C0` (`mov byte [eax*2+0x54F932],0`) and by the init clear loop `0x4011D5` for all 254 records. | `inUse` (field of `QTaskPhaseRecord`) | **PROVEN** |
| `0x0054F933` (record `+3`) | `uint8` per record | **Only ever written.** Exactly four references exist in the image, all stores: `0x401804` (`dl`=1 → 1), `0x401852` (→0), `0x401882` (`cl`=1 → 1), `0x4018AC` (→0). No load of this byte exists anywhere (an exhaustive immediate scan of the whole range `[0x54F930,0x54FF28)` in `.text` finds no other reference; there is also no base-address load of `0x54F930` other than the four store sites). The write pattern merely separates the entry points: `0x4017C0`/`0x401860` store 1, `0x401810`/`0x401890` store 0. | **UNRESOLVED** — keep the byte unnamed (`unresolved` field). What would settle it: a read instruction of the byte, or the class/QVM data definition of the phase record layout; neither exists in `igi.exe`. | **UNRESOLVED** |

Not part of Task B but proven while checking the extent: `0x54FF24` begins a separate 16-byte phase→class byte map (four dword `0xFFFFFFFF` stores at `0x401100`, `0x40110A`, `0x401111`, `0x401118`; the only reader is the loop `0x4014EA`–`0x401523`, which indexes `[ecx+0x54FF24]` and ends with `cmp ecx,0x10`, so exactly 16 entries), and `0xA969E0` holds a 254-byte class→phase map (`0x4010FB`–`0x401130`, count 254 = `0x5F4/6`); `0x401D30` writes both (`map16[arg2]=arg1&0xff`, `map254[arg1&0xff]=arg2&0xff`). This confirms that the record table ends at `0x54FF24` and that the records are indexed by an id up to 253.

### 13.6 LOUD flags (things the Lead must not miss)

1. **Candidate record count is wrong.** Retail has **254** records (`[0x54F930,0x54FF24)`, `0x5F4` bytes) and `QTask_ClearEventTable` clears all 254. The candidate declares `event_table_entry_count = 0x540U / 6U` = **224** (`task_types.hpp:66`), so `QTask_ClearEventTable` clears 30 records too few and every scan bound (`QTask_AllocateEvent`, `QTask_NewQTaskEventNoContextFn`) stops 30 ids early. Evidence: `0x4011D5`–`0x4011E0` loop and `0x54FF24` being a different object. Suggested constants: count 254 (`0x5F4/6`), end `0x54FF24`, scan bound `0x54FF26` (= end + 2, because the cursor addresses the `+2` byte).
2. **Allocator exhaustion diverges.** Retail `0x4017C0`/`0x401810` push `str.Unable_to_allocate_event` (`0x4017DB`/`0x40182B`), `call 0x4AF7B0` (the `Debug_Error` logger), and then `0x4017E8 eb fe` / `0x401838 eb fe` — a `jmp $` self-spin (`0x4017E5`/`0x401835` are only the `add esp,4` that follows the call, not a loop target). The candidate `QTask_AllocateEvent` returns 0 (`task_types.cpp`, the `event_id >= event_table_entry_count` branch). Keep the retail spin (or at least the `Debug_Error` call) per rule 10.13(1).
3. **`0x4C1780` is a one-byte `c3`.** Any name claiming behaviour for it (the BN alias `AnimSound_LoadParameters` is already flagged in `native_4c1780.hpp`) is unsupported; `0x4EF830` — one of the 33 — exists solely to call it. Do not let a rename of `0x4C1780` claim semantics, and do not "optimise" the call out of `0x4EF830`.
4. **Two record fields are write-only.** `+0` (16-bit value) and `+3` (the flag) have **no readers anywhere** in `igi.exe`. Candidate transcriptions must still perform those stores (they are observable side effects of `QTask_AllocateEvent`, the two mark functions and `TaskEventTable_ClearSlot`), but no semantic name can be justified for `+0`/`+3` yet.
5. **kindE6 is undecodable from code.** E6 handlers store 1, 3 and `0xB`; the slot is an attribute index consumed by the class/QVM channel whose invocation is not located. Three of the 33 therefore stay unresolved: `0x4EF8B0`, `0x4EFB00`, `0x4EFA10`.
6. **`0x4AF870` writes a callback nobody reads.** `0x936270` has exactly one reference in the image (the store). It cannot be named; if a "callback" name is demanded anyway, it must not claim a role.
7. **The E4/E5 and `0x548176`/`0x548177` globals are phase ids, not state bytes.** The `Script_StateByteGet_*` placeholders and the `Script_ParameterEventSlotGet*` family naming are both describing the same mechanism: ids allocated from the `0x54F930` phase table and consumed as the phase argument of `0x400`/`0x530` registrations. Names above make that explicit.
8. **E9 = size is now proven, not hypothesised.** Any earlier text that says otherwise (§9.2) is superseded by §13.3.2; the `..._ValueSizeGet` names are safe to use, and `0x427D70` genuinely serves both Int32 and Real32 because both are 4-byte values.
9. **A seventh Graph getter exists outside the 33.** `0x4F9270` (`a0 71 e3 54 00; c3` = `mov al,[0x54E371]; ret`) is read from `0x54E371` and is called from `0x5187F6`; it is not in the Lead's list of 33, has no address-suffixed placeholder anywhere in `engine/`, and is not transcribed. It belongs to the same Graph phase-id family as the six in §13.2 rows 3–8, so transcribe/naming it together with them.
10. **The Graph teardown proves the ten Graph slots are phase ids, not counters.** `0x4FB110`–`0x4FB1A0` reads each of the ten slots and clears it with `0x4018C0`, releases `0xA76D10` through `0x4C1830`, then unregisters the class word `0x54E364` through `0x401A20`. A candidate Graph shutdown must keep that order and those calls.

### 13.7 Evidence commands (reproducible)

```sh
# bodies (all 33)
r2 -q -e scr.color=0 -c 'pd 12 @ 0x4EF830' -c 'pd 16 @ 0x4EF990' -c 'pd 8 @ 0x4EFA90' -c q igi.exe
# registration contexts
r2 -q -e scr.color=0 -c 'pd 40 @ 0x4EF440' -c 'pd 46 @ 0x4EF640' -c q igi.exe
# Graph_Init (class word + ten allocated ids)
r2 -q -e scr.color=0 -c 'pd 50 @ 0x4FAA60' -c q igi.exe
# kind getter -> slot map
r2 -q -e scr.color=0 -c 'pd 1 @ 0x4F1B60' -c 'pd 1 @ 0x4F1B70' -c 'pd 1 @ 0x4F1B80' \
   -c 'pd 1 @ 0x4F1B90' -c 'pd 1 @ 0x4F1BA0' -c 'pd 1 @ 0x4F1BB0' -c q igi.exe
# kindE7 copy widths (E9 = size proof)
r2 -q -e scr.color=0 -c 'pd 10 @ 0x4EFF90' -c 'pd 10 @ 0x4F0170' -c 'pd 8 @ 0x4EFEE0' \
   -c 'pd 8 @ 0x4F0060' -c 'pd 10 @ 0x4EFC90' -c 'pd 8 @ 0x4F0560' -c q igi.exe
# ret-only 0x4C1780
r2 -q -e scr.color=0 -c 'pd 1 @ 0x4C1780' -c q igi.exe
# record table extent and field writes
r2 -q -e scr.color=0 -c 'pd 40 @ 0x4011C0' -c 'pd 110 @ 0x4017C0' -c q igi.exe
# error callback
r2 -q -e scr.color=0 -c 'pd 24 @ 0x4AF810' -c 'pd 10 @ 0x4AF8C8' -c q igi.exe
# QTask phase id slots
r2 -q -e scr.color=0 -c 'pd 40 @ 0x4C5960' -c 'pd 30 @ 0x4C5AE0' -c q igi.exe
```

Reference scans (call/push/immediate) are PE-aware byte scans; the exact method: find every `68 <le32(VA)>` for registration sites, enumerate every `e8` in `.text` and compute `target = site+5+rel` for call sites, and scan every 4-byte literal in a target range for reads/writes (used for `0x54F930`–`0x54FF28` and `0x936270`/`0x936274`).

### 13.8 Follow-up (Lead verification round): flag #2 resolution, and the seventh Graph getter in full

#### 13.8.1 Flag #2 — the divergence is real, but it is **not** in `native_401810`

The candidate routine whose exhaustion path returns 0 is **`QTask_AllocateEvent`** in `engine/src/retail/core/task_types.cpp` (its own provenance comment records the retail VA `0x004017C0`), not `native_401810.cpp` (`0x00401810`). Retail `0x004017C0`'s exhaustion branch, byte for byte:

- `0x4017D9 7c ef` = `jl 0x4017CA` (loop exit — the scan starts at `0x54F9F2`, id `0x20`)
- `0x4017DB 68 a8 61 53 00` = `push 0x5361A8`; the bytes at `0x5361A8` are exactly `55 6e 61 62 6c 65 20 74 6f 20 61 6c 6c 6f 63 61 74 65 20 65 76 65 6e 74` = `"Unable to allocate event"`
- `0x4017E0 e8 cb df 0a 00` = `call 0x4AF7B0` (the fatal dispatcher; `engine/src/retail/core/retail_error.cpp:8` records that VA as `Debug_Error`)
- `0x4017E5 83 c4 04 add esp,4`; `0x4017E8 eb fe jmp 0x4017E8`

The candidate instead has `if (event_id >= event_table_entry_count) { return 0U; }` in that branch. After `c5fdc2e1` (`event_table_entry_count = 0x5F4U/6U` = 254) the **bound arithmetic is now identical to retail**: the loop tests ids `0x20..0xFD` (222 ids), exactly retail's `0x54F9F2` → exit at `0x54FF26`; the "30 ids short" half of flag #2 is closed, only the exhaustion path remains. Two smaller byte-faithfulness points in the same routine: retail writes `+2` first (`0x4017F7`), then a single 16-bit store at `+0` (`0x4017FD 66 89 b1 30 f9 54 00`), then `+3` (`0x401804`), where the candidate writes `+0/+1`, `+2`, `+3`; and retail leaves the full id in EAX (`inc eax` from `0x20`) where the candidate returns `uint8_t`.

`native_401810.cpp` itself is behaviourally faithful for `0x00401810` (same scan, same stores, same `Debug_Error` + self-spin on exhaustion). Only its comments are wrong: its exhaustion comment names `0x0040183A-0x0040184F`, but that range is the **success** path — retail exhaustion is `0x40182B`–`0x401838` (`push 0x5361A8` at `0x40182B`, `call 0x4AF7B0` at `0x401830`, `add esp,4` at `0x401835`, `eb fe` at `0x401838`); and its success comment names `0x00401826-0x00401839`, where retail's success block is `0x40183A`–`0x401859` (`mov dx,[esp+4]` at `0x40183A`, `+2`=1 at `0x401844`, 16-bit `+0` store at `0x40184B`, `+3`=0 at `0x401852`, `ret` at `0x401859`).

#### 13.8.2 `0x4F9270` in full (flag #9)

- **Body:** `a0 71 e3 54 00; c3` = `mov al,[0x54E371]; ret`, `0x4F9270`–`0x4F9275` (6 bytes). No other instruction, no call.
- **Only caller in the image:** `0x5187F6`, inside the `"GraphNode"` class initialiser `0x518760` (`0x518760 sub esp,0x118`; `0x518775 push "GraphNode"` — string at `0x54ED0C`; `0x51877A call 0x401900`; class word stored at `0x54EC9C` by `0x518787`). The block is `0x5187F1 push 0x4D3710; 0x5187F6 call 0x4F9270; 0x5187FB mov cx,[0x54EC9C]; 0x518802 push eax; 0x518803 push ecx; 0x518804 call 0x401400` — the value is the **phase argument** of a class phase-handler registration whose handler is the ret-only `0x4D3710` (already transcribed as `native_4d3710.cpp`). The same initialiser registers `E5` at `0x5187C1` and `E4` at `0x5187DC` for the same class word, so the reader is a generic phase-id getter, not a Graph-only method.
- **`0x518760`'s caller:** exactly one, `0x518489`, immediately after `0x518484 call 0x4FAA60` (`Graph_Init`) in the class-registration cascade that begins at `0x5183D0`.
- **What `0x54E371` is:** the Graph class-word block **+ `0xD`**. `0x54E364` word = "Graph" class word (written at `0x4FAA82`); `0x54E366` word = image `0x0181` with **no code reference anywhere** (unexplained); `0x54E368`–`0x54E371` are ten one-byte ids, image `0xFF`.
- **Writer:** exactly one — `0x4FAAD0 a2 71 e3 54 00` in `Graph_Init 0x4FAA60`, immediately after `0x4FAACA call 0x4017C0` (allocated with argument 0). Teardown `0x4FB110` reads it at `0x4FB184` (`mov cl,[0x54E371]`) and clears it through `0x4018C0` at `0x4FB18B`.
- **Transcription status:** the writer is **not** transcribed (no `native_4faa60.cpp`; the six existing Graph getters document "runtime-written by the untranscribed Graph_Init 0x4FAA60"; `graph_state.hpp` marks them placeholders, `graph_state.cpp` does not exist — each getter file defines its own `g_...` byte). Adding `native_4f9270.cpp` now would be the same blocker as `0x401A60`: a reader whose retail writer is untranscribed, backed by a stand-in. Correct order is to transcribe `Graph_Init 0x4FAA60` first (it writes all ten slots, allocates the five-entry `0xA76D10` array, registers the class and its handlers), then `0x4F9270` reads real storage; the call site `0x518760` is also untranscribed, so end-to-end validation needs it too.
- **Do not import** the external catalogue name `Script_CallNativeHandler_22` (BN/Re-Agent catalogues, CFG 1.000): a 6-byte `mov al,[mem]; ret` body contains no call and the name asserts behaviour the bytes contradict.
