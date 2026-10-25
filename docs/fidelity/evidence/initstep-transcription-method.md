# InitStep transcription method — measurement procedure and known traps

Author: `chain-worker`. Scope: the 56 `AIGraph_InitStep_*` targets in `initstep-targets-sorted.txt`.
This note records the *measurement* procedure and the defects found while landing the first six units
(`0x530270`, `0x4EDCE0`, `0x52CA70`, `0x4F1E30`, `0x503D60`, `0x508D70`). It complements
`deferred-push-rule.md`, which covers one of the traps below.

## 1. Instruction counts: use ndisasm, not radare2 `pd`

**radare2 `pd` silently drops instructions.** On `0x00503D60` it returned **34** instructions covering
**121 of 135 bytes**; the true body is **36** instructions over all 135 bytes. The two it omitted were

```
0x00503DAC  668B0D6CE65400   mov cx,[0x0054e66c]
0x00503DBA  668B156CE65400   mov dx,[0x0054e66c]
```

which are precisely the instructions that define the `ecx` and `edx` the body later pushes. Trusting `pd`
would have produced a file missing two instructions plus a false "these registers are indeterminate, they
come from the caller" note. The listing *looked* complete.

**Always check that the byte accounting closes.** Decode the exact extent and confirm
`sum(instruction lengths) == extent size`. That check is what caught the defect; nothing else did.

```python
# authoritative sweep for a body, using the auditor's own VA->offset mapping
import importlib.util, sys, subprocess
spec = importlib.util.spec_from_file_location('nfa', 'scripts/native_fidelity_audit.py')
nfa = importlib.util.module_from_spec(spec); sys.modules['nfa'] = nfa; spec.loader.exec_module(nfa)
img = nfa.RetailImage()                      # parses the section table; va_to_off() returns None for BSS
start, size = 0x503D60, 135
off = img.va_to_off(start)
open('/tmp/s.bin','wb').write(img.data[off:off+size])
out = subprocess.run(['/opt/homebrew/bin/ndisasm','-b32','-o','0x%x'%start,'/tmp/s.bin'],
                     capture_output=True, text=True).stdout
rows = []
for line in out.splitlines():
    p = line.split()
    if len(p) >= 3:
        rows.append((int(p[0],16), len(p[1])//2, ' '.join(p[2:])))
assert sum(r[1] for r in rows) == size, 'byte accounting does not close'
```

The extent runs to the **`ret`** (or the final `jmp` of a tail call), not to the next function start; read the
byte after the terminator, and treat `0x90`/`0xCC` as padding that is not yours.

**The evidence list has at least one wrong count:** `0x530270` is **7** instructions (six `E8` calls plus one
`E9` tail jump), not the 9 listed. An ndisasm sweep over the 35-byte extent gives 7, and the compiled object
emits exactly 6 `call` + 1 `jmp`. Units `0x4EDCE0` (16), `0x52CA70` (24), `0x4F1E30` (27), `0x503D60` (36) and
`0x508D70` (38) all match the list. Regenerating the list with ndisasm is recommended.

## 2. Called vs address-taken: use `call_index()`, never a raw `E8` byte scan

`img.call_index()` decodes `E8` **at instruction boundaries** and returns 1729 entries for the image. A raw
scan for the byte `E8` returns **5049** — roughly 3300 phantoms, because `push 0x0041E8C0` encodes as
`68 C0 E8 41 00` and the middle bytes look like a call.

A raw scan *over*-reports, so a raw `False` happens to imply a real `False`. The first six units' conclusions
survived that way, but by luck of the error direction, not by method.

Rule in practice: an address that is only ever pushed is **address-taken only** — pass it as a cast integer and
**do not declare it**, because the declaration emits a relocation and the relocation is what the linker
reports. An address that is **called** is a real link edge and must be declared as a symbol. Both kinds appear
side by side in one body: `0x00508D70` pushes four handler-looking immediates, three are address-taken only
(`0x00508E00`, `0x00508E30`, `0x00508E40`) and the fourth is `QTask_NoOpHandler` `0x004D3710`, which **is**
called and is therefore declared.

## 3. Resolve every symbol by VA before naming anything

Look the address up in the tree's `// Native: <name>; retail VA 0x00XXXXXX` provenance lines and use the
tree's existing name and signature. `Sub_4C17C0` was once a stale placeholder for an already-implemented
routine two directories away. Names invented from a band's apparent purpose are guesses.

