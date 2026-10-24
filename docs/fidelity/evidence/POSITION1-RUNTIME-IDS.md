# Position-1 registrar: runtime id boundary (retail capture)

Source of truth for this note: `docs/fidelity/evidence/runtime/tasktype-registration-trace.json`
(sha256 `e7d06f88e89c480a34c5a50ec897b19f44f6d4c854e05cbb61bd9cc6d1c2b5e6`), captured by the
Tester from retail `igi.exe` (sha256 `dea34892…5a01`) with a **spawn-suspended** typed Frida
probe (`probe_tasktype_register.js` sha256 `76a5d970…6a3d`) hooking `0x00402930` and
`0x00401900` before the first instruction. 279 calls, none before entry, none after return.
Launch: `igi.exe window level1`, cwd the game data root (proved via `Process.getCurrentDir()`
at registrar entry). Limitation: one idle level-1 run, no gameplay input.

## Settled by observation

- **279 registrations, ids contiguous 0..278, one per call.** This agrees with the engine's
  existing 279-class capture and disproves the older assumption that position 1 alone
  assigns "ids 0..43".
- Every callsite is an `E8` decoding to `0x00401900` (279 distinct sites, 279 distinct
  return addresses), so each id maps one-to-one onto a transcription site.
- First: id 0 `QTask`, size 32, arg3 `0x181`, site `0x401130` (return `0x401135`).
- Last: id 278 `MenuLoader`, size 292, arg3 `0x0`, site `0x4190FA` (return `0x4190FF`).
- **`AIGraph` is id 122** at site `0x508FBF` (inside `0x508FA0`/AIGraph_Init). That is the
  decisive number: the AI-graph registrar (chain position 1) does **not** end before id 43;
  it finishes at or after 122.
- Registration is not ordered by chain position: two ids (0 `QTask`, 125 `Flow`) come from
  sites below `0x402930` (`0x401130`, `0x401FD0`), while `AIGraph` lands at 122 inside
  position 1. So the call graph, not the flat chain list, determines ids.

## Consequence for the transcription plan

The position-1 subtree must produce ids 0..122 (123 classes) before `0x489560` (position 2)
runs. The inventory counted **127 unconditional `0x00401900` sites** in that subtree, so
roughly four sites are conditional/duplicate/dormant on this path. That gap is now a
precise, checkable target: transcribe `0x508F80` → `0x5183D0` group → `0x508FA0` and the
observed ids must fall 0..122 in the observed order.

## Still open

- Which exact sites are the four non-executing ones among the 127 (compare ordered id list
  against the subtree site list).
- Testers' capture does not include exit code (driver terminated the spawn after 30 s);
  irrelevant to id attribution, but it means this trace cannot support lifetime claims.

## Site attribution: 127 static sites vs 123 observed ids

Mapping the retail callsites to the inventory's function ranges
(`position1-tree.json`, 537 nodes keyed by VA with Start/Highest Address):

- **97 functions in the position-1 subtree call `0x00401900`**, and **all 97 have at
  least one observed registration site** - so no whole function is dormant on the
  level-1 path.
- Those 97 functions account for **123 observed registrations**, i.e. exactly ids
  **0..122**, matching the `AIGraph`=122 boundary.
- The inventory counted **127 static `0x00401900` call sites** in the same subtree,
  so the difference is **4 call sites**, not 4 functions: some functions contain
  more than one site and only one of them executes on this path (for example a
  class registered in one branch of an init function while another branch is
  skipped at level 1).

That converts the remaining uncertainty from "unknown which classes are missing"
into "find the 4 non-executing sites among 97 functions". It also confirms the
subtree boundary is right: attributing every observed registration to a tree
function needed no guesses, and no observed site fell outside the 537 nodes.

## Correction to carry forward

The earlier framing "position 1 registers ids 0..43" was inherited from a flat
chain table; the retail evidence says the executed ids are 0..122 assigned through
97 registrar functions. Any transcription that reproduces a 44-class group and
stops is wrong by 79 classes.

## Round 26: dormant-site identification - attempts and their failures

Goal: name the 4 static sites (of 127) that do not execute, by intersecting
per-function static sites with the 279 observed sites. Three approaches tried,
none produced the list; recording exactly why so the next attempt starts further
along:

