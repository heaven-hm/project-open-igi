# Band summary — `0x00419000`–`0x0041FFFF`: twelve registrars, eleven teardowns, and the 12-byte routine that made them linkable

Scope: the startup guard `0x00402930`'s registrar/teardown family inside `0x00419000`–`0x0041FFFF`.
Author: `flow-analyst`. Written 2026-10-25 after the Lead confirmed the band closed (all eleven teardowns
present, built, linkable and counted once by name in `igi1.exe`).
Every figure below is tagged with how it was obtained. Three classes of statement appear and are not mixed:

- **[M]** measured by me, with the instrument named; reproducible from the retail image.
- **[L]** recorded by the Lead (or by analyst through him) — quoted, not re-derived by me.
- **[D]** disagreement: my measurement does not reproduce the recorded figure. Stated as a disagreement, not tidied.

Retail artifact for every measurement: `igi.exe` SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, PE32 i386, ImageBase `0x400000`,
`.text` raw `0x1000` / VA `0x00401000`, file-backed to VA `0x00550000`; VA→file `0x1000 + (VA - 0x401000)`.
Candidate builds are named individually wherever a build figure is quoted, because the binary was rebuilt
repeatedly during this work and no hash may be carried forward.

---

## 1. The band's structure, as measurements

### 1.1 The guard's call set

**[M]** `0x00402930`–ret `0x00402FEB`: **314 direct call instructions**, **308 distinct callee targets**.
Counted by decoding every `E8` **at instruction boundaries** and resolving `v+5+rel32`.

Two instrument faults are recorded here because both were live in this session and both produce confident
wrong answers:

- An absolute **byte scan can never find** a call (the displacement is relative), and it can also **invent**
  one: at `0x0041E6F9` the second byte of the immediate of `68 C0 E8 41 00` (`push 0x0041E8C0`) decodes as an
  opcode, manufacturing the phantom callee `0x00ABE73F`. Any handler address containing an `0xE8` byte does
  this, so the contamination is systematic, not a one-off.
- **Line counts overstate instruction counts.** `ndisasm` prints the 8-byte `mov word [mem],imm16` across two
  output lines. I reported a 4-instruction `0x004C1830` (it is 5) and 10/7-instruction teardowns (they are
  9/6) before finding this; the counts in this document are instruction-boundary counts, cross-checked against
  radare2 `afij`.

**[L]** At his measurement against binary `0d88cfd0…`, the Lead counted 158 of the 308 targets with a file and
150 without, and noted that "308 is the guard's direct callee set, not the project's remaining work set".

### 1.2 Registration phase — descending, with one intruder

**[M]** `0x00402B79`–`0x00402C32` contains **38 call sites**. The registrar targets run **descending**:

```
0x42FF00 0x42FE40 0x42DCF0 0x42DAC0 0x42D2B0 0x42BEB0 0x42ACC0 0x427FF0 0x427D90 0x427830
0x4272A0 0x426D90 0x426630 0x425C80 0x424E80 0x4170A0 0x424B90 0x423FC0 0x4230A0 0x422B40
0x4223D0 0x421750 0x421430 0x420CC0 0x420300 0x41F340 0x41E690 0x41DC50 0x41D080 0x41C700
0x41B8E0 0x41B310 0x41AAF0 0x41A1E0 0x4199F0 0x419760 0x4190E0
```

**[M]** The order is descending **except for two entries**:
`0x004170A0` (an `0x0041xxxx` address sitting inside the descending `0x0042xxxx` run, between `0x00424E80`
and `0x00424B90`) and one trailing call whose target is `0x00402F50` — an address **inside the guard's own
extent**, i.e. a call to a local subroutine of `0x00402930` rather than to a registrar. Stated as
"descending" without the intruder, a later reader comparing the list against the guard would find a
mismatch and "fix" the wrong thing.

### 1.3 Teardown phase — ascending, with one intruder and one repeated non-teardown

**[M]** `0x00402C40`–`0x00402FEB` contains **159 call sites**. The first twenty-two targets in order:

