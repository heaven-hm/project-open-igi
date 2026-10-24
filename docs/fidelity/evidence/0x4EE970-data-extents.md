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