1. `callsum.json` in the inventory only summarises the **top-level** functions
   (`402930, 508f80, 5183d0, 508fa0, 530270, 5309c0`), not all 97 registrar
   functions - it yields 1 static `0x401900` site for the whole tree, so it cannot
   answer the question.
2. Batch r2 disassembly (`af`, then `pdj N @ VA` for all 537 nodes in one
   invocation): 537 command pairs were issued but the output could not be split
   back into per-function JSON arrays with a line-oriented parser (0 blobs
   recovered). The method is sound; the parsing strategy is wrong. Next attempt
   should use `pdfj` per function individually, or `pdj` with an explicit output
   delimiter per invocation.
3. `axt 0x401900` after `af @ 0x401900`: returned zero xrefs, i.e. r2 needs a
   fuller analysis pass (`aaa`/`aac`) before call xrefs exist on this binary.

Status: the 4 dormant sites remain unidentified; the 123-vs-127 difference is
still interpreted (branch-skipped registrations inside functions that do run) and
that interpretation stays labelled as interpretation until the list exists.

Cost note for the next round: one `aaa` pass over 13 MB is cheaper than 97 separate
r2 launches; run it once, dump `axt` for every `0x401900` caller, then intersect
with observed sites.

## Round 27: no dormant sites exist - the "127" count was wrong

Single `aaa` pass over the retail image, then `axt 0x00401900`:

- **exactly 279 call sites** to `0x00401900` exist in the binary, and that set is
  **identical** to the 279 observed sites in the retail capture (`set(sites) ==
  set(observed)`, zero unobserved, zero extra).
- **123 of those sites fall inside the position-1 subtree**, and **all 123
  executed** (ids 0..122). Unobserved sites inside the tree: **none**.

Conclusion: the inventory's "127 unconditional `0x00401900` sites" in the subtree
was an overcount by four - there are no branch-skipped or dormant registration
sites at all at level 1. The earlier interpretation (four class registrations
skipped by branches) is therefore withdrawn; the correct statement is 123 sites,
123 registrations, ids 0..122, in 97 functions.

This also validates the capture mechanically: the static xref set and the runtime
call set agree exactly, so neither the probe nor the disassembly is missing a
registration site. That is the strongest form of agreement available here - two
independent methods producing the same 279-element set.

## Consequence

Position 1's acceptance criterion is now unambiguous and closed-form: transcribe
the 97 registrar functions reached from `0x508f80`; the candidate must emit ids
0..122 in the observed order with the observed (name, size, arg3) triples, and the
retail callsite -> id mapping file gives the expected sequence entry by entry.

## Round 28: signatures for the position-1 entry chain (BN, retail)

Read from the inventory tree (`position1-tree.json`, Binary Ninja definitions):

| VA | BN definition | blocks | extent end | callees |
| --- | --- | --- | --- | --- |
| `0x00508F80` | `int32_t()` | 1 | `0x00508F89` | 2 |
| `0x005183D0` | `int32_t()` | 1 | `0x00518541` | 74 |
| `0x00508FA0` | `void*()` | 1 | `0x00509292` | 43 |
| `0x00530270` | `int32_t()` | 1 | `0x00530292` | 7 |
| `0x00401900` | `int32_t(int32_t arg1, int32_t arg2, int16_t arg3, int32_t arg4)` | 18 | `0x00401A10` | 4 |

Two consequences worth recording before any body is written:

1. **`arg3` is `int16_t`**, not a pointer. The capture's "124 pointer-like values" are
   therefore just 16-bit values that happen to exceed a pointer heuristic - the
   third argument is a small integer discriminator (first observed value `0x181`),
   not a string table and not a parent pointer. The earlier "parent constant vs
   per-class string table" question is answered: neither, it is a 16-bit argument.
2. **`0x00508F80` returns `int32_t` while its tail-jump target `0x00508FA0` returns
   `void*`**, i.e. the thunk passes the tail callee's `eax` straight through. That
   ABI detail must be preserved when transcribing, and it means the thunk's return
   must be expressed without inventing a conversion step (a plain `return` of the
   tail call, keeping the retail's eax semantics).

Transcription order and blockers: `0x00508F80` is 9 bytes but cannot link until
`0x005183D0` and `0x00508FA0` exist as natives (74 and 43 callees respectively), so
the entry thunk lands **after** those two, not before. The next increments are
therefore inside the `0x5183D0` group, following the observed id order 0..122 rather
than the flat call order.