```
0x4C1780 0x4192B0 0x4199B0 0x41A1A0 0x41AAC0 0x41B260 0x41B840 0x41C660 0x41D050 0x41DBF0
0x41E660 0x41F2A0 0x4201B0 0x420C90 0x4213E0 0x421700 0x4223A0 0x422B10 0x423050 0x423F90
0x424790 0x424E10 …
```

- **[M]** My **eleven** teardowns appear contiguously and in exactly ascending order
  `0x004192B0 … 0x0041F2A0`, and are immediately followed by **`0x004201B0`** — which is what independently
  identifies `0x004201B0` as the twelfth teardown (SlideBar's), in the other writer's range. The order I was
  asked to write them in and the order the guard calls them are the same list; that is a check on both.
- **[M]** One intruder: `0x00418AC0` appears after `0x004424E10`-range entries, out of the ascending run.
- **[M]** `0x004C1780` is called **repeatedly inside the teardown phase** (first, then interleaved) and is not
  a teardown of this family. Any port that treats "a call in the teardown phase" as "a teardown" will
  mis-classify it.

### 1.4 Alternation and population

**[M]** Registrars and teardowns **alternate in memory** throughout `0x00419000`–`0x0041FFFF`. The band holds
**12 registrars + 11 teardowns = 23 natives**. The twelfth teardown (`0x004201B0`) falls outside the band.

The twelve registrars (extent and instruction counts as recorded in each file's provenance line; getter,
id slot and container slot **[M]** re-measured for this summary by reading each file's own declarations):

| registrar VA | extent | ins | parent getter | class-id slot | container slot | name |
|---|---|---|---|---|---|---|
| `0x004190E0` | 269 B | 78 | `0x004018F0` | `0x00539ADC` | `0x0057BAF4` | MenuLoader |
| `0x00419760` | 137 B | 38 | `0x004018F0` | `0x00539AF0` | — (none) | BackgroundFX |
| `0x004199F0` | 496 B | 139 | `0x00424850` | `0x00539C28` | `0x0057BB88` | DialogWindow |
| `0x0041A1E0` | 389 B | 108 | `0x00424850` | `0x00539C40` | `0x0057BB90` | TypeWriterBox |
| `0x0041AAF0` | 421 B | 124 | `0x00424850` | `0x0053B580` | `0x0057BB98` | TextBox |
| `0x0041B310` | 383 B | 111 | `0x00424850` | `0x0053B58C` | `0x0057BBA8` | PictureBox |
| `0x0041B8E0` | 559 B | 162 | `0x00424850` | `0x0053B59C` | `0x0057BBB4` | ScrollListBox |
| `0x0041C700` | 460 B | 132 | `0x00424850` | `0x0053B5F4` | `0x0057BBBC` | AsciiBox |
| `0x0041D080` | 520 B | 149 | `0x00424850` | `0x0053B640` | `0x0057BBC4` | InputBox |
| `0x0041DC50` | 371 B | 101 | `0x00424850` | `0x0053B674` | `0x0057BBD0` | ToggleBox |
| `0x0041E690` | 558 B | 161 | `0x00424850` | `0x0053B694` | `0x0057BBDC` | ListBox |
| `0x0041F340` | 596 B | 173 | `0x00424850` | `0x0053B6BC` | `0x0057BBE4` | SlideBar |

The eleven teardowns. Every one is `TRANSCRIBED` + `LINKABLE` + `BUILD_VERIFIED`, `runtime UNOBSERVED`.
All are 37 bytes / 9 instructions except `0x004199B0` (25 bytes / 6 instructions). Each is 1 block, 0 edges.

| teardown VA | container | class-id slot | owner registrar | object sha256 (first 8) |
|---|---|---|---|---|
| `0x004192B0` | `0x0057BAF4` | `0x00539ADC` | MenuLoader `0x004190E0` | `c5708a34` |
| `0x004199B0` | **none** | `0x00539AF0` | BackgroundFX `0x00419760` | `d8796f4e` |
| `0x0041A1A0` | `0x0057BB88` | `0x00539C28` | DialogWindow `0x004199F0` | `603a75d2` |
| `0x0041AAC0` | `0x0057BB90` | `0x00539C40` | TypeWriterBox `0x0041A1E0` | `22f71d5d` |
| `0x0041B260` | `0x0057BB98` | `0x0053B580` | TextBox `0x0041AAF0` | `cf802f46` |
| `0x0041B840` | `0x0057BBA8` | `0x0053B58C` | PictureBox `0x0041B310` | `01854cad` |
| `0x0041C660` | `0x0057BBB4` | `0x0053B59C` | ScrollListBox `0x0041B8E0` | `ee7cb5a2` |
| `0x0041D050` | `0x0057BBBC` | `0x0053B5F4` | AsciiBox `0x0041C700` | `913c8f29` |
| `0x0041DBF0` | `0x0057BBC4` | `0x0053B640` | InputBox `0x0041D080` | `e4a482df` |
| `0x0041E660` | `0x0057BBD0` | `0x0053B674` | ToggleBox `0x0041DC50` | `9ad7c45a` |
| `0x0041F2A0` | `0x0057BBDC` | `0x0053B694` | ListBox `0x0041E690` | `848ef762` |

Teardown shape (ten of the eleven):

```
mov eax,[container]      ; 0x0057BBxx
push eax
call 0x004C1830          ; QTaskList_Delete, 12 bytes / 5 instructions
mov cx,[class-id]        ; 0x0053Bxxx
push ecx
call 0x00401A20          ; TaskType_Clear
add esp,8                ; covers BOTH caller pushes
mov word [class-id],0x181
ret
```

**[M]** The `add esp,8` is load-bearing to the *reading* of the body: `0x004C1830`'s internal `pop ecx` cleans
only its own push, so the caller's container push is still live at the `add`. A reader who assumes the callee
cleaned the argument will delete the `add esp,8` as redundant and shift the stack.

### 1.5 Parent getters — a division of the band, not a constant

**[M]** In my twelve: **ten use `0x00424850`** and **two use `0x004018F0`** (`0x004190E0`, `0x00419760`).

**[L]** Across the whole family the Lead records **four** getters, not one: `0x00424850` (files 1–6),
`0x004018F0` (7/8/10), `0x004D9FA0` (11–15), `0x004F45F0` (16).

**[M]** Two supporting measurements for that division: `0x00427D90` calls **`0x004D9FB0`** (the 11–15 family's
sibling entry, one `push`/`ret` apart from `0x004D9FA0`), and **both** `0x00427FF0` and `0x0042ACC0` call
**`0x004F45F0`**. So the `0x004F45F0` getter is shared by two registrars, one of which is unwritten — see
§2's corrections.

### 1.6 There is no canonical append shape

**[L]** Four distinct `lea` orderings were observed across the family; the tally stood at **4–1** at the last
count (one ordering used by four files, the others once each).

**[M]** Within my band the per-file records already refuse to generalise: `0x004199F0` computes its frame
pointers *before* the handler push; `0x0041AAF0`'s append 2 computes the low pointer *after* it; and
`0x0041B8E0`, `0x0041D080` and `0x0041E690` push the handler result first and compute both pointers
afterwards at `esp = frame-0x0C`. Taking any one file's arithmetic as a formula shifts every offset in the
others by `0xC`. Each of the 35 append sites in the band was resolved from its own `esp` at its own
instruction.

---

## 2. Measured absences, and three corrections

These are the entries a later reader would otherwise "fix".

### 2.1 `0x00427FF0` — the wide registrar **[M]**

- `0x00427FF0`–`0x00428266` = 630 bytes of code, **171 instructions** to the `ret`.
- Prologue `sub esp,0x898` (2200 bytes). **85 pushes** to the `ret`.
- **15 `add esp` cleanups**: `0x10`, `0x18` ×3, `0x38`, `0x40`, `0x8bc`, `0xc` ×8.
- Stack footprint 2200 + 85×4 = **2540 bytes** — matching the Lead's figure exactly.
- Parent getter `0x004F45F0`; registers name `0x0053BC80` with size `0x898`, id at `0x0053BB8C`.
- Slots: `0x0057BC88` (8 references), `0x0057BC94` (4 references).

**[D] Correction 1 — it is not the only framed registrar, and not the only one with a parameter container.**
`0x0042ACC0` (guard callee #124, called from `0x00402B97`; the Lead's own file, still unwritten) is:

- framed with **`sub esp,0x8e0`** (2272 bytes — larger than `0x898`);
- parent getter `0x004F45F0` (the same getter as `0x00427FF0`);
- registering a class of size `0x8e0` with name `0x0053BD1C`, id stored at `0x0057BCB8`;
- calling **`0x004F1950` once (container create)** and **`0x004F1A80` ten times (six-argument appends)**;
- carrying a second slot at **`0x0057BCBC`** (12 references in the first `0x800` bytes) beside the id slot
  `0x0057BCB8` (17 references).

So the family has **two** framed registrars and **two** registrars with parameter containers, and both share
the `0x004F45F0` getter. "No frame in the other fifteen" is a measurement only if the fifteen excludes
`0x0042ACC0`; as stated it is not. The 2540-byte stack figure for `0x00427FF0` stands.

### 2.2 `0x00427D90` — null handlers **[M], with a disagreement**

`0x00427D90`–`0x00427F29`, **114 instructions**, 16 link call sites (14 × `0x00401400`, 2 × `0x00401530`),
registration pair at the top, parent getter `0x004D9FB0`, id `0x0053BB6C`.

The function contains **11 `push 0` immediates**. Decomposed by position:

| role | count | sites |
|---|---|---|
| the registration's two zero words | 2 | `0x00427D90`, `0x00427D92` |
| **handler-position null** | **2** | `0x00427EA0` (with phase id 9), `0x00427F0C` (with phase id 4) |
| phase-id null | 1 | `0x00427DAE` (handler `0x00427F30`) |
| argument to a post-link setup call | 6 | before `0x00416840`, `0x00416860`, `0x004D14D0`, `0x004CEA20`, `0x004CEA10`, `0x004C56F0` |

**[D] Correction 2 — I measure 2 handler-position nulls, not 8.** The other six null immediates are
arguments to post-link setup routines, not registrations; the remaining one is a phase id. The Lead's item
"the eight measured null handlers in `0x00427D90` — registered nulls, not untranscribed addresses" is
therefore **not reproduced** by this measurement, and is left standing as a disagreement for reconciliation
rather than absorbed into this document. The substantive part of the claim — that a null registered as a
handler is a *registered null*, and must not be read as a missing transcription — is correct and is
independently visible at `0x00427EA0` and `0x00427F0C`, where the handler slot holds the literal 0 and the
phase id beside it is 9 and 4 respectively.

### 2.3 The stack-probe bracket **[M]** — both ends are in this band

The MSVC 6 stack probe `0x004A54A0` walks one `0x1000` page per step, so the threshold is 4096 bytes:

| frame | bytes | probes? | file |
|---|---|---|---|
| `0x13FC` | 5116 | **yes** | `native_41b8e0.cpp` (ScrollListBox) |
| `0xD38` | 3384 | **no** | `native_41e690.cpp` (ListBox) |
| `0x898` | 2200 | no | `0x00427FF0` |
| `0x1A8` | 424 | no | `native_41d080.cpp` (InputBox) |
| `0xF8` | 248 | no | `native_41c700.cpp` (AsciiBox) |

`0xD38` is the largest frame in the band that does **not** probe and `0x13FC` the smallest that does; the
bracket is fixed from both sides by measurements rather than by the threshold alone. `0x004A54A0` is a
compiler-emitted CRT helper, never hand-transcribed: the C++ frame replaces it, and the probe is deliberately
*not* declared as a callee (contrast the toolchain symbol `___chkstk_ms`, which the i686 compiler emits for
the 0x13FC frame and which does appear in that file's link evidence).

### 2.4 Other absences

- **[L]** `ConditionalSound` registers neither `0x004B8810` nor `0x004B8890`. (Both names appear in
  `native_424e80.cpp` and `native_425580.cpp`, outside my band.)
- **[M]** The handler addresses the registrars pass are **raw code immediates transcribed by nobody** and are
  passed as cast integers; they create no link dependency and stay recorded gaps in the tree, not stubs. In
  this band that is ten handler addresses per box registrar.

---

## 3. Population figures: the retired ones and the measured ones

- **[L]** **95 distinct handler immediates**, 8 of them shared across registrars — `0x00426860` in 4 files,
  `0x00426D30` in 3, `0x0042D9B0` in 3, and five more in 2 files each. **The earlier per-file handler figures
  counted *links*, not distinct handlers, and are retired.** A link count and a handler-population count are
  different measurements and the earlier numbers were the former labelled as the latter.
- **[L]** Kind A and Kind B are counted separately.
- **[M]** **Nine name registrations in `0x00427FF0` — confirmed.** Exactly 9 `call 0x004B8810` sites at
  `0x004281EE`, `0x004281FA`, `0x00428206`, `0x00428212`, `0x0042821E`, `0x0042822D`, `0x00428239`,
  `0x00428245`, `0x00428251`, with name pointers

  ```
  0x0053BC6C 0x0053BC58 0x0053BC40 0x0053BC24 0x0053BC04 0x0053BBE4 0x0053BBC4 0x0053BBB0 0x0053BB90
  ```

  (five in the `0x0053BCxx` family, four in the `0x0053BBxx` family) and numeric second arguments
  `{ runtime&0xffff, 0, 1, 0, 2, 3, 4, 5, 1 }`. **The numeric values repeat: 0 twice and 1 twice.** Those
  collisions are intentional in retail; a port that de-duplicates names or ids by their numeric argument
  will collide registrations that retail keeps separate.

---

## 4. The two laws of the reset convention

**[L]** Analyst proved both from the full 22 (the twelve registrars and their teardowns across the family).

**Law 1 — the container delete is present *iff* the registrar allocated a container.**
**[M]** Measured independently in this band and it holds exactly: 11 of the 12 registrars allocate a
container (all except BackgroundFX `0x00419760`), and exactly those 11 teardowns call `0x004C1830`;
`0x004199B0` (BackgroundFX's teardown) is the single body of the twelve without the call. A second,
independent confirmation came from the opposite direction: `0x004C1830` has **164 caller sites image-wide**,
of which **exactly 11** lie in `0x00419000`–`0x00420200` — the ten in-band ones plus `0x004201B6`, inside
SlideBar's teardown `0x004201B0`. Neither list was derived from the other, and they agree on all twelve.

**Law 2 — the `0x0181` reset is a contiguous ascending run that stops after MenuWindow.**
**[M]** All eleven in-band teardowns end `mov word [class-id],0x181`; the run's start, its contiguity in
memory and its stop point are analyst's measurement over the full 22. The reset is not decoration: it returns
the class to the "not registered" state after the type slot is cleared, and **no build or link check can
catch its absence** — the file compiles, links and runs with the store removed.

---

## 5. The linkage axis

**[M]** Authoritative measurement: compile each owner registrar, read `nm --defined-only`, take the symbol.

| owner registrar | class-id global | container global |
|---|---|---|
| `0x004190E0` MenuLoader | `__ZN3igi6retail4core8g_539adcE` | `__ZN3igi6retail4core8g_57baf4E` |
| `0x00419760` BackgroundFX | `_g_539af0` | — |
| `0x004199F0` DialogWindow | `_g_539c28` | `_g_57bb88` (and `_g_57bb80`, its arg6 pre-push) |
| `0x0041A1E0` TypeWriterBox | `_g_539c40` | `_g_57bb90` |
| `0x0041AAF0` TextBox | `_g_53b580` | `_g_57bb98` |
| `0x0041B310` PictureBox | `_g_53b58c` | `_g_57bba8` |
| `0x0041B8E0` ScrollListBox | `_g_53b59c` | `_g_57bbb4` |
| `0x0041C700` AsciiBox | `_g_53b5f4` | `_g_57bbbc` |
| `0x0041D080` InputBox | `_g_53b640` | `_g_57bbc4` |
| `0x0041DC50` ToggleBox | `_g_53b674` | `_g_57bbd0` |
| `0x0041E690` ListBox | `_g_53b694` | `_g_57bbdc` |
| `0x0041F340` SlideBar | `_g_53b6bc` | `_g_57bbe4` |

**Ten of the eleven owners define their globals with C linkage; exactly one — `0x004190E0` — uses the C++
namespace form.** So **the owner's file decides the linkage**, and the linkage is a property of the file, not
of the band, the subsystem, or the address range.

**This trap fired twice in the same hour, independently.** My teardown `0x004199B0` declared `0x00539AF0` as
`igi::retail::core::g_539af0` — the form that is correct for the MenuLoader owner `0x004190E0` — compiled
clean under i686 and host, passed the extractor, and failed only the link check:
`NOT LINKABLE — undefined in tree: ZN3igi6retail4core8g_539af0E`, because `0x00419760` defines it inside its
own `extern "C"` block as `_g_539af0`. Analyst hit the same class of failure on the same day. The working
rule is therefore procedural: **measure the owner's symbol before writing the declaration** (`nm
--defined-only` on the owner's compiled object), never infer it from a sibling file's source text. After the
failure I compiled all eleven owners and produced the table above *before* writing the remaining nine
teardowns; none of them failed.

---

## 6. Unresolved — listed as unresolved

1. **The handler ABI.** `FrameHandler` remains a **lossy placeholder**. **[L]** Analyst measured **82 of 169
   handlers reading arg2**, while **two walkers push only one argument**. Both measurements stand; neither
   settles the calling convention, and no port decision should cite either as if it did.
2. **`0x004B8920` and its own target `0x004BAB30`** — a **double gap**: the routine is untranscribed and the
   address it calls is untranscribed as well.
3. **`0x0042ACC0`** — the Lead's, **still unwritten**, and now measured to be the band family's widest member:
   2272-byte frame, `0x004F45F0` parent, one container create (`0x004F1950`) and ten appends (`0x004F1A80`),
   two slots (`0x0057BCB8`, `0x0057BCBC`). Its transcription is not a copy of the sixteen thin registrars.
4. **No runtime evidence exists for this band.** All 23 natives are `TRANSCRIBED` + `LINKABLE` +
   `BUILD_VERIFIED` and `runtime UNOBSERVED`. Nothing here is `VERIFIED`, and the band's
   registration/teardown ordering has not been observed in a running process.
5. **My three disagreements with the recorded figures**, left standing: `0x0042ACC0`'s frame and container
   (§2.1), the null-handler count in `0x00427D90` (§2.2), and the two phase-order intruders `0x004170A0` and
   `0x00418AC0` (§1.2, §1.3).
6. **A stale note in the tree:** `engine/include/igi/retail/core/qtask_list.hpp:18` and
   `engine/src/retail/core/qtask_list.cpp:18` still claim `0x004C1830` cannot be reconstructed. It is
   transcribed, in the build, and counted once by name.
7. **Outside this band:** 150 of the guard's 308 distinct callee targets still have no file at the Lead's last
   measurement (binary `0d88cfd0…`). The 23 natives here are a closed block, not the guard's remaining work.

---

## 7. What made this block reachable

The eleven teardowns are twelve-byte-wide link targets whose only interesting instruction is a call to
`0x004C1830`. That routine is **12 bytes, 1 block, 0 edges, 5 instructions**: `mov eax,[esp+4]; push eax;
call 0x004B0D10; pop ecx; ret`. It was transcribed **first**, before any teardown, so the eleven teardowns
could be ordinary linkable natives with a real callee instead of a second batch of deliberately-unbuilt
files. Its own `pop ecx` cleans only its own push — which is what makes the teardowns' `add esp,8` a measured
fact rather than an assumption.

Order of work, for the record: `0x004C1830`; then the eleven teardowns in ascending VA order, each body read
instruction by instruction rather than copied from the previous one, each reported individually.