Also check **linkage**: several getters are defined only in their own `.cpp` inside `extern "C"` and have no
header (`Sys_GetGlobal16_505C80`, `Core_GetEventWord_4E7530`, `Sys_GetGlobal8_504DF0`, the five `0x4F0Fxx`
getters, `0x004FB5E0`, `0x004D9FB0`). They must be declared locally with matching linkage; declaring a
C++-linkage function inside `extern "C"` produces a name the linker cannot find.

## 4. `extern` vs definition for globals

A body that stores to a global defines it **only if no other file already defines it** — otherwise two
definitions of one symbol. Define with the value the image holds at that address:

- file-backed `.data` below the raw end `0x00552000`: read the bytes and carry that value
  (`0x0054E66C` → `0x0181`; `0x0054E830`/`0x0054E831` → `0xFF`; `0x0054F718` → `0x0181`);
- above the raw end: zero fill, define as `0`;
- `RetailImage.va_to_off()` returns `None` exactly for the zero-fill case, which is a useful signal.

Known discrepancy to be aware of, not introduced here: `sys/native_4f1e00.cpp`, `native_4f1e10.cpp` and
`native_4f1e20.cpp` define `g_54e160`, `g_54e150`, `g_54e158` with no initializer (i.e. 0) while the image
holds `0xFFFFFFFF` at all three addresses. `sys/native_5042a0.cpp` gets it right (`g_54e66c = 0x181U`).

## 5. Argument deferral and stack accounting

Reconcile every `add esp,N` against the pushes it releases, and derive each callee's arity from **its own
prologue's stack reads**, not from the push count at the call site. Full rule and the `0x401530`/`0x401900`
read tables are in `deferred-push-rule.md`.

Two consequences seen so far:

- an "ignored" push can still be the **next** callee's argument (`push 0x00503F20` before an argument-less
  getter, consumed as the following link's handler) — three independent instances: `0x0052CA70`, `0x00503D60`,
  `0x00508D70`; `core/native_427d90.cpp:67` already documented the same phenomenon;
- an ignored push can also be a **later** argument of the *same* call's callee: `TaskType_Register` reads four
  slots, so at `0x0052CA70` and `0x00503D60` its fourth argument is the second leftover push, value **0**.

## 6. Oddities to preserve, not tidy

- **Partial register writes.** `0x004D9FB0` is `mov al,byte [0x0054864A]; ret` — it writes **only AL**. At
  `0x00503DB3` the pushed event id is therefore `(return_of_0x00401400 & 0xFFFFFF00) | g_54864A`, keeping the
  upper 24 bits of the *previous call's return*. Widening the byte would be wrong whenever those bits are
  non-zero. The same body re-reads its class id from memory into `ax`, `cx`, `dx` and `ax` before four separate
  uses; that re-reading is preserved rather than hoisted into one local.
- **Differing orderings.** `0x004F1E30` writes its five globals in address order
  `0x54E160, 0x54E150, 0x54E154, 0x54E158, 0x54E15C` while registering them under
  `EnumInt32, EnumReal32, EnumString16, EnumString32, EnumString256` — the second store goes to the *lowest*
  global address.
- **Scheduling artifacts.** An `add esp` or an argument push may sit between a call and the store of its
  result; that changes only dead stack writes and needs a comment, not reordering.

## 7. Per-unit checklist

1. ndisasm sweep the exact extent to the terminator; confirm byte accounting closes; take the instruction count.
2. `call_index()` every `call`/`jmp` target and every pushed code address; split into called vs address-taken.
3. Resolve each target by VA in the tree; record the name and signature actually found.
4. Read strings and global initial values from the image via `RetailImage.va_to_off()`.
5. Reconcile every `add esp` and check the next callee's prologue for deferred pushes.
6. Write one file per VA in the domain folder, with the five-line provenance block and the blessed
   address-suffixed placeholder name.
7. Compile with `i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra`; record exit status, warning count and the
   undefined-symbol **names**.
8. Run `scripts/check_native_linkable.py` on the source path after `rm -f /tmp/_probe.o`.
9. Confirm the manifest extractor returns the VA.
10. Report VA, path, compile exit + warnings, undefined names, link status and which archive it was measured
    against. A count of zero from a failed build is not a count of zero.
