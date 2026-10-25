# 0x004A5500 is the statically linked MSVC `strncpy` (identification record)

Artifact: retail `igi.exe`, SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, image base `0x00400000`.
Span: VA `0x004A5500`-`0x004A55FD`, 254 bytes, 35 blocks, 56 edges, 109 instructions (radare2 6.2.0 `af`/`afij` over the exact image;
first bytes `8b 4c 24 0c 57 85 c9 74 7a 56 53 8b d9 8b 74 24 14 f7 c6 03`).

## Why this is a CRT routine, not a game native

The body is the textbook MSVC `strncpy` and carries its two word-scan constants, which are not game data:

| Evidence | Address | Instruction |
| --- | --- | --- |
| magic | `0x004A5591` | `mov $0x7efefeff,%edx` |
| magic | `0x004A55A4` | `test $0x81010100,%eax` |
| source alignment probe | `0x004A5513` | `test $esi,3` on the source pointer |
| short-source zero fill | `0x004A55E7`-`0x004A55EF` | dword store loop that pads the destination to the requested count |
| return value | `0x004A55F6` | `mov 0x10(%esp),%eax` before the pops, i.e. the destination pointer |

Argument order `(char* destination, const char* source, size_t count)` is confirmed by all fifteen call sites, each of which pushes three
arguments and cleans twelve bytes: `0x0041A8CA`, `0x0041D88B`, `0x0044C795`, `0x004973FD`, `0x004A69C2`, `0x004B1E2B`, `0x004B1E52`,
`0x004B1EC3`, `0x004B8E60`, `0x004B8EC1`, `0x004B9082`, `0x004BCB76`, `0x004BDBC4`, `0x004C087C`, `0x00516F2E`. Three independent
non-QVM sites show the same shape, for example `0x004B1E28`-`0x004B1E30` (`push count`, `push source`, `push destination`, `call`, `add $0xc`)
and `0x00516F27`-`0x00516F33` (`push 0x10(%ebp)`, `push 0xc(%ebp)`, `push %edi`, `call`, `add $0xc`).

## How the port handles it

Retail calls its linked MSVC `strncpy` at `0x004A5500`; there is no game semantics to reconstruct. Following the precedent already recorded in
`engine/src/retail/error/error_show.cpp` for the linked MSVC `sprintf` at `0x004A53B3` ("The port defines no body for it; the header's
`using ::sprintf` binds the name to the candidate CRT symbol, which is what retail's callsites use too"), the port defines **no body** for
`0x004A5500` and binds the single QVM call site to the candidate CRT `std::strncpy`:

- `engine/src/retail/script/native_4bcae0.cpp` (call site `0x004BCB76`) documents the identification in the provenance block and calls
  `std::strncpy(destination, string, size - 1)`, which is the same operation, order and count the retail performs at that point.
- No `native_4a5500.cpp` implementation unit exists, deliberately: a hand-written body would duplicate a CRT routine the retail links in, which
  §10.13/§10.14 of `AGENTS.md` forbid and the `sprintf` precedent already settled.

This is a deviation from the "one file per VA" plan for this wave and is reported to the Lead rather than silently resolved.

## Open items

- The MSVC version/corpus the retail was linked against is not established; only the routine identity and its externally visible contract are.
- No runtime trace of `0x004A5500` was taken in this pass (status: identified statically; runtime UNOBSERVED).
- `0x005169B0`, which the earlier dependency table compared with `0x004A5500` on a `(source, destination)` shape, is **not** covered by this
  record and stays an unnamed VA whose behaviour still needs derivation; the comparison is superseded for `0x004A5500` by the `strncpy`
  identification above (third argument is a count, return value is the destination pointer).
